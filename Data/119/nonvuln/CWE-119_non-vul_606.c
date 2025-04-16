void pdf_print::put_slash
      (int bloc, int eloc, int count, struct file_info *f) 
{ 
    double nflags = (double)count;
    int thickness = str_to_dvi("0.005 in");

    if (f->flags & LSA_FORM) thickness = str_to_dvi("0.023 in");

    saveloc(REGS-1);
/*     move_n_v("0.19 in"); */
    while (count) {
	p_put_rule(save_h[eloc] - save_h[bloc], thickness);
	movev(str_to_inch("0.03 in") + 0.06 / nflags);
	count --;
    }
    getloc(REGS-1);
}