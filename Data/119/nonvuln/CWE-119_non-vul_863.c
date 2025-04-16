static GFont *FVCheckFont(FontView *fv,int type) {
    FontRequest rq;

    if ( fv->fontset[type]==NULL ) {
	memset(&rq,0,sizeof(rq));
	rq.utf8_family_name = fv_fontnames;
	rq.point_size = fv_fontsize;
	rq.weight = 400;
	rq.style = 0;
	if (type&_uni_italic)
	    rq.style |= fs_italic;
	if (type&_uni_vertical)
	    rq.style |= fs_vertical;
	fv->fontset[type] = GDrawInstanciateFont(fv->v,&rq);
    }
return( fv->fontset[type] );
}