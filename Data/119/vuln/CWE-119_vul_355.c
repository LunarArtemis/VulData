static int xc_dom_parse_zimage64_kernel(struct xc_dom_image *dom)
{
    struct zimage64_hdr *zimage;
    uint64_t v_start, v_end;
    uint64_t rambase = dom->rambase_pfn << XC_PAGE_SHIFT;

    DOMPRINTF_CALLED(dom->xch);

    zimage = dom->kernel_blob;

    v_start = rambase + zimage->text_offset;
    v_end = v_start + dom->kernel_size;

    dom->kernel_seg.vstart = v_start;
    dom->kernel_seg.vend   = v_end;

    /* Call the kernel at offset 0 */
    dom->parms.virt_entry = v_start;
    dom->parms.virt_base = rambase;

    dom->guest_type = "xen-3.0-aarch64";
    DOMPRINTF("%s: %s: 0x%" PRIx64 " -> 0x%" PRIx64 "",
              __FUNCTION__, dom->guest_type,
              dom->kernel_seg.vstart, dom->kernel_seg.vend);

    return 0;
}