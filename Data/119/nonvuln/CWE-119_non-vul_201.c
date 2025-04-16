static u8 bs_selector(int block_size)
{
	switch (block_size) {
	case 512:	    return 0x1;
	case 520:	    return 0x2;
	case 4096:	    return 0x3;
	case 4160:	    return 0x4;
	case 1073741824:    return 0x5;
	default:	    return 0;
	}
}