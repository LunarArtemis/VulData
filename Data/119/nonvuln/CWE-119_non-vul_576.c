void pdf_print::push() 
{
//    pdf_command(PPUSH, 0, 0, 0, 0);
//    printf("pdf push %d, dvi_h %d, stack h %d\n",
//	   sp, dvi_h, stack_h[sp]);
    slp(sp, stack_h, stack_v); sp++;		  // also set the dvi_v, dvi_h
}