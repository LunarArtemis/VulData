void pdf_print::p_moveh(const int hor) 
{
   if (hor == 0) return;
   pdf_command(MOVEH, hor, 0, 0, 0);
   dvi_h += hor;	
}