void shell_spaces_trim(char *str)
{
	u16_t len = shell_strlen(str);
	u16_t shift = 0U;

	if (!str) {
		return;
	}

	for (u16_t i = 0; i < len - 1; i++) {
		if (isspace((int)str[i])) {
			for (u16_t j = i + 1; j < len; j++) {
				if (isspace((int)str[j])) {
					shift++;
					continue;
				}

				if (shift > 0) {
					/* +1 for EOS */
					memmove(&str[i + 1],
						&str[j],
						len - shift + 1);
					len -= shift;
					shift = 0U;
				}

				break;
			}
		}
	}
}