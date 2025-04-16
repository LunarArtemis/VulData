static ssize_t fuse_dev_do_read(struct fuse_conn *fc, struct file *file,
				struct fuse_copy_state *cs, size_t nbytes)
{
	int err;
	struct fuse_req *req;
	struct fuse_in *in;
	unsigned reqsize;

 restart:
	spin_lock(&fc->lock);
	err = -EAGAIN;
	if ((file->f_flags & O_NONBLOCK) && fc->connected &&
	    !request_pending(fc))
		goto err_unlock;

	request_wait(fc);
	err = -ENODEV;
	if (!fc->connected)
		goto err_unlock;
	err = -ERESTARTSYS;
	if (!request_pending(fc))
		goto err_unlock;

	if (!list_empty(&fc->interrupts)) {
		req = list_entry(fc->interrupts.next, struct fuse_req,
				 intr_entry);
		return fuse_read_interrupt(fc, cs, nbytes, req);
	}

	if (forget_pending(fc)) {
		if (list_empty(&fc->pending) || fc->forget_batch-- > 0)
			return fuse_read_forget(fc, cs, nbytes);

		if (fc->forget_batch <= -8)
			fc->forget_batch = 16;
	}

	req = list_entry(fc->pending.next, struct fuse_req, list);
	req->state = FUSE_REQ_READING;
	list_move(&req->list, &fc->io);

	in = &req->in;
	reqsize = in->h.len;
	/* If request is too large, reply with an error and restart the read */
	if (nbytes < reqsize) {
		req->out.h.error = -EIO;
		/* SETXATTR is special, since it may contain too large data */
		if (in->h.opcode == FUSE_SETXATTR)
			req->out.h.error = -E2BIG;
		request_end(fc, req);
		goto restart;
	}
	spin_unlock(&fc->lock);
	cs->req = req;
	err = fuse_copy_one(cs, &in->h, sizeof(in->h));
	if (!err)
		err = fuse_copy_args(cs, in->numargs, in->argpages,
				     (struct fuse_arg *) in->args, 0);
	fuse_copy_finish(cs);
	spin_lock(&fc->lock);
	req->locked = 0;
	if (req->aborted) {
		request_end(fc, req);
		return -ENODEV;
	}
	if (err) {
		req->out.h.error = -EIO;
		request_end(fc, req);
		return err;
	}
	if (!req->isreply)
		request_end(fc, req);
	else {
		req->state = FUSE_REQ_SENT;
		list_move_tail(&req->list, &fc->processing);
		if (req->interrupted)
			queue_interrupt(fc, req);
		spin_unlock(&fc->lock);
	}
	return reqsize;

 err_unlock:
	spin_unlock(&fc->lock);
	return err;
}