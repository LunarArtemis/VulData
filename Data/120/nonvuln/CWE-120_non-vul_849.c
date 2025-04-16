void
devn_free_params(gx_device *thread_cdev)
{
    gs_devn_params *devn_params;
    int k;

    devn_params = dev_proc(thread_cdev, ret_devn_params)(thread_cdev);
    if (devn_params == NULL) return;

    for (k = 0; k < devn_params->separations.num_separations; k++) {
        gs_free_object(thread_cdev->memory,
                       devn_params->separations.names[k].data,
                       "devn_free_params");
        devn_params->separations.names[k].data = NULL;
    }

    for (k = 0; k < devn_params->pdf14_separations.num_separations; k++) {
        gs_free_object(thread_cdev->memory,
                       devn_params->pdf14_separations.names[k].data,
                       "devn_free_params");
        devn_params->pdf14_separations.names[k].data = NULL;
    }
}