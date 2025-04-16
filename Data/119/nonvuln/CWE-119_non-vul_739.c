static void FVMenuDetachAndRemoveGlyphs(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    char *buts[3];

    buts[0] = _("_Remove");
    buts[1] = _("_Cancel");
    buts[2] = NULL;

    if ( gwwv_ask(_("Detach & Remove Glyphs"),(const char **) buts,0,1,_("Are you sure you wish to remove these glyphs? This operation cannot be undone."))==1 )
return;

    FVDetachAndRemoveGlyphs((FontViewBase *) fv);
}