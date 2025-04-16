static inline void check_mapped_dB(const struct usbmix_name_map *p,
				   struct usb_mixer_elem_info *cval)
{
	if (p && p->dB) {
		cval->dBmin = p->dB->min;
		cval->dBmax = p->dB->max;
		cval->initialized = 1;
	}
}