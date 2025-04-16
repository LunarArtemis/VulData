void pdf_print::put_thick_slant(int bloc, int eloc) 
{ 
  pdf_command(TH_LINE, save_h[bloc], save_v[bloc], save_h[eloc], save_v[eloc]);
}