static void _GTextFieldReplace(GTextField *gt, const unichar_t *str) {
    unichar_t *old = gt->oldtext;
    unichar_t *new = malloc((u_strlen(gt->text)-(gt->sel_end-gt->sel_start) + u_strlen(str)+1)*sizeof(unichar_t));

    gt->oldtext = gt->text;
    gt->sel_oldstart = gt->sel_start;
    gt->sel_oldend = gt->sel_end;
    gt->sel_oldbase = gt->sel_base;

    u_strncpy(new,gt->text,gt->sel_start);
    u_strcpy(new+gt->sel_start,str);
    gt->sel_start = u_strlen(new);
    u_strcpy(new+gt->sel_start,gt->text+gt->sel_end);
    gt->text = new;
    gt->sel_end = gt->sel_base = gt->sel_start;
    free(old);

    GTextFieldRefigureLines(gt,gt->sel_oldstart);
}