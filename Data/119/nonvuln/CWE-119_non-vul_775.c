static void FVMenuRmInstrTables(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    TtfTablesFree(fv->b.sf->ttf_tables);
    fv->b.sf->ttf_tables = NULL;
    if ( !fv->b.sf->changed ) {
	fv->b.sf->changed = true;
	FVSetTitles(fv->b.sf);
    }
}