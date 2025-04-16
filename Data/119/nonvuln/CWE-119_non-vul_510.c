void snd_usb_mixer_elem_init_std(struct usb_mixer_elem_list *list,
				 struct usb_mixer_interface *mixer,
				 int unitid)
{
	list->mixer = mixer;
	list->id = unitid;
	list->dump = snd_usb_mixer_dump_cval;
#ifdef CONFIG_PM
	list->resume = restore_mixer_value;
#endif
}