static int _gtextfield_editcmd(GGadget *g,enum editor_commands cmd) {
    if ( gtextfield_editcmd(g,cmd)) {
	_ggadget_redraw(g);
	GTPositionGIC((GTextField *) g);
return( true );
    }
return( false );
}