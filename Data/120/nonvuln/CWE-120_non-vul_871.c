RELOC_PTRS_END

void
gx_devn_prn_device_finalize(const gs_memory_t *cmem, void *vpdev)
{
    devn_free_params((gx_device*) vpdev);
    gx_device_finalize(cmem, vpdev);
}