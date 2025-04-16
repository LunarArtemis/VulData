void pdf_print::p_put_rule(int w, int h) 
{ 
    if (highlight==On){
	pdf_command(P_S_GRAY, 0, 0, 0, 0);
    }
    pdf_command(RULE, w, h, 0, 0);
    if (highlight==On) { 
	clear_highlight();  
	pdf_command(P_U_GRAY, 0, 0, 0, 0);
    }
}