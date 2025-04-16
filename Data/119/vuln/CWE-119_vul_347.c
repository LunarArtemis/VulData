static int flask_security_context(struct xen_flask_sid_context *arg)
{
    int rv;
    char *buf;

    rv = domain_has_security(current->domain, SECURITY__CHECK_CONTEXT);
    if ( rv )
        return rv;

    rv = flask_copyin_string(arg->context, &buf, arg->size);
    if ( rv )
        return rv;

    rv = security_context_to_sid(buf, arg->size, &arg->sid);
    if ( rv < 0 )
        goto out;

 out:
    xfree(buf);

    return rv;
}