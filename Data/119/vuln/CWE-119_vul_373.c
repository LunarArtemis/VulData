private int
mconvert(struct magic_set *ms, struct magic *m)
{
	union VALUETYPE *p = &ms->ms_value;

	switch (m->type) {
	case FILE_BYTE:
		cvt_8(p, m);
		return 1;
	case FILE_SHORT:
		cvt_16(p, m);
		return 1;
	case FILE_LONG:
	case FILE_DATE:
	case FILE_LDATE:
		cvt_32(p, m);
		return 1;
	case FILE_QUAD:
	case FILE_QDATE:
	case FILE_QLDATE:
		cvt_64(p, m);
		return 1;
	case FILE_STRING:
	case FILE_BESTRING16:
	case FILE_LESTRING16: {
		/* Null terminate and eat *trailing* return */
		p->s[sizeof(p->s) - 1] = '\0';
		return 1;
	}
	case FILE_PSTRING: {
		char *ptr1 = p->s, *ptr2 = ptr1 + file_pstring_length_size(m);
		size_t len = file_pstring_get_length(m, ptr1);
		if (len >= sizeof(p->s))
			len = sizeof(p->s) - 1;
		while (len--)
			*ptr1++ = *ptr2++;
		*ptr1 = '\0';
		return 1;
	}
	case FILE_BESHORT:
		p->h = (short)((p->hs[0]<<8)|(p->hs[1]));
		cvt_16(p, m);
		return 1;
	case FILE_BELONG:
	case FILE_BEDATE:
	case FILE_BELDATE:
		p->l = (int32_t)
		    ((p->hl[0]<<24)|(p->hl[1]<<16)|(p->hl[2]<<8)|(p->hl[3]));
		cvt_32(p, m);
		return 1;
	case FILE_BEQUAD:
	case FILE_BEQDATE:
	case FILE_BEQLDATE:
		p->q = (uint64_t)
		    (((uint64_t)p->hq[0]<<56)|((uint64_t)p->hq[1]<<48)|
		     ((uint64_t)p->hq[2]<<40)|((uint64_t)p->hq[3]<<32)|
		     ((uint64_t)p->hq[4]<<24)|((uint64_t)p->hq[5]<<16)|
		     ((uint64_t)p->hq[6]<<8)|((uint64_t)p->hq[7]));
		cvt_64(p, m);
		return 1;
	case FILE_LESHORT:
		p->h = (short)((p->hs[1]<<8)|(p->hs[0]));
		cvt_16(p, m);
		return 1;
	case FILE_LELONG:
	case FILE_LEDATE:
	case FILE_LELDATE:
		p->l = (int32_t)
		    ((p->hl[3]<<24)|(p->hl[2]<<16)|(p->hl[1]<<8)|(p->hl[0]));
		cvt_32(p, m);
		return 1;
	case FILE_LEQUAD:
	case FILE_LEQDATE:
	case FILE_LEQLDATE:
		p->q = (uint64_t)
		    (((uint64_t)p->hq[7]<<56)|((uint64_t)p->hq[6]<<48)|
		     ((uint64_t)p->hq[5]<<40)|((uint64_t)p->hq[4]<<32)|
		     ((uint64_t)p->hq[3]<<24)|((uint64_t)p->hq[2]<<16)|
		     ((uint64_t)p->hq[1]<<8)|((uint64_t)p->hq[0]));
		cvt_64(p, m);
		return 1;
	case FILE_MELONG:
	case FILE_MEDATE:
	case FILE_MELDATE:
		p->l = (int32_t)
		    ((p->hl[1]<<24)|(p->hl[0]<<16)|(p->hl[3]<<8)|(p->hl[2]));
		cvt_32(p, m);
		return 1;
	case FILE_FLOAT:
		cvt_float(p, m);
		return 1;
	case FILE_BEFLOAT:
		p->l =  ((uint32_t)p->hl[0]<<24)|((uint32_t)p->hl[1]<<16)|
			((uint32_t)p->hl[2]<<8) |((uint32_t)p->hl[3]);
		cvt_float(p, m);
		return 1;
	case FILE_LEFLOAT:
		p->l =  ((uint32_t)p->hl[3]<<24)|((uint32_t)p->hl[2]<<16)|
			((uint32_t)p->hl[1]<<8) |((uint32_t)p->hl[0]);
		cvt_float(p, m);
		return 1;
	case FILE_DOUBLE:
		cvt_double(p, m);
		return 1;
	case FILE_BEDOUBLE:
		p->q =  ((uint64_t)p->hq[0]<<56)|((uint64_t)p->hq[1]<<48)|
			((uint64_t)p->hq[2]<<40)|((uint64_t)p->hq[3]<<32)|
			((uint64_t)p->hq[4]<<24)|((uint64_t)p->hq[5]<<16)|
			((uint64_t)p->hq[6]<<8) |((uint64_t)p->hq[7]);
		cvt_double(p, m);
		return 1;
	case FILE_LEDOUBLE:
		p->q =  ((uint64_t)p->hq[7]<<56)|((uint64_t)p->hq[6]<<48)|
			((uint64_t)p->hq[5]<<40)|((uint64_t)p->hq[4]<<32)|
			((uint64_t)p->hq[3]<<24)|((uint64_t)p->hq[2]<<16)|
			((uint64_t)p->hq[1]<<8) |((uint64_t)p->hq[0]);
		cvt_double(p, m);
		return 1;
	case FILE_REGEX:
	case FILE_SEARCH:
	case FILE_DEFAULT:
		return 1;
	default:
		file_magerror(ms, "invalid type %d in mconvert()", m->type);
		return 0;
	}
}