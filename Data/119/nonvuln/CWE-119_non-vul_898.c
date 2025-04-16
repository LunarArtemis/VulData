char *GetPostScriptFontName(char *dir, int mult) {
    unichar_t *ret;
    char *u_dir;
    char *temp;

    u_dir = def2utf8_copy(dir);
    ret = FVOpenFont(_("Open Font"), u_dir,mult);
    temp = u2def_copy(ret);

    free(ret);
return( temp );
}