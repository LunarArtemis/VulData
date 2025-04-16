void MergeKernInfo(SplineFont *sf,EncMap *map) {
#ifndef __Mac
    static char wild[] = "*.{afm,tfm,ofm,pfm,bin,hqx,dfont,feature,feat,fea}";
    static char wild2[] = "*.{afm,amfm,tfm,ofm,pfm,bin,hqx,dfont,feature,feat,fea}";
#else
    static char wild[] = "*";	/* Mac resource files generally don't have extensions */
    static char wild2[] = "*";
#endif
    char *ret = gwwv_open_filename(_("Merge Feature Info"),NULL,
	    sf->mm!=NULL?wild2:wild,NULL);
    char *temp;

    if ( ret==NULL )
return;				/* Cancelled */
    temp = utf82def_copy(ret);

    if ( !LoadKerningDataFromMetricsFile(sf,temp,map))
	ff_post_error(_("Load of Kerning Metrics Failed"),_("Failed to load kern data from %s"), temp);
    free(ret); free(temp);
}