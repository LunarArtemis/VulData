static void FVMenuMakeNamelist(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    char buffer[1025];
    char *filename, *temp;
    FILE *file;

    snprintf(buffer, sizeof(buffer),"%s/%s.nam", getFontForgeUserDir(Config), fv->b.sf->fontname );
    temp = def2utf8_copy(buffer);
    filename = gwwv_save_filename(_("Make Namelist"), temp,"*.nam");
    free(temp);
    if ( filename==NULL )
return;
    temp = utf82def_copy(filename);
    file = fopen(temp,"w");
    free(temp);
    if ( file==NULL ) {
	ff_post_error(_("Namelist creation failed"),_("Could not write %s"), filename);
	free(filename);
return;
    }
    FVB_MakeNamelist((FontViewBase *) fv, file);
    fclose(file);
}