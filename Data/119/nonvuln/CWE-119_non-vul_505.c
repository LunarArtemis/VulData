void snd_usb_mixer_elem_free(struct snd_kcontrol *kctl)
{
	kfree(kctl->private_data);
	kctl->private_data = NULL;
}