static void usb_mixer_selector_elem_free(struct snd_kcontrol *kctl)
{
	int i, num_ins = 0;

	if (kctl->private_data) {
		struct usb_mixer_elem_info *cval = kctl->private_data;
		num_ins = cval->max;
		kfree(cval);
		kctl->private_data = NULL;
	}
	if (kctl->private_value) {
		char **itemlist = (char **)kctl->private_value;
		for (i = 0; i < num_ins; i++)
			kfree(itemlist[i]);
		kfree(itemlist);
		kctl->private_value = 0;
	}
}