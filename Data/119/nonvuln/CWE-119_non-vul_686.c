void MenuExit(GWindow UNUSED(base), struct gmenuitem *UNUSED(mi), GEvent *e) {
    if ( e==NULL )	/* Not from the menu directly, but a shortcut */
	_MenuExit(NULL);
    else
	DelayEvent(_MenuExit,NULL);
}