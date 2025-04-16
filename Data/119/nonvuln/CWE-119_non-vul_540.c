static void volume_control_quirks(struct usb_mixer_elem_info *cval,
				  struct snd_kcontrol *kctl)
{
	struct snd_usb_audio *chip = cval->head.mixer->chip;
	switch (chip->usb_id) {
	case USB_ID(0x0763, 0x2030): /* M-Audio Fast Track C400 */
	case USB_ID(0x0763, 0x2031): /* M-Audio Fast Track C600 */
		if (strcmp(kctl->id.name, "Effect Duration") == 0) {
			cval->min = 0x0000;
			cval->max = 0xffff;
			cval->res = 0x00e6;
			break;
		}
		if (strcmp(kctl->id.name, "Effect Volume") == 0 ||
		    strcmp(kctl->id.name, "Effect Feedback Volume") == 0) {
			cval->min = 0x00;
			cval->max = 0xff;
			break;
		}
		if (strstr(kctl->id.name, "Effect Return") != NULL) {
			cval->min = 0xb706;
			cval->max = 0xff7b;
			cval->res = 0x0073;
			break;
		}
		if ((strstr(kctl->id.name, "Playback Volume") != NULL) ||
			(strstr(kctl->id.name, "Effect Send") != NULL)) {
			cval->min = 0xb5fb; /* -73 dB = 0xb6ff */
			cval->max = 0xfcfe;
			cval->res = 0x0073;
		}
		break;

	case USB_ID(0x0763, 0x2081): /* M-Audio Fast Track Ultra 8R */
	case USB_ID(0x0763, 0x2080): /* M-Audio Fast Track Ultra */
		if (strcmp(kctl->id.name, "Effect Duration") == 0) {
			usb_audio_info(chip,
				       "set quirk for FTU Effect Duration\n");
			cval->min = 0x0000;
			cval->max = 0x7f00;
			cval->res = 0x0100;
			break;
		}
		if (strcmp(kctl->id.name, "Effect Volume") == 0 ||
		    strcmp(kctl->id.name, "Effect Feedback Volume") == 0) {
			usb_audio_info(chip,
				       "set quirks for FTU Effect Feedback/Volume\n");
			cval->min = 0x00;
			cval->max = 0x7f;
			break;
		}
		break;

	case USB_ID(0x0d8c, 0x0103):
		if (!strcmp(kctl->id.name, "PCM Playback Volume")) {
			usb_audio_info(chip,
				 "set volume quirk for CM102-A+/102S+\n");
			cval->min = -256;
		}
		break;

	case USB_ID(0x0471, 0x0101):
	case USB_ID(0x0471, 0x0104):
	case USB_ID(0x0471, 0x0105):
	case USB_ID(0x0672, 0x1041):
	/* quirk for UDA1321/N101.
	 * note that detection between firmware 2.1.1.7 (N101)
	 * and later 2.1.1.21 is not very clear from datasheets.
	 * I hope that the min value is -15360 for newer firmware --jk
	 */
		if (!strcmp(kctl->id.name, "PCM Playback Volume") &&
		    cval->min == -15616) {
			usb_audio_info(chip,
				 "set volume quirk for UDA1321/N101 chip\n");
			cval->max = -256;
		}
		break;

	case USB_ID(0x046d, 0x09a4):
		if (!strcmp(kctl->id.name, "Mic Capture Volume")) {
			usb_audio_info(chip,
				"set volume quirk for QuickCam E3500\n");
			cval->min = 6080;
			cval->max = 8768;
			cval->res = 192;
		}
		break;

	case USB_ID(0x046d, 0x0807): /* Logitech Webcam C500 */
	case USB_ID(0x046d, 0x0808):
	case USB_ID(0x046d, 0x0809):
	case USB_ID(0x046d, 0x0819): /* Logitech Webcam C210 */
	case USB_ID(0x046d, 0x081b): /* HD Webcam c310 */
	case USB_ID(0x046d, 0x081d): /* HD Webcam c510 */
	case USB_ID(0x046d, 0x0825): /* HD Webcam c270 */
	case USB_ID(0x046d, 0x0826): /* HD Webcam c525 */
	case USB_ID(0x046d, 0x08ca): /* Logitech Quickcam Fusion */
	case USB_ID(0x046d, 0x0991):
	case USB_ID(0x046d, 0x09a2): /* QuickCam Communicate Deluxe/S7500 */
	/* Most audio usb devices lie about volume resolution.
	 * Most Logitech webcams have res = 384.
	 * Probably there is some logitech magic behind this number --fishor
	 */
		if (!strcmp(kctl->id.name, "Mic Capture Volume")) {
			usb_audio_info(chip,
				"set resolution quirk: cval->res = 384\n");
			cval->res = 384;
		}
		break;
	}
}