static void FVMenuShowMetrics(GWindow fvgw,struct gmenuitem *mi, GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(fvgw);
    GRect pos;
    GWindow gw;
    GWindowAttrs wattrs;
    struct md_data d;
    GGadgetCreateData gcd[7];
    GTextInfo label[6];
    int metrics = mi->mid==MID_ShowHMetrics ? fv->showhmetrics : fv->showvmetrics;

    d.fv = fv;
    d.done = 0;
    d.ish = mi->mid==MID_ShowHMetrics;

    memset(&wattrs,0,sizeof(wattrs));
    wattrs.mask = wam_events|wam_cursor|wam_utf8_wtitle|wam_undercursor|wam_restrict;
    wattrs.event_masks = ~(1<<et_charup);
    wattrs.restrict_input_to_me = 1;
    wattrs.undercursor = 1;
    wattrs.cursor = ct_pointer;
    wattrs.utf8_window_title = d.ish?_("Show H. Metrics"):_("Show V. Metrics");
    pos.x = pos.y = 0;
    pos.width =GDrawPointsToPixels(NULL,GGadgetScale(170));
    pos.height = GDrawPointsToPixels(NULL,130);
    gw = GDrawCreateTopWindow(NULL,&pos,md_e_h,&d,&wattrs);

    memset(&label,0,sizeof(label));
    memset(&gcd,0,sizeof(gcd));

    label[0].text = (unichar_t *) _("Baseline");
    label[0].text_is_1byte = true;
    gcd[0].gd.label = &label[0];
    gcd[0].gd.pos.x = 8; gcd[0].gd.pos.y = 8;
    gcd[0].gd.flags = gg_enabled|gg_visible|(metrics&fvm_baseline?gg_cb_on:0);
    gcd[0].gd.cid = fvm_baseline;
    gcd[0].creator = GCheckBoxCreate;

    label[1].text = (unichar_t *) _("Origin");
    label[1].text_is_1byte = true;
    gcd[1].gd.label = &label[1];
    gcd[1].gd.pos.x = 8; gcd[1].gd.pos.y = gcd[0].gd.pos.y+16;
    gcd[1].gd.flags = gg_enabled|gg_visible|(metrics&fvm_origin?gg_cb_on:0);
    gcd[1].gd.cid = fvm_origin;
    gcd[1].creator = GCheckBoxCreate;

    label[2].text = (unichar_t *) _("Advance Width as a Line");
    label[2].text_is_1byte = true;
    gcd[2].gd.label = &label[2];
    gcd[2].gd.pos.x = 8; gcd[2].gd.pos.y = gcd[1].gd.pos.y+16;
    gcd[2].gd.flags = gg_enabled|gg_visible|gg_utf8_popup|(metrics&fvm_advanceat?gg_cb_on:0);
    gcd[2].gd.cid = fvm_advanceat;
    gcd[2].gd.popup_msg = (unichar_t *) _("Display the advance width as a line\nperpendicular to the advance direction");
    gcd[2].creator = GCheckBoxCreate;

    label[3].text = (unichar_t *) _("Advance Width as a Bar");
    label[3].text_is_1byte = true;
    gcd[3].gd.label = &label[3];
    gcd[3].gd.pos.x = 8; gcd[3].gd.pos.y = gcd[2].gd.pos.y+16;
    gcd[3].gd.flags = gg_enabled|gg_visible|gg_utf8_popup|(metrics&fvm_advanceto?gg_cb_on:0);
    gcd[3].gd.cid = fvm_advanceto;
    gcd[3].gd.popup_msg = (unichar_t *) _("Display the advance width as a bar under the glyph\nshowing the extent of the advance");
    gcd[3].creator = GCheckBoxCreate;

    label[4].text = (unichar_t *) _("_OK");
    label[4].text_is_1byte = true;
    label[4].text_in_resource = true;
    gcd[4].gd.label = &label[4];
    gcd[4].gd.pos.x = 20-3; gcd[4].gd.pos.y = GDrawPixelsToPoints(NULL,pos.height)-35-3;
    gcd[4].gd.pos.width = -1; gcd[4].gd.pos.height = 0;
    gcd[4].gd.flags = gg_visible | gg_enabled | gg_but_default;
    gcd[4].gd.cid = 10;
    gcd[4].creator = GButtonCreate;

    label[5].text = (unichar_t *) _("_Cancel");
    label[5].text_is_1byte = true;
    label[5].text_in_resource = true;
    gcd[5].gd.label = &label[5];
    gcd[5].gd.pos.x = -20; gcd[5].gd.pos.y = gcd[4].gd.pos.y+3;
    gcd[5].gd.pos.width = -1; gcd[5].gd.pos.height = 0;
    gcd[5].gd.flags = gg_visible | gg_enabled | gg_but_cancel;
    gcd[5].creator = GButtonCreate;

    GGadgetsCreate(gw,gcd);

    GDrawSetVisible(gw,true);
    while ( !d.done )
	GDrawProcessOneEvent(NULL);
    GDrawDestroyWindow(gw);

    SavePrefs(true);
    GDrawRequestExpose(fv->v,NULL,false);
}