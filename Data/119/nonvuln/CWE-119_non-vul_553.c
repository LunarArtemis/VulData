static void build_feature_ctl_badd(struct usb_mixer_interface *mixer,
			      unsigned int ctl_mask, int control, int unitid,
			      const struct usbmix_name_map *badd_map)
{
	__build_feature_ctl(mixer, badd_map, ctl_mask, control,
			NULL, NULL, unitid, 0, 0);
}