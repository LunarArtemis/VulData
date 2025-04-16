void pdf_bit_char(i_buf *pdf, int char_num)
{
    int i;
    PKBit *b;
    extern PKBit bits[];
    int byte_w;

    b = &bits[char_num];
    byte_w = (b->bm_w + 7 ) / 8; 


    pdf->PutString("/b_");
    pdf->Put10(char_num);
    pdf->PutString(" {");  // char_name);

    pdf->Put10(b->bm_w); pdf->Put10(b->bm_h);
    pdf->PutString("true [1 0 0 1 ");
    pdf->Put10(b->bm_h_off);     pdf->Put10(b->bm_h - b->bm_v_off );
    pdf->PutString("]\n{<");
/* bitmap */

    for (i=0; i < byte_w * b->bm_h; i++) {
	pdf->Put16(b->bm_bits[i] & 0xff);
	if (!((i+1) % 40)) pdf->PutString("\n");
    }
    pdf->PutString(">}\n imagemask } bind def\n");

 /* end bitmap */
}