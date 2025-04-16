static int zerocopy_sg_from_iovec(struct sk_buff *skb, const struct iovec *from,
				  int offset, size_t count)
{
	int len = iov_length(from, count) - offset;
	int copy = skb_headlen(skb);
	int size, offset1 = 0;
	int i = 0;

	/* Skip over from offset */
	while (count && (offset >= from->iov_len)) {
		offset -= from->iov_len;
		++from;
		--count;
	}

	/* copy up to skb headlen */
	while (count && (copy > 0)) {
		size = min_t(unsigned int, copy, from->iov_len - offset);
		if (copy_from_user(skb->data + offset1, from->iov_base + offset,
				   size))
			return -EFAULT;
		if (copy > size) {
			++from;
			--count;
			offset = 0;
		} else
			offset += size;
		copy -= size;
		offset1 += size;
	}

	if (len == offset1)
		return 0;

	while (count--) {
		struct page *page[MAX_SKB_FRAGS];
		int num_pages;
		unsigned long base;
		unsigned long truesize;

		len = from->iov_len - offset;
		if (!len) {
			offset = 0;
			++from;
			continue;
		}
		base = (unsigned long)from->iov_base + offset;
		size = ((base & ~PAGE_MASK) + len + ~PAGE_MASK) >> PAGE_SHIFT;
		num_pages = get_user_pages_fast(base, size, 0, &page[i]);
		if ((num_pages != size) ||
		    (num_pages > MAX_SKB_FRAGS - skb_shinfo(skb)->nr_frags)) {
			for (i = 0; i < num_pages; i++)
				put_page(page[i]);
			return -EFAULT;
		}
		truesize = size * PAGE_SIZE;
		skb->data_len += len;
		skb->len += len;
		skb->truesize += truesize;
		atomic_add(truesize, &skb->sk->sk_wmem_alloc);
		while (len) {
			int off = base & ~PAGE_MASK;
			int size = min_t(int, len, PAGE_SIZE - off);
			__skb_fill_page_desc(skb, i, page[i], off, size);
			skb_shinfo(skb)->nr_frags++;
			/* increase sk_wmem_alloc */
			base += size;
			len -= size;
			i++;
		}
		offset = 0;
		++from;
	}
	return 0;
}