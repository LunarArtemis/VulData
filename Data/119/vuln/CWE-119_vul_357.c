static int xc_dom_parse_zimage32_kernel(struct xc_dom_image *dom)
{
    uint32_t *zimage;
    uint32_t start, entry_addr;
    uint64_t v_start, v_end;
    uint64_t rambase = dom->rambase_pfn << XC_PAGE_SHIFT;

    DOMPRINTF_CALLED(dom->xch);

    zimage = (uint32_t *)dom->kernel_blob;

    /* Do not load kernel at the very first RAM address */
    v_start = rambase + 0x8000;
    v_end = v_start + dom->kernel_size;

    start = zimage[ZIMAGE32_START_OFFSET/4];

    if (start == 0)
        entry_addr = v_start;
    else
        entry_addr = start;

    /* find kernel segment */
    dom->kernel_seg.vstart = v_start;
    dom->kernel_seg.vend   = v_end;

    dom->parms.virt_entry = entry_addr;
    dom->parms.virt_base = rambase;

    dom->guest_type = "xen-3.0-armv7l";
    DOMPRINTF("%s: %s: 0x%" PRIx64 " -> 0x%" PRIx64 "",
              __FUNCTION__, dom->guest_type,
              dom->kernel_seg.vstart, dom->kernel_seg.vend);
    return 0;
}