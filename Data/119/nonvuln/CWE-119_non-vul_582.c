void pdf_print::define_all_fonts()
{
    int i;
    font_list **ff = f_a;
    for (i=0; i< MAXFONTS; i++) {
	if ((*ff) && (*ff)->name) {
	    dbg1(Fonts, "define all fonts %s\n", (*ff)->name);
	    define_font((*ff)->num, (*ff)->name);
	}
	(*ff++);
    }
}