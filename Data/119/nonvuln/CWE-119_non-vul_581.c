void pdf_print::set_a_char (unsigned char c) 
{ 
    if (highlight==On) 
	pdf_command(P_S_GRAY, 0, 0, 0, 0);
    if (c == 0365) 
      dvi_h += inch_to_dvi(f_a[curfont]->fnt->get_width('i'));
    else if ( c == 0074)
      dvi_h += inch_to_dvi(f_a[curfont]->fnt->get_width('!'));
    else if ( c == 0076)
      dvi_h += inch_to_dvi(f_a[curfont]->fnt->get_width('?'));
    else if ( c == 014 )  /* fi */
      dvi_h += inch_to_dvi(f_a[curfont]->fnt->get_width(0256));
    else if ( c == 015 )  /* fl */
      dvi_h += inch_to_dvi(f_a[curfont]->fnt->get_width(0257));
    else if ( c == 031 )  /* german ss */
      dvi_h += inch_to_dvi(f_a[curfont]->fnt->get_width(031));
    else
      dvi_h += inch_to_dvi(f_a[curfont]->fnt->get_width(c));

    if (curfont != 0) {
//	printf("set_a_char: c is %d %c\n", c, c);
	if ( c == 0013) {
	    c = 0146; // ff - just one f in postscript
	    pdf_command(CHAR, (int)c, 0, 0,0); // print the first one
	}
	else if ( c == 0014) c = 0256; // fi
	else if ( c == 0015) c = 0257; // fl
	else if ( c == 0031) c = 0373; // fs
	else if ( c == 0032) c = 0361; // ae
	else if ( c == 0035) c = 0341; // AE
	else if ( c == 0033) c = 0352; // oe
	else if ( c == 0036) c = 0372; // OE
	else if ( c == 0034) c = 0371; // oo
	else if ( c == 0037) c = 0351; // OO
	else if ( c == 0074) c = 0241; // ! inverted
	else if ( c == 0076) c = 0277; // ? inverted
    }

    if (curfont == 0 && pdf_used[c] < 4) 
      pdf_used[c]++;
    pdf_command(CHAR, (int)c, 0, 0,0);
    if (highlight==On) { 
	clear_highlight();  
	pdf_command(P_U_GRAY, 0, 0, 0, 0);
    }
}