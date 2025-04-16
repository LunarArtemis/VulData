static int flask_security_set_bool(struct xen_flask_boolean *arg)
{
    int rv;

    rv = flask_security_resolve_bool(arg);
    if ( rv )
        return rv;

    rv = domain_has_security(current->domain, SECURITY__SETBOOL);
    if ( rv )
        return rv;

    spin_lock(&sel_sem);

    if ( arg->commit )
    {
        int num;
        int *values;

        rv = security_get_bools(&num, NULL, &values);
        if ( rv != 0 )
            goto out;

        if ( arg->bool_id >= num )
        {
            rv = -ENOENT;
            goto out;
        }
        values[arg->bool_id] = !!(arg->new_value);

        arg->enforcing = arg->pending = !!(arg->new_value);

        if ( bool_pending_values )
            bool_pending_values[arg->bool_id] = !!(arg->new_value);

        rv = security_set_bools(num, values);
        xfree(values);
    }
    else
    {
        if ( !bool_pending_values )
            flask_security_make_bools();

        if ( arg->bool_id >= bool_num )
            goto out;

        bool_pending_values[arg->bool_id] = !!(arg->new_value);
        arg->pending = !!(arg->new_value);
        arg->enforcing = security_get_bool_value(arg->bool_id);

        rv = 0;
    }

 out:
    spin_unlock(&sel_sem);
    return rv;
}