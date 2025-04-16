static void FVMenuCopyFrom(GWindow UNUSED(gw), struct gmenuitem *mi, GEvent *UNUSED(e)) {
    /*FontView *fv = (FontView *) GDrawGetUserData(gw);*/

    if ( mi->mid==MID_CharName )
	copymetadata = !copymetadata;
    else if ( mi->mid==MID_TTFInstr )
	copyttfinstr = !copyttfinstr;
    else
	onlycopydisplayed = (mi->mid==MID_DisplayedFont);
    SavePrefs(true);
}