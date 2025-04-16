int snd_usb_caiaq_audio_init(struct snd_usb_caiaqdev *dev)
{
	int i, ret;

	dev->n_audio_in  = max(dev->spec.num_analog_audio_in,
			       dev->spec.num_digital_audio_in) /
				CHANNELS_PER_STREAM;
	dev->n_audio_out = max(dev->spec.num_analog_audio_out,
			       dev->spec.num_digital_audio_out) /
				CHANNELS_PER_STREAM;
	dev->n_streams = max(dev->n_audio_in, dev->n_audio_out);

	debug("dev->n_audio_in = %d\n", dev->n_audio_in);
	debug("dev->n_audio_out = %d\n", dev->n_audio_out);
	debug("dev->n_streams = %d\n", dev->n_streams);

	if (dev->n_streams > MAX_STREAMS) {
		log("unable to initialize device, too many streams.\n");
		return -EINVAL;
	}

	ret = snd_pcm_new(dev->chip.card, dev->product_name, 0,
			dev->n_audio_out, dev->n_audio_in, &dev->pcm);

	if (ret < 0) {
		log("snd_pcm_new() returned %d\n", ret);
		return ret;
	}

	dev->pcm->private_data = dev;
	strcpy(dev->pcm->name, dev->product_name);

	memset(dev->sub_playback, 0, sizeof(dev->sub_playback));
	memset(dev->sub_capture, 0, sizeof(dev->sub_capture));

	memcpy(&dev->pcm_info, &snd_usb_caiaq_pcm_hardware,
			sizeof(snd_usb_caiaq_pcm_hardware));

	/* setup samplerates */
	dev->samplerates = dev->pcm_info.rates;
	switch (dev->chip.usb_id) {
	case USB_ID(USB_VID_NATIVEINSTRUMENTS, USB_PID_AK1):
	case USB_ID(USB_VID_NATIVEINSTRUMENTS, USB_PID_RIGKONTROL3):
	case USB_ID(USB_VID_NATIVEINSTRUMENTS, USB_PID_SESSIONIO):
	case USB_ID(USB_VID_NATIVEINSTRUMENTS, USB_PID_GUITARRIGMOBILE):
		dev->samplerates |= SNDRV_PCM_RATE_192000;
		/* fall thru */
	case USB_ID(USB_VID_NATIVEINSTRUMENTS, USB_PID_AUDIO2DJ):
	case USB_ID(USB_VID_NATIVEINSTRUMENTS, USB_PID_AUDIO4DJ):
	case USB_ID(USB_VID_NATIVEINSTRUMENTS, USB_PID_AUDIO8DJ):
		dev->samplerates |= SNDRV_PCM_RATE_88200;
		break;
	}

	snd_pcm_set_ops(dev->pcm, SNDRV_PCM_STREAM_PLAYBACK,
				&snd_usb_caiaq_ops);
	snd_pcm_set_ops(dev->pcm, SNDRV_PCM_STREAM_CAPTURE,
				&snd_usb_caiaq_ops);

	snd_pcm_lib_preallocate_pages_for_all(dev->pcm,
					SNDRV_DMA_TYPE_CONTINUOUS,
					snd_dma_continuous_data(GFP_KERNEL),
					MAX_BUFFER_SIZE, MAX_BUFFER_SIZE);

	dev->data_cb_info =
		kmalloc(sizeof(struct snd_usb_caiaq_cb_info) * N_URBS,
					GFP_KERNEL);

	if (!dev->data_cb_info)
		return -ENOMEM;

	for (i = 0; i < N_URBS; i++) {
		dev->data_cb_info[i].dev = dev;
		dev->data_cb_info[i].index = i;
	}

	dev->data_urbs_in = alloc_urbs(dev, SNDRV_PCM_STREAM_CAPTURE, &ret);
	if (ret < 0) {
		kfree(dev->data_cb_info);
		free_urbs(dev->data_urbs_in);
		return ret;
	}

	dev->data_urbs_out = alloc_urbs(dev, SNDRV_PCM_STREAM_PLAYBACK, &ret);
	if (ret < 0) {
		kfree(dev->data_cb_info);
		free_urbs(dev->data_urbs_in);
		free_urbs(dev->data_urbs_out);
		return ret;
	}

	return 0;
}