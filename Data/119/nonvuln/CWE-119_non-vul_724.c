static void FV_BiggerGlyphCache(FontView *fv, int gidcnt) {
    if ( fv->filled!=NULL )
	BDFOrigFixup(fv->filled,gidcnt,fv->b.sf);
}