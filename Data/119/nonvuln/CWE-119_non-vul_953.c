static void GTextFieldComplete(GTextField *gt,int from_tab) {
    GCompletionField *gc = (GCompletionField *) gt;
    unichar_t **ret;
    int i, len, orig_len;
    unichar_t *pt1, *pt2, ch;
    /* If not from_tab, then the textfield has already been changed and we */
    /* must mark it as such (but don't mark twice) */

    ret = (gc->completion)(&gt->g,from_tab);
    if ( ret==NULL || ret[0]==NULL ) {
	if ( from_tab )
	    GDrawBeep(NULL);
	else
	    GTextFieldChanged(gt,-1);
	free(ret);
    } else {
	orig_len = u_strlen(gt->text);
	len = u_strlen(ret[0]);
	for ( i=1; ret[i]!=NULL; ++i ) {
	    for ( pt1=ret[0], pt2=ret[i]; *pt1==*pt2 && pt1-ret[0]<len ; ++pt1, ++pt2 );
	    len = pt1-ret[0];
	}
	if ( orig_len!=len ) {
	    ch = ret[0][len]; ret[0][len] = '\0';
	    GTextFieldSetTitle(&gt->g,ret[0]);
	    ret[0][len] = ch;
	    if ( !from_tab )
		GTextFieldSelect(&gt->g,orig_len,len);
	    GTextFieldChanged(gt,-1);
	} else if ( !from_tab )
	    GTextFieldChanged(gt,-1);
	if ( ret[1]!=NULL ) {
	    gc->choices = ret;
	    gc->selected = -1;
	    if ( from_tab ) GDrawBeep(NULL);
	    qsort(ret,i,sizeof(unichar_t *),ucmp);
	    gc->ctot = i;
	    if ( i>=MAXLINES ) {
		/* Try to shrink the list by just showing initial stubs of the */
		/*  names with multiple entries with a common next character */
		/* So if we have matched against "a" and we have "abc", "abd" "acc" */
		/*  the show "ab..." and "acc" */
		unichar_t **ret2=NULL, last_ch = -1;
		int cnt, doit, type2=false;
		for ( doit=0; doit<2; ++doit ) {
		    for ( i=cnt=0; ret[i]!=NULL; ++i ) {
			if ( last_ch!=ret[i][len] ) {
			    if ( doit && type2 ) {
				int c2 = cnt/MAXBRACKETS, c3 = cnt%MAXBRACKETS;
				if ( ret[i][len]=='\0' )
		    continue;
				if ( c3==0 ) {
				    ret2[c2] = calloc((len+MAXBRACKETS+2+4+1),sizeof(unichar_t));
				    memcpy(ret2[c2],ret[i],len*sizeof(unichar_t));
				    ret2[c2][len] = '[';
				}
				ret2[c2][len+1+c3] = ret[i][len];
				uc_strcpy(ret2[c2]+len+2+c3,"] ...");
			    } else if ( doit ) {
				ret2[cnt] = malloc((u_strlen(ret[i])+5)*sizeof(unichar_t));
				u_strcpy(ret2[cnt],ret[i]);
			    }
			    ++cnt;
			    last_ch = ret[i][len];
			} else if ( doit && !type2 ) {
			    int j;
			    for ( j=len+1; ret[i][j]!='\0' && ret[i][j] == ret2[cnt-1][j]; ++j );
			    uc_strcpy(ret2[cnt-1]+j," ...");
			}
		    }
		    if ( cnt>=MAXLINES*MAXBRACKETS )
		break;
		    if ( cnt>=MAXLINES && !doit ) {
			type2 = (cnt+MAXBRACKETS-1)/MAXBRACKETS;
			ret2 = malloc((type2+1)*sizeof(unichar_t *));
		    } else if ( !doit )
			ret2 = malloc((cnt+1)*sizeof(unichar_t *));
		    else {
			if ( type2 )
			    cnt = type2;
			ret2[cnt] = NULL;
		    }
		}
		if ( ret2!=NULL ) {
		    for ( i=0; ret[i]!=NULL; ++i )
			free(ret[i]);
		    free(ret);
		    ret = gc->choices = ret2;
		    i = gc->ctot = cnt;
		}
	    }
	    if ( gc->ctot>=MAXLINES ) {
		/* Too many choices. Don't popup a list of them */
		gc->choices = NULL;
		for ( i=0; ret[i]!=NULL; ++i )
		    free(ret[i]);
		free(ret);
	    } else {
		gc->ctot = i;
		GCompletionCreatePopup(gc);
	    }
	} else {
	    free(ret[1]);
	    free(ret);
	}
    }
}