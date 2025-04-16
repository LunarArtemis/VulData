static int snd_usb_mixer_status_create(struct usb_mixer_interface *mixer)
{
	struct usb_endpoint_descriptor *ep;
	void *transfer_buffer;
	int buffer_length;
	unsigned int epnum;

	/* we need one interrupt input endpoint */
	if (get_iface_desc(mixer->hostif)->bNumEndpoints < 1)
		return 0;
	ep = get_endpoint(mixer->hostif, 0);
	if (!usb_endpoint_dir_in(ep) || !usb_endpoint_xfer_int(ep))
		return 0;

	epnum = usb_endpoint_num(ep);
	buffer_length = le16_to_cpu(ep->wMaxPacketSize);
	transfer_buffer = kmalloc(buffer_length, GFP_KERNEL);
	if (!transfer_buffer)
		return -ENOMEM;
	mixer->urb = usb_alloc_urb(0, GFP_KERNEL);
	if (!mixer->urb) {
		kfree(transfer_buffer);
		return -ENOMEM;
	}
	usb_fill_int_urb(mixer->urb, mixer->chip->dev,
			 usb_rcvintpipe(mixer->chip->dev, epnum),
			 transfer_buffer, buffer_length,
			 snd_usb_mixer_interrupt, mixer, ep->bInterval);
	usb_submit_urb(mixer->urb, GFP_KERNEL);
	return 0;
}