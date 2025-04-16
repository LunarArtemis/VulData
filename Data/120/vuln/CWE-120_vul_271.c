static pj_status_t parse_rr(pj_dns_parsed_rr *rr, pj_pool_t *pool,
                            const pj_uint8_t *pkt,
                            const pj_uint8_t *start, const pj_uint8_t *max,
                            int *parsed_len)
{
    const pj_uint8_t *p = start;
    int name_len, name_part_len;
    pj_status_t status;

    /* Get the length of the name */
    status = get_name_len(0, pkt, start, max, &name_part_len, &name_len);
    if (status != PJ_SUCCESS)
        return status;

    /* Allocate memory for the name */
    rr->name.ptr = (char*) pj_pool_alloc(pool, name_len+4);
    rr->name.slen = 0;

    /* Get the name */
    status = get_name(0, pkt, start, max, &rr->name);
    if (status != PJ_SUCCESS)
        return status;

    p = (start + name_part_len);

    /* Check the size can accomodate next few fields. */
    if (p+10 > max)
        return PJLIB_UTIL_EDNSINSIZE;

    /* Get the type */
    pj_memcpy(&rr->type, p, 2);
    rr->type = pj_ntohs(rr->type);
    p += 2;
    
    /* Get the class */
    pj_memcpy(&rr->dnsclass, p, 2);
    rr->dnsclass = pj_ntohs(rr->dnsclass);
    p += 2;

    /* Class MUST be IN */
    if (rr->dnsclass != 1) {
        /* Class is not IN, return error only if type is known (see #1889) */
        if (rr->type == PJ_DNS_TYPE_A     || rr->type == PJ_DNS_TYPE_AAAA  ||
            rr->type == PJ_DNS_TYPE_CNAME || rr->type == PJ_DNS_TYPE_NS    ||
            rr->type == PJ_DNS_TYPE_PTR   || rr->type == PJ_DNS_TYPE_SRV)
        {
            return PJLIB_UTIL_EDNSINCLASS;
        }
    }

    /* Get TTL */
    pj_memcpy(&rr->ttl, p, 4);
    rr->ttl = pj_ntohl(rr->ttl);
    p += 4;

    /* Get rdlength */
    pj_memcpy(&rr->rdlength, p, 2);
    rr->rdlength = pj_ntohs(rr->rdlength);
    p += 2;

    /* Check that length is valid */
    if (p + rr->rdlength > max)
        return PJLIB_UTIL_EDNSINSIZE;

    /* Parse some well known records */
    if (rr->type == PJ_DNS_TYPE_A) {
        pj_memcpy(&rr->rdata.a.ip_addr, p, 4);
        p += 4;

    } else if (rr->type == PJ_DNS_TYPE_AAAA) {
        pj_memcpy(&rr->rdata.aaaa.ip_addr, p, 16);
        p += 16;

    } else if (rr->type == PJ_DNS_TYPE_CNAME ||
               rr->type == PJ_DNS_TYPE_NS ||
               rr->type == PJ_DNS_TYPE_PTR) 
    {

        /* Get the length of the target name */
        status = get_name_len(0, pkt, p, max, &name_part_len, &name_len);
        if (status != PJ_SUCCESS)
            return status;

        /* Allocate memory for the name */
        rr->rdata.cname.name.ptr = (char*) pj_pool_alloc(pool, name_len);
        rr->rdata.cname.name.slen = 0;

        /* Get the name */
        status = get_name(0, pkt, p, max, &rr->rdata.cname.name);
        if (status != PJ_SUCCESS)
            return status;

        p += name_part_len;

    } else if (rr->type == PJ_DNS_TYPE_SRV) {

        /* Priority */
        pj_memcpy(&rr->rdata.srv.prio, p, 2);
        rr->rdata.srv.prio = pj_ntohs(rr->rdata.srv.prio);
        p += 2;

        /* Weight */
        pj_memcpy(&rr->rdata.srv.weight, p, 2);
        rr->rdata.srv.weight = pj_ntohs(rr->rdata.srv.weight);
        p += 2;

        /* Port */
        pj_memcpy(&rr->rdata.srv.port, p, 2);
        rr->rdata.srv.port = pj_ntohs(rr->rdata.srv.port);
        p += 2;
        
        /* Get the length of the target name */
        status = get_name_len(0, pkt, p, max, &name_part_len, &name_len);
        if (status != PJ_SUCCESS)
            return status;

        /* Allocate memory for the name */
        rr->rdata.srv.target.ptr = (char*) pj_pool_alloc(pool, name_len);
        rr->rdata.srv.target.slen = 0;

        /* Get the name */
        status = get_name(0, pkt, p, max, &rr->rdata.srv.target);
        if (status != PJ_SUCCESS)
            return status;
        p += name_part_len;

    } else {
        /* Copy the raw data */
        rr->data = pj_pool_alloc(pool, rr->rdlength);
        pj_memcpy(rr->data, p, rr->rdlength);

        p += rr->rdlength;
    }

    *parsed_len = (int)(p - start);
    return PJ_SUCCESS;
}