void pdf_print::p_num(int n) 
{ 
#define N_S 5
    char string[N_S];
    int i;
    double total_width=0.0;
#ifdef MAC
#else
    sprintf(string, "%d", n);
#endif
    push();
    movev("0.18 in");

    use_font(1);
    for (i=0; i < N_S && string[i]; i++)
      total_width += f_a[curfont]->fnt->get_width(string[i]);

    //    printf("p_num, width is %f  %s\n", total_width, string);

    moveh(-total_width/2.0 + 0.018);

    for (i=0; i < N_S && string[i]; i++)
      set_a_char(string[i]);
	
    use_font(0);
    pop();
}