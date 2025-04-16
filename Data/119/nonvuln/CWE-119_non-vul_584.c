void pdf_print::print_clipped(char c, int font/* acutally height */)
{ 
    pdf_command ( PDF_CLIP, c, font, dvi_h, dvi_v);
     dvi_h += inch_to_dvi(f_a[curfont]->fnt->get_width(c));
}