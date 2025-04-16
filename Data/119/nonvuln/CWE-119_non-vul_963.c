static void GCompletionCreatePopup(GCompletionField *gc) {
    int width, maxw, i;
    GWindowAttrs pattrs;
    GWindow base = gc->gl.gt.g.base;
    GDisplay *disp = GDrawGetDisplayOfWindow(base);
    GWindow root = GDrawGetRoot(disp);
    int bp = GBoxBorderWidth(base,gc->gl.gt.g.box);
    GRect pos, screen;
    GPoint pt;

    GDrawSetFont(base,gc->gl.gt.font);

    maxw = 0;
    for ( i=0; i<gc->ctot; ++i ) {
	width = GDrawGetTextWidth(base,gc->choices[i],-1);
	if ( width > maxw ) maxw = width;
    }
    maxw += 2*bp;
    pos.width = maxw; pos.height = gc->gl.gt.fh*gc->ctot+2*bp;
    if ( pos.width < gc->gl.gt.g.r.width )
	pos.width = gc->gl.gt.g.r.width;

    pattrs.mask = wam_events|wam_nodecor|wam_positioned|wam_cursor|
	    wam_transient|wam_verytransient/*|wam_bordwidth|wam_bordcol*/;
    pattrs.event_masks = -1;
    pattrs.nodecoration = true;
    pattrs.positioned = true;
    pattrs.cursor = ct_pointer;
    pattrs.transient = GWidgetGetTopWidget(base);
    pattrs.border_width = 1;
    pattrs.border_color = gc->gl.gt.g.box->main_foreground;

    GDrawGetSize(root,&screen);
    pt.x = gc->gl.gt.g.r.x;
    pt.y = gc->gl.gt.g.r.y + gc->gl.gt.g.r.height;
    GDrawTranslateCoordinates(base,root,&pt);
    if (  pt.y+pos.height > screen.height ) {
	if ( pt.y-gc->gl.gt.g.r.height-pos.height>=0 ) {
	    /* Is there more room above the widget ?? */
	    pt.y -= gc->gl.gt.g.r.height;
	    pt.y -= pos.height;
	} else if ( pt.x + gc->gl.gt.g.r.width + maxw <= screen.width ) {
	    pt.x += gc->gl.gt.g.r.width;
	    pt.y = 0;
	} else
	    pt.x = pt.y = 0;
    }
    pos.x = pt.x; pos.y = pt.y;

    gc->choice_popup = GWidgetCreateTopWindow(disp,&pos,popup_eh,gc,&pattrs);
    GDrawSetGIC(gc->choice_popup,GWidgetCreateInputContext(gc->choice_popup,gic_overspot|gic_orlesser),
	    gc->gl.gt.g.inner.x,gc->gl.gt.g.inner.y+gc->gl.gt.as);
    GDrawSetVisible(gc->choice_popup,true);
    /* Don't grab this one. User should be free to ignore it */
}