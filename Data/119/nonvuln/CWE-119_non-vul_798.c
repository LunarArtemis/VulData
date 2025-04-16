static void FVMenuChangeMMBlend(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    MMSet *mm = fv->b.sf->mm;

    if ( mm==NULL || mm->apple )
return;
    MMChangeBlend(mm,fv,false);
}