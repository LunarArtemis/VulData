static int getorigin(void *UNUSED(d), BasePoint *base, int index) {
    /*FontView *fv = (FontView *) d;*/

    base->x = base->y = 0;
    switch ( index ) {
      case 0:		/* Character origin */
	/* all done */
      break;
      case 1:		/* Center of selection */
	/*CVFindCenter(cv,base,!CVAnySel(cv,NULL,NULL,NULL,NULL));*/
      break;
      default:
return( false );
    }
return( true );
}