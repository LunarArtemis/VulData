static void gnttab_copy_release_buf(struct gnttab_copy_buf *buf)
{
    if ( buf->virt )
    {
        unmap_domain_page(buf->virt);
        buf->virt = NULL;
    }
    if ( buf->have_type )
    {
        put_page_type(buf->page);
        buf->have_type = 0;
    }
    if ( buf->page )
    {
        put_page(buf->page);
        buf->page = NULL;
    }
    if ( buf->have_grant )
    {
        release_grant_for_copy(buf->domain, buf->ptr.u.ref, buf->read_only);
        buf->have_grant = 0;
    }
}