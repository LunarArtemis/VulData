void pdf_print::put_a_char (unsigned char c) 
{
    if (highlight==On)  {
	if (highlight_type == Paren ) {
	    moveh (-.08);
	    pdf_command(PCHAR, (int)'(', 0, 0, 0);
	    moveh (.08);
	}
	else if (highlight_type == Red) {
	  pdf_command(P_S_RED, 0, 0, 0, 0);
	}
	else
	  pdf_command(P_S_GRAY, 0, 0, 0, 0);
    }
    if (curfont == 0 && pdf_used[c] < 4) 
	pdf_used[c]++;
    pdf_command(PCHAR, (int)c, 0, 0,0);
    if (highlight==On) { 
	clear_highlight();  
	if (highlight_type == Paren) {
	  double www=f_a[curfont]->fnt->get_width(c);
	    moveh (www);
	    pdf_command(PCHAR, (int)')', 0, 0, 0);
	    pdf_used['(']++;	pdf_used[')']++;
	    moveh (-www);
	}
	else if (highlight_type == Red)
	  pdf_command(P_U_RED, 0, 0, 0, 0);
	else
	  pdf_command(P_U_GRAY, 0, 0, 0, 0);
    }
}