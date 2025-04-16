void pdf_print::glp(int reg,int h[], int v[])
{
    if (reg >= REGS) 
      dbg1 (Error, "tab: getloc: illegal register %d\n", (void *)reg);

    h_diff = h[reg] - dvi_h;
    v_diff = v[reg] - dvi_v;
    dvi_h = h[reg];
    dvi_v = v[reg];

    dbg5(Stack, "print: glp: reg %d v_diff %d h_diff %d dvi_v %d dvi_h %d\n", 
	 (void *)reg, 
	 (void *)v_diff, (void *)h_diff, 
	 (void *)dvi_v, (void *)dvi_h );

    pdf_command(MOVE, dvi_h, -dvi_v, 0, 0);
    
}