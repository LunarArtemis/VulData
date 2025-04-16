static int check_matrix_bitmap(unsigned char *bmap,
			       int ich, int och, int num_outs)
{
	int idx = ich * num_outs + och;
	return bmap[idx >> 3] & (0x80 >> (idx & 7));
}