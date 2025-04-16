static bool separations_equal(const gs_separations *p1, const gs_separations *p2)
{
    int k;

    if (p1->num_separations != p2->num_separations)
        return false;
    for (k = 0; k < p1->num_separations; k++) {
        if (p1->names[k].size != p2->names[k].size)
            return false;
        else if (p1->names[k].size > 0) {
            if (memcmp(p1->names[k].data, p2->names[k].data, p1->names[k].size) != 0)
                return false;
        }
    }
    return true;
}