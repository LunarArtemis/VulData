static void FVMenuRenameByNamelist(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    char **namelists = AllNamelistNames();
    int i;
    int ret;
    NameList *nl;
    extern int allow_utf8_glyphnames;

    for ( i=0; namelists[i]!=NULL; ++i );
    ret = gwwv_choose(_("Rename by NameList"),(const char **) namelists,i,0,_("Rename the glyphs in this font to the names found in the selected namelist"));
    if ( ret==-1 )
return;
    nl = NameListByName(namelists[ret]);
    if ( nl==NULL ) {
	IError("Couldn't find namelist");
return;
    } else if ( nl!=NULL && nl->uses_unicode && !allow_utf8_glyphnames) {
	ff_post_error(_("Namelist contains non-ASCII names"),_("Glyph names should be limited to characters in the ASCII character set, but there are names in this namelist which use characters outside that range."));
return;
    }
    SFRenameGlyphsToNamelist(fv->b.sf,nl);
    GDrawRequestExpose(fv->v,NULL,false);
}