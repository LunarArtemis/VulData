void pdf_print::put_slant(int bloc, int eloc) 
{
    saveloc(REGS-1);

    pdf_command(LINE, save_h[bloc], save_v[bloc], save_h[eloc], save_v[eloc]);
    
    getloc(REGS-1);
    return;
}