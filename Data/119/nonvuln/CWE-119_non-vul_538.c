static void snd_usb_mixer_dump_cval(struct snd_info_buffer *buffer,
				    struct usb_mixer_elem_list *list)
{
	struct usb_mixer_elem_info *cval = mixer_elem_list_to_info(list);
	static char *val_types[] = {"BOOLEAN", "INV_BOOLEAN",
				    "S8", "U8", "S16", "U16"};
	snd_iprintf(buffer, "    Info: id=%i, control=%i, cmask=0x%x, "
			    "channels=%i, type=\"%s\"\n", cval->head.id,
			    cval->control, cval->cmask, cval->channels,
			    val_types[cval->val_type]);
	snd_iprintf(buffer, "    Volume: min=%i, max=%i, dBmin=%i, dBmax=%i\n",
			    cval->min, cval->max, cval->dBmin, cval->dBmax);
}