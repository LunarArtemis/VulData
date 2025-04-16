void pdf_print::use_font(int fontnum) 
{ 
    curfont = fontnum;
    switch (fontnum) {
    case 7:			/* page nnumber font doesn't shrink 12 pt */
	pdf_command( NUMFONT, 0, 0, 0, 0);
	break;
    case 6:			/* font 6 18 pt text */
	pdf_command( MUSICS, 0, 0, 0, 0);
	break;
    case 5:			/* font 5 10 pt italic */
	pdf_command( ITAL, 0, 0, 0, 0); // title italic
	break;
    case 4:			/* font 4 med */
	pdf_command( MED, 0, 0, 0, 0);
	break;
    case 3:			/* font 3 big */
	pdf_command( BIGIT, 0, 0, 0, 0);
	break;
    case 2:			/* font 2 */
	pdf_command( ROMAN, 0, 0, 0, 0);	// title
	break;
    case 1:			/* font 1 */
	pdf_command( SMALL, 0, 0, 0, 0); // words to songs
	break;
    case 0:		        /* lute */
	pdf_command( LUTE, 0, 0, 0, 0);
	break;
    default:
	dbg0(Warning, "tab: pdf_setfont: undefined font number, using 2\n");
	pdf_command( ROMAN, 0, 0, 0, 0);
	break;
    }
}