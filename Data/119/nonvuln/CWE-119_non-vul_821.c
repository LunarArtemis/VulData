static void FVMenuDeltas(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);

    if ( !hasFreeTypeDebugger())
return;
    DeltaSuggestionDlg(fv,NULL);
}