static void FVMenuEditTable(GWindow gw, struct gmenuitem *mi, GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    SFEditTable(fv->b.sf,
	    mi->mid==MID_Editprep?CHR('p','r','e','p'):
	    mi->mid==MID_Editfpgm?CHR('f','p','g','m'):
	    mi->mid==MID_Editmaxp?CHR('m','a','x','p'):
				  CHR('c','v','t',' '));
}