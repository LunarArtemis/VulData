void CWeightTable::Calc(int dest_len, int dest_min, int dest_max, int src_len, int src_min, int src_max, int flags)
{
    if (m_pWeightTables) {
        FX_Free(m_pWeightTables);
        m_pWeightTables = NULL;
    }
    double scale, base;
    scale = FXSYS_Div((FX_FLOAT)(src_len), (FX_FLOAT)(dest_len));
    if (dest_len < 0) {
        base = (FX_FLOAT)(src_len);
    } else {
        base = 0;
    }
    int ext_size = flags & FXDIB_BICUBIC_INTERPOL ? 3 : 1;
    m_ItemSize = sizeof(int) * 2 + (int)(sizeof(int) * (FXSYS_ceil(FXSYS_fabs((FX_FLOAT)scale)) + ext_size));
    m_DestMin = dest_min;
    if ((dest_max - dest_min) > (int)((1U << 30) - 4) / m_ItemSize) {
        return;
    }
    m_pWeightTables = FX_AllocNL(FX_BYTE, (dest_max - dest_min) * m_ItemSize + 4);
    if (m_pWeightTables == NULL) {
        return;
    }
    if ((flags & FXDIB_NOSMOOTH) != 0 || FXSYS_fabs((FX_FLOAT)scale) < 1.0f) {
        for (int dest_pixel = dest_min; dest_pixel < dest_max; dest_pixel ++) {
            PixelWeight& pixel_weights = *GetPixelWeight(dest_pixel);
            double src_pos = dest_pixel * scale + scale / 2 + base;
            if (flags & FXDIB_INTERPOL) {
                pixel_weights.m_SrcStart = (int)FXSYS_floor((FX_FLOAT)src_pos - 1.0f / 2);
                pixel_weights.m_SrcEnd = (int)FXSYS_floor((FX_FLOAT)src_pos + 1.0f / 2);
                if (pixel_weights.m_SrcStart < src_min) {
                    pixel_weights.m_SrcStart = src_min;
                }
                if (pixel_weights.m_SrcEnd >= src_max) {
                    pixel_weights.m_SrcEnd = src_max - 1;
                }
                if (pixel_weights.m_SrcStart == pixel_weights.m_SrcEnd) {
                    pixel_weights.m_Weights[0] = 65536;
                } else {
                    pixel_weights.m_Weights[1] = FXSYS_round((FX_FLOAT)(src_pos - pixel_weights.m_SrcStart - 1.0f / 2) * 65536);
                    pixel_weights.m_Weights[0] = 65536 - pixel_weights.m_Weights[1];
                }
            } else if (flags & FXDIB_BICUBIC_INTERPOL) {
                pixel_weights.m_SrcStart = (int)FXSYS_floor((FX_FLOAT)src_pos - 1.0f / 2);
                pixel_weights.m_SrcEnd = (int)FXSYS_floor((FX_FLOAT)src_pos + 1.0f / 2);
                int start = pixel_weights.m_SrcStart - 1;
                int end = pixel_weights.m_SrcEnd + 1;
                if (start < src_min) {
                    start = src_min;
                }
                if (end >= src_max) {
                    end = src_max - 1;
                }
                if (pixel_weights.m_SrcStart < src_min) {
                    src_pos += src_min - pixel_weights.m_SrcStart;
                    pixel_weights.m_SrcStart = src_min;
                }
                if (pixel_weights.m_SrcEnd >= src_max) {
                    pixel_weights.m_SrcEnd = src_max - 1;
                }
                int weight;
                weight = FXSYS_round((FX_FLOAT)(src_pos - pixel_weights.m_SrcStart - 1.0f / 2) * 256);
                if (start == end) {
                    pixel_weights.m_Weights[0] = (SDP_Table[256 + weight] + SDP_Table[weight] + SDP_Table[256 - weight] + SDP_Table[512 - weight]) << 8;
                } else if ((start == pixel_weights.m_SrcStart && (pixel_weights.m_SrcStart == pixel_weights.m_SrcEnd ||
                            end == pixel_weights.m_SrcEnd) && start < end) || (start < pixel_weights.m_SrcStart && pixel_weights.m_SrcStart == pixel_weights.m_SrcEnd && end == pixel_weights.m_SrcEnd)) {
                    if (start < pixel_weights.m_SrcStart) {
                        pixel_weights.m_Weights[0] = SDP_Table[256 + weight] << 8;
                        pixel_weights.m_Weights[1] = (SDP_Table[weight] + SDP_Table[256 - weight] + SDP_Table[512 - weight]) << 8;
                    } else {
                        if (pixel_weights.m_SrcStart == pixel_weights.m_SrcEnd) {
                            pixel_weights.m_Weights[0] = (SDP_Table[256 + weight] + SDP_Table[weight] + SDP_Table[256 - weight]) << 8;
                            pixel_weights.m_Weights[1] = SDP_Table[512 - weight] << 8;
                        } else {
                            pixel_weights.m_Weights[0] = (SDP_Table[256 + weight] + SDP_Table[weight]) << 8;
                            pixel_weights.m_Weights[1] = (SDP_Table[256 - weight] + SDP_Table[512 - weight]) << 8;
                        }
                    }
                    if (pixel_weights.m_SrcStart == pixel_weights.m_SrcEnd) {
                        pixel_weights.m_SrcEnd = end;
                    }
                    if (start < pixel_weights.m_SrcStart) {
                        pixel_weights.m_SrcStart = start;
                    }
                } else if (start == pixel_weights.m_SrcStart &&
                           start < pixel_weights.m_SrcEnd &&
                           pixel_weights.m_SrcEnd < end) {
                    pixel_weights.m_Weights[0] = (SDP_Table[256 + weight] + SDP_Table[weight]) << 8;
                    pixel_weights.m_Weights[1] = SDP_Table[256 - weight] << 8;
                    pixel_weights.m_Weights[2] = SDP_Table[512 - weight] << 8;
                    pixel_weights.m_SrcEnd = end;
                } else if (start < pixel_weights.m_SrcStart &&
                           pixel_weights.m_SrcStart < pixel_weights.m_SrcEnd &&
                           pixel_weights.m_SrcEnd == end) {
                    pixel_weights.m_Weights[0] = SDP_Table[256 + weight] << 8;
                    pixel_weights.m_Weights[1] = SDP_Table[weight] << 8;
                    pixel_weights.m_Weights[2] = (SDP_Table[256 - weight] + SDP_Table[512 - weight]) << 8;
                    pixel_weights.m_SrcStart = start;
                } else {
                    pixel_weights.m_Weights[0] = SDP_Table[256 + weight] << 8;
                    pixel_weights.m_Weights[1] = SDP_Table[weight] << 8;
                    pixel_weights.m_Weights[2] = SDP_Table[256 - weight] << 8;
                    pixel_weights.m_Weights[3] = SDP_Table[512 - weight] << 8;
                    pixel_weights.m_SrcStart = start;
                    pixel_weights.m_SrcEnd = end;
                }
            } else {
                pixel_weights.m_SrcStart = pixel_weights.m_SrcEnd = (int)FXSYS_floor((FX_FLOAT)src_pos);
                if (pixel_weights.m_SrcStart < src_min) {
                    pixel_weights.m_SrcStart = src_min;
                }
                if (pixel_weights.m_SrcEnd >= src_max) {
                    pixel_weights.m_SrcEnd = src_max - 1;
                }
                pixel_weights.m_Weights[0] = 65536;
            }
        }
        return;
    }
    for (int dest_pixel = dest_min; dest_pixel < dest_max; dest_pixel ++) {
        PixelWeight& pixel_weights = *GetPixelWeight(dest_pixel);
        double src_start = dest_pixel * scale + base;
        double src_end = src_start + scale;
        int start_i, end_i;
        if (src_start < src_end) {
            start_i = (int)FXSYS_floor((FX_FLOAT)src_start);
            end_i = (int)FXSYS_ceil((FX_FLOAT)src_end);
        } else {
            start_i = (int)FXSYS_floor((FX_FLOAT)src_end);
            end_i = (int)FXSYS_ceil((FX_FLOAT)src_start);
        }
        if (start_i < src_min) {
            start_i = src_min;
        }
        if (end_i >= src_max) {
            end_i = src_max - 1;
        }
        if (start_i > end_i) {
            if (start_i >= src_max) {
                start_i = src_max - 1;
            }
            pixel_weights.m_SrcStart = start_i;
            pixel_weights.m_SrcEnd = start_i;
            continue;
        }
        pixel_weights.m_SrcStart = start_i;
        pixel_weights.m_SrcEnd = end_i;
        for (int j = start_i; j <= end_i; j ++) {
            double dest_start = FXSYS_Div((FX_FLOAT)(j) - base, scale);
            double dest_end = FXSYS_Div((FX_FLOAT)(j + 1) - base, scale);
            if (dest_start > dest_end) {
                double temp = dest_start;
                dest_start = dest_end;
                dest_end = temp;
            }
            double area_start = dest_start > (FX_FLOAT)(dest_pixel) ? dest_start : (FX_FLOAT)(dest_pixel);
            double area_end = dest_end > (FX_FLOAT)(dest_pixel + 1) ? (FX_FLOAT)(dest_pixel + 1) : dest_end;
            double weight = area_start >= area_end ? 0.0f : area_end - area_start;
            if (weight == 0 && j == end_i) {
                pixel_weights.m_SrcEnd --;
                break;
            }
            pixel_weights.m_Weights[j - start_i] = FXSYS_round((FX_FLOAT)(weight * 65536));
        }
    }
}