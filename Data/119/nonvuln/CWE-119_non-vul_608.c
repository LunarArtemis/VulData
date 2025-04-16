void pdf_print::pop() 
{
//    pdf_command(PPOP, 0, 0, 0, 0);
//    sp--; dvi_h = stack_h[sp]; dvi_v = stack_v[sp];
    sp--;
    glp(sp, stack_h, stack_v);
//    printf("pdf pop  %d, h %d, stack h %d\n", sp, dvi_h, stack_h[sp]);
}