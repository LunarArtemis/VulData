int security_get_bools(int *len, char ***names, int **values)
{
    int i, rc = -ENOMEM;

    POLICY_RDLOCK;
    if ( names )
        *names = NULL;
    *values = NULL;

    *len = policydb.p_bools.nprim;
    if ( !*len )
    {
        rc = 0;
        goto out;
    }

    if ( names ) {
        *names = (char**)xmalloc_array(char*, *len);
        if ( !*names )
            goto err;
        memset(*names, 0, sizeof(char*) * *len);
    }

    *values = (int*)xmalloc_array(int, *len);
    if ( !*values )
        goto err;

    for ( i = 0; i < *len; i++ )
    {
        size_t name_len;
        (*values)[i] = policydb.bool_val_to_struct[i]->state;
        if ( names ) {
            name_len = strlen(policydb.p_bool_val_to_name[i]) + 1;
            (*names)[i] = (char*)xmalloc_array(char, name_len);
            if ( !(*names)[i] )
                goto err;
            strlcpy((*names)[i], policydb.p_bool_val_to_name[i], name_len);
            (*names)[i][name_len - 1] = 0;
        }
    }
    rc = 0;
out:
    POLICY_RDUNLOCK;
    return rc;
err:
    if ( names && *names )
    {
        for ( i = 0; i < *len; i++ )
            xfree((*names)[i]);
    }
    xfree(*values);
    goto out;
}