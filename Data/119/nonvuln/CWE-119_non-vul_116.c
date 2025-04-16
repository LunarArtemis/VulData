static void get_vers(char *p)
{
	int i, j, k;

	i = j = k = 0;
	if (sscanf(p, "%d.%d.%d", &i, &j, &k) != 3)
		if (sscanf(p, "%d.%d", &i, &j) != 2)
			sscanf(p, "%d", &i);
	parse.abc_vers = (i << 16) + (j << 8) + k;
}