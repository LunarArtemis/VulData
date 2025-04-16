static void *ddgenunicodedata(void *_gt,int32 *len) {
    void *temp = genunicodedata(_gt,len);
    GTextField *gt = _gt;
    _GTextFieldReplace(gt,nullstr);
    _ggadget_redraw(&gt->g);
return( temp );
}