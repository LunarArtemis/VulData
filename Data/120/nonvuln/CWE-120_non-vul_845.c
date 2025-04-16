static int
compare_equivalent_cmyk_color_params(const equivalent_cmyk_color_params *pequiv_colors1, const equivalent_cmyk_color_params *pequiv_colors2)
{
  int i;
  if (pequiv_colors1->all_color_info_valid != pequiv_colors2->all_color_info_valid)
    return(1);
  for (i=0;  i<GX_DEVICE_MAX_SEPARATIONS;  i++) {
    if (pequiv_colors1->color[i].color_info_valid != pequiv_colors2->color[i].color_info_valid)
      return(1);
    if (pequiv_colors1->color[i].c                != pequiv_colors2->color[i].c               )
      return(1);
    if (pequiv_colors1->color[i].m                != pequiv_colors2->color[i].m               )
      return(1);
    if (pequiv_colors1->color[i].y                != pequiv_colors2->color[i].y               )
      return(1);
    if (pequiv_colors1->color[i].k                != pequiv_colors2->color[i].k               )
      return(1);
  }
  return(0);
}