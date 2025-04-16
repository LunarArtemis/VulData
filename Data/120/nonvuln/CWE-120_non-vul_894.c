static int usecs_to_dev(unsigned long usecs, unsigned long const array[],
			int size)
{
	int i;
	int rval = -EINVAL;

	for (i = 0; i < size / 2; i++)
		if (array[(i * 2) + 1] >= usecs) {
			rval = array[i * 2];
			break;
		}

	return rval;
}