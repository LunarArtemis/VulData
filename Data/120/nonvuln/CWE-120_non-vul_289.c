static void request_end(struct fuse_conn *fc, struct fuse_req *req)
__releases(fc->lock)
{
	void (*end) (struct fuse_conn *, struct fuse_req *) = req->end;
	req->end = NULL;
	list_del(&req->list);
	list_del(&req->intr_entry);
	req->state = FUSE_REQ_FINISHED;
	if (req->background) {
		if (fc->num_background == fc->max_background) {
			fc->blocked = 0;
			wake_up_all(&fc->blocked_waitq);
		}
		if (fc->num_background == fc->congestion_threshold &&
		    fc->connected && fc->bdi_initialized) {
			clear_bdi_congested(&fc->bdi, BLK_RW_SYNC);
			clear_bdi_congested(&fc->bdi, BLK_RW_ASYNC);
		}
		fc->num_background--;
		fc->active_background--;
		flush_bg_queue(fc);
	}
	spin_unlock(&fc->lock);
	wake_up(&req->waitq);
	if (end)
		end(fc, req);
	fuse_put_request(fc, req);
}