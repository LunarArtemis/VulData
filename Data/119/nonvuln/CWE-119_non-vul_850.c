static enum merge_type SelMergeType(GEvent *e) {
    if ( e->type!=et_mouseup )
return( mt_set );

return( ((e->u.mouse.state&ksm_shift)?mt_merge:0) |
	((e->u.mouse.state&ksm_control)?mt_restrict:0) );
}