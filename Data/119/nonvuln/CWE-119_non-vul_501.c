int snd_usb_mixer_suspend(struct usb_mixer_interface *mixer)
{
	snd_usb_mixer_inactivate(mixer);
	return 0;
}