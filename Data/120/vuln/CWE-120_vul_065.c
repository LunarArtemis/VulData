static int
pj_common_print_page(gx_device_printer *pdev, gp_file *prn_stream, int y_origin,
  const char *end_page)
{
#define DATA_SIZE (LINE_SIZE * 8)
        byte *data =
                (byte *)gs_malloc(pdev->memory, DATA_SIZE, 1,
                                  "paintjet_print_page(data)");
        byte *plane_data =
                (byte *)gs_malloc(pdev->memory, LINE_SIZE * 3, 1,
                                  "paintjet_print_page(plane_data)");
        if ( data == 0 || plane_data == 0 )
        {	if ( data )
                        gs_free(pdev->memory, (char *)data, DATA_SIZE, 1,
                                "paintjet_print_page(data)");
                if ( plane_data )
                        gs_free(pdev->memory, (char *)plane_data, LINE_SIZE * 3, 1,
                                "paintjet_print_page(plane_data)");
                return_error(gs_error_VMerror);
        }

        /* set raster graphics resolution -- 90 or 180 dpi */
        gp_fprintf(prn_stream, "\033*t%dR", X_DPI);

        /* set the line width */
        gp_fprintf(prn_stream, "\033*r%dS", DATA_SIZE);

        /* set the number of color planes */
        gp_fprintf(prn_stream, "\033*r%dU", 3);		/* always 3 */

        /* move to top left of page */
        gp_fprintf(prn_stream, "\033&a0H\033&a%dV", y_origin);

        /* select data compression */
        gp_fputs("\033*b1M", prn_stream);

        /* start raster graphics */
        gp_fputs("\033*r1A", prn_stream);

        /* Send each scan line in turn */
           {	int lnum;
                int line_size = gdev_mem_bytes_per_scan_line((gx_device *)pdev);
                int num_blank_lines = 0;
                for ( lnum = 0; lnum < pdev->height; lnum++ )
                   {	byte *end_data = data + line_size;
                        gdev_prn_copy_scan_lines(pdev, lnum,
                                                 (byte *)data, line_size);
                        /* Remove trailing 0s. */
                        while ( end_data > data && end_data[-1] == 0 )
                                end_data--;
                        if ( end_data == data )
                           {	/* Blank line */
                                num_blank_lines++;
                           }
                        else
                           {	int i;
                                byte *odp;
                                byte *row;

                                /* Pad with 0s to fill out the last */
                                /* block of 8 bytes. */
                                memset(end_data, 0, 7);

                                /* Transpose the data to get pixel planes. */
                                for ( i = 0, odp = plane_data; i < DATA_SIZE;
                                      i += 8, odp++
                                    )
                                 { /* The following is for 16-bit machines */
#define spread3(c)\
 { 0, c, c*0x100, c*0x101, c*0x10000L, c*0x10001L, c*0x10100L, c*0x10101L }
                                   static ulong spr40[8] = spread3(0x40);
                                   static ulong spr8[8] = spread3(8);
                                   static ulong spr2[8] = spread3(2);
                                   register byte *dp = data + i;
                                   register ulong pword =
                                     (spr40[dp[0]] << 1) +
                                     (spr40[dp[1]]) +
                                     (spr40[dp[2]] >> 1) +
                                     (spr8[dp[3]] << 1) +
                                     (spr8[dp[4]]) +
                                     (spr8[dp[5]] >> 1) +
                                     (spr2[dp[6]]) +
                                     (spr2[dp[7]] >> 1);
                                   odp[0] = (byte)(pword >> 16);
                                   odp[LINE_SIZE] = (byte)(pword >> 8);
                                   odp[LINE_SIZE*2] = (byte)(pword);
                                 }
                                /* Skip blank lines if any */
                                if ( num_blank_lines > 0 )
                                   {	/* move down from current position */
                                        gp_fprintf(prn_stream, "\033&a+%dV",
                                                   num_blank_lines * (720 / Y_DPI));
                                        num_blank_lines = 0;
                                   }

                                /* Transfer raster graphics */
                                /* in the order R, G, B. */
                                for ( row = plane_data + LINE_SIZE * 2, i = 0;
                                      i < 3; row -= LINE_SIZE, i++
                                    )
                                   {	byte temp[LINE_SIZE * 2];
                                        int count = compress1_row(row, row + LINE_SIZE, temp);
                                        gp_fprintf(prn_stream, "\033*b%d%c",
                                                   count, "VVW"[i]);
                                        gp_fwrite(temp, sizeof(byte),
                                                  count, prn_stream);
                                   }
                           }
                   }
           }

        /* end the page */
        gp_fputs(end_page, prn_stream);

        gs_free(pdev->memory, (char *)data, DATA_SIZE, 1, "paintjet_print_page(data)");
        gs_free(pdev->memory, (char *)plane_data, LINE_SIZE * 3, 1, "paintjet_print_page(plane_data)");

        return 0;
}