static void check_no_speaker_on_headset(struct snd_kcontrol *kctl,
					struct snd_card *card)
{
	const char *names_to_check[] = {
		"Headset", "headset", "Headphone", "headphone", NULL};
	const char **s;
	bool found = false;

	if (strcmp("Speaker", kctl->id.name))
		return;

	for (s = names_to_check; *s; s++)
		if (strstr(card->shortname, *s)) {
			found = true;
			break;
		}

	if (!found)
		return;

	strlcpy(kctl->id.name, "Headphone", sizeof(kctl->id.name));
}