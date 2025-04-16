static enum fchooserret _FVSaveAsFilterFunc(GGadget *g,struct gdirentry *ent, const unichar_t *dir)
{
    char* n = u_to_c(ent->name);
    int ew = endswithi( n, "sfd" ) || endswithi( n, "sfdir" );
    if( ew )
	return fc_show;
    if( ent->isdir )
	return fc_show;
    return fc_hide;
}