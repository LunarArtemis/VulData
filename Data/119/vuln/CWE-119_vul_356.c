static int xc_dom_probe_zimage32_kernel(struct xc_dom_image *dom)
{
    uint32_t *zimage;
    uint32_t end;

    if ( dom->kernel_blob == NULL )
    {
        xc_dom_panic(dom->xch, XC_INTERNAL_ERROR,
                     "%s: no kernel image loaded", __FUNCTION__);
        return -EINVAL;
    }

    if ( dom->kernel_size < 0x30 /*sizeof(struct setup_header)*/ )
    {
        xc_dom_printf(dom->xch, "%s: kernel image too small", __FUNCTION__);
        return -EINVAL;
    }

    zimage = (uint32_t *)dom->kernel_blob;
    if ( zimage[ZIMAGE32_MAGIC_OFFSET/4] != ZIMAGE32_MAGIC )
    {
        xc_dom_printf(dom->xch, "%s: kernel is not an arm32 zImage", __FUNCTION__);
        return -EINVAL;
    }

    end = zimage[ZIMAGE32_END_OFFSET/4];

    /*
     * Check for an appended DTB.
     */
    if ( end + sizeof(struct minimal_dtb_header) < dom->kernel_size ) {
        struct minimal_dtb_header *dtb_hdr;
        dtb_hdr = (struct minimal_dtb_header *)(dom->kernel_blob + end);
        if (ntohl/*be32_to_cpu*/(dtb_hdr->magic) == DTB_MAGIC) {
            xc_dom_printf(dom->xch, "%s: found an appended DTB", __FUNCTION__);
            end += ntohl/*be32_to_cpu*/(dtb_hdr->total_size);
        }
    }

    dom->kernel_size = end;

    return 0;
}