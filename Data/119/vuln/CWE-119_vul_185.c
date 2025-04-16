static int wdm_post_reset(struct usb_interface *intf)
{
	struct wdm_device *desc = wdm_find_device(intf);
	int rv;

	clear_bit(WDM_RESETTING, &desc->flags);
	rv = recover_from_urb_loss(desc);
	mutex_unlock(&desc->wlock);
	mutex_unlock(&desc->rlock);
	return 0;
}