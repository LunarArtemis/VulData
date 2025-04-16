static void *genunicodedata(void *_gt,int32 *len) {
    GTextField *gt = _gt;
    unichar_t *temp;
    *len = gt->sel_end-gt->sel_start + 1;
    temp = malloc((*len+2)*sizeof(unichar_t));
    temp[0] = 0xfeff;		/* KDE expects a byte order flag */
    u_strncpy(temp+1,gt->text+gt->sel_start,gt->sel_end-gt->sel_start);
    temp[*len+1] = 0;
return( temp );
}