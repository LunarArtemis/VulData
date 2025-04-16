int  pdf_print::more()
{ 
    double length;
    if ( red == 1.0 ) {
	length = 2.6;
    }
    else if (red == 0.94440){ /* 95 */
	length = 2.5;
    }
    else if (red == 0.88880){ /* 9 */
	length = 2.4;
    }
    else if (red == 0.777770){ /* 8 */
	length = 2.80;
    }
    else {
	length = 2.50;           /* 7 */
    }
    //        printf("pdf more %f\n", dvi_to_inch(dvi_v));
    if (inch_to_dvi(length) > dvi_v) {
	return(1);		/* END_MORE */
    }
    return(0);			/* END_OK */
}