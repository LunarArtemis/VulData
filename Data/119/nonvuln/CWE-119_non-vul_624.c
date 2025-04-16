void pdf_print::put_uline(int bloc, int eloc)
{ 
    saveloc(REGS-1);
    pdf_command ( PTIE2, save_h[eloc] - save_h[bloc], 0, 0, 0);
    getloc(REGS-1);
}