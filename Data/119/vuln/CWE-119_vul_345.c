static int flask_security_resolve_bool(struct xen_flask_boolean *arg)
{
    char *name;
    int rv;

    if ( arg->bool_id != -1 )
        return 0;

    rv = flask_copyin_string(arg->name, &name, arg->size);
    if ( rv )
        return rv;

    arg->bool_id = security_find_bool(name);
    arg->size = 0;

    xfree(name);

    return 0;
}