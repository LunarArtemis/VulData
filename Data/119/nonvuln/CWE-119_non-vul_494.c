static struct usb_feature_control_info *get_feature_control_info(int control)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(audio_feature_info); ++i) {
		if (audio_feature_info[i].control == control)
			return &audio_feature_info[i];
	}
	return NULL;
}