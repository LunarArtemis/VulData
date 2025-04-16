static int security_preserve_bools(struct policydb *p)
{
    int rc, nbools = 0, *bvalues = NULL, i;
    char **bnames = NULL;
    struct cond_bool_datum *booldatum;
    struct cond_node *cur;

    rc = security_get_bools(&nbools, &bnames, &bvalues);
    if ( rc )
        goto out;
    for ( i = 0; i < nbools; i++ )
    {
        booldatum = hashtab_search(p->p_bools.table, bnames[i]);
        if ( booldatum )
            booldatum->state = bvalues[i];
    }
    for ( cur = p->cond_list; cur; cur = cur->next )
    {
        rc = evaluate_cond_node(p, cur);
        if ( rc )
            goto out;
    }

out:
    if ( bnames )
    {
        for ( i = 0; i < nbools; i++ )
            xfree(bnames[i]);
    }
    xfree(bnames);
    xfree(bvalues);
    return rc;
}