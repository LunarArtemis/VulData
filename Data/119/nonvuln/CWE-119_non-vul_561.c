static int snd_usb_mixer_dev_free(struct snd_device *device)
{
	struct usb_mixer_interface *mixer = device->device_data;
	snd_usb_mixer_free(mixer);
	return 0;
}