static void snd_usb_mixer_inactivate(struct usb_mixer_interface *mixer)
{
	usb_kill_urb(mixer->urb);
	usb_kill_urb(mixer->rc_urb);
}