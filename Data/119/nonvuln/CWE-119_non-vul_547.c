static int mixer_ctl_feature_info(struct snd_kcontrol *kcontrol,
				  struct snd_ctl_elem_info *uinfo)
{
	struct usb_mixer_elem_info *cval = kcontrol->private_data;

	if (cval->val_type == USB_MIXER_BOOLEAN ||
	    cval->val_type == USB_MIXER_INV_BOOLEAN)
		uinfo->type = SNDRV_CTL_ELEM_TYPE_BOOLEAN;
	else
		uinfo->type = SNDRV_CTL_ELEM_TYPE_INTEGER;
	uinfo->count = cval->channels;
	if (cval->val_type == USB_MIXER_BOOLEAN ||
	    cval->val_type == USB_MIXER_INV_BOOLEAN) {
		uinfo->value.integer.min = 0;
		uinfo->value.integer.max = 1;
	} else {
		if (!cval->initialized) {
			get_min_max_with_quirks(cval, 0, kcontrol);
			if (cval->initialized && cval->dBmin >= cval->dBmax) {
				kcontrol->vd[0].access &= 
					~(SNDRV_CTL_ELEM_ACCESS_TLV_READ |
					  SNDRV_CTL_ELEM_ACCESS_TLV_CALLBACK);
				snd_ctl_notify(cval->head.mixer->chip->card,
					       SNDRV_CTL_EVENT_MASK_INFO,
					       &kcontrol->id);
			}
		}
		uinfo->value.integer.min = 0;
		uinfo->value.integer.max =
			(cval->max - cval->min + cval->res - 1) / cval->res;
	}
	return 0;
}