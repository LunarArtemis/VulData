static int u2utf8_index(int pos,const char *start) {
    const char *pt = start;

    while ( --pos>=0 )
	utf8_ildb(&pt);
return( pt-start );
}