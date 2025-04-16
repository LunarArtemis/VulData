static int
okiibm_print_page(gx_device_printer *pdev, gp_file *prn_stream)
{
        char init_string[16], end_string[16];
        int init_length, end_length;

        init_length = sizeof(okiibm_init_string);
        memcpy(init_string, okiibm_init_string, init_length);

        end_length = sizeof(okiibm_end_string);
        memcpy(end_string, okiibm_end_string, end_length);

        if ( pdev->y_pixels_per_inch > 72 &&
             pdev->x_pixels_per_inch > 60 )
        {
                /* Unidirectional printing for the higher resolutions. */
                memcpy( init_string + init_length, okiibm_one_direct,
                        sizeof(okiibm_one_direct) );
                init_length += sizeof(okiibm_one_direct);

                memcpy( end_string + end_length, okiibm_two_direct,
                        sizeof(okiibm_two_direct) );
                end_length += sizeof(okiibm_two_direct);
        }

        return okiibm_print_page1( pdev, prn_stream,
                                   pdev->y_pixels_per_inch > 72 ? 1 : 0,
                                   init_string, init_length,
                                   end_string, end_length );
}