ENUM_PTRS_END
static RELOC_PTRS_WITH(gx_devn_prn_device_reloc_ptrs, gx_devn_prn_device *pdev)
{
    RELOC_PREFIX(st_device_printer);
    {
        int i;

        for (i = 0; i < pdev->devn_params.separations.num_separations; ++i) {
            RELOC_PTR(gx_devn_prn_device, devn_params.separations.names[i].data);
        }
    }
}