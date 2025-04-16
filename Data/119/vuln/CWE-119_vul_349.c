static int flask_security_user(struct xen_flask_userlist *arg)
{
    char *user;
    u32 *sids;
    u32 nsids;
    int rv;

    rv = domain_has_security(current->domain, SECURITY__COMPUTE_USER);
    if ( rv )
        return rv;

    rv = flask_copyin_string(arg->u.user, &user, arg->size);
    if ( rv )
        return rv;

    rv = security_get_user_sids(arg->start_sid, user, &sids, &nsids);
    if ( rv < 0 )
        goto out;

    if ( nsids * sizeof(sids[0]) > arg->size )
        nsids = arg->size / sizeof(sids[0]);

    arg->size = nsids;

    if ( copy_to_guest(arg->u.sids, sids, nsids) )
        rv = -EFAULT;

    xfree(sids);
 out:
    xfree(user);
    return rv;
}