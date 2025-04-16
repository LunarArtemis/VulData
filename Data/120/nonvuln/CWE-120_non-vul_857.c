static bool devn_params_equal(const gs_devn_params *p1, const gs_devn_params *p2)
{
    if (p1->bitspercomponent != p2->bitspercomponent)
        return false;
    if (p1->max_separations != p2->max_separations)
        return false;
    if (p1->num_separation_order_names != p2->num_separation_order_names)
        return false;
    if (p1->num_std_colorant_names != p2->num_std_colorant_names)
        return false;
    if (p1->page_spot_colors != p2->page_spot_colors)
        return false;
    if (!separations_equal(&p1->pdf14_separations, &p2->pdf14_separations))
        return false;
    if (!separations_equal(&p1->separations, &p2->separations))
        return false;
    if (memcmp(p1->separation_order_map, p2->separation_order_map, sizeof(gs_separation_map)) != 0)
        return false;
    if (p1->std_colorant_names != p2->std_colorant_names)
        return false;
    return true;
}