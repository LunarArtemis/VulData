static int flask_security_make_bools(void)
{
    int ret = 0;
    int num;
    int *values = NULL;
    
    xfree(bool_pending_values);
    
    ret = security_get_bools(&num, NULL, &values);
    if ( ret != 0 )
        goto out;

    bool_num = num;
    bool_pending_values = values;

 out:
    return ret;
}