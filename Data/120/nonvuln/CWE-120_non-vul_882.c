static unsigned long dev_to_usecs(int value, unsigned long const array[],
			int size)
{
	int i;
	int rval = -EINVAL;

	for (i = 0; i < size / 2; i++)
		if (array[i * 2] == value) {
			rval = array[(i * 2) + 1];
			break;
		}

	return rval;
}