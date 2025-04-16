static void gt_cursor_pos(GTextField *gt, int *x, int *y) {
    _gt_cursor_pos(gt,gt->sel_start,x,y);
}