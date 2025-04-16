static struct fuse_forget_link *dequeue_forget(struct fuse_conn *fc,
					       unsigned max,
					       unsigned *countp)
{
	struct fuse_forget_link *head = fc->forget_list_head.next;
	struct fuse_forget_link **newhead = &head;
	unsigned count;

	for (count = 0; *newhead != NULL && count < max; count++)
		newhead = &(*newhead)->next;

	fc->forget_list_head.next = *newhead;
	*newhead = NULL;
	if (fc->forget_list_head.next == NULL)
		fc->forget_list_tail = &fc->forget_list_head;

	if (countp != NULL)
		*countp = count;

	return head;
}