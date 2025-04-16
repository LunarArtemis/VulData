int snd_usb_mixer_vol_tlv(struct snd_kcontrol *kcontrol, int op_flag,
			 unsigned int size, unsigned int __user *_tlv)
{
	struct usb_mixer_elem_info *cval = kcontrol->private_data;
	DECLARE_TLV_DB_MINMAX(scale, 0, 0);

	if (size < sizeof(scale))
		return -ENOMEM;
	if (cval->min_mute)
		scale[0] = SNDRV_CTL_TLVT_DB_MINMAX_MUTE;
	scale[2] = cval->dBmin;
	scale[3] = cval->dBmax;
	if (copy_to_user(_tlv, scale, sizeof(scale)))
		return -EFAULT;
	return 0;
}