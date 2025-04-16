static bool
check_process_color_names(fixed_colorant_names_list plist,
                          const gs_param_string * pstring)
{
    if (plist) {
        uint size = pstring->size;

        while( *plist) {
            if (compare_color_names(*plist, strlen(*plist), pstring->data, size)) {
                return true;
            }
            plist++;
        }
    }
    return false;
}