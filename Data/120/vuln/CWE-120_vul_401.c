static uint64_t sniff_channel_order(uint8_t (*layout_map)[3], int tags)
{
    int i, n, total_non_cc_elements;
    struct elem_to_channel e2c_vec[4 * MAX_ELEM_ID] = { { 0 } };
    int num_front_channels, num_side_channels, num_back_channels;
    uint64_t layout = 0;

    if (FF_ARRAY_ELEMS(e2c_vec) < tags)
        return 0;

    i = 0;
    num_front_channels =
        count_paired_channels(layout_map, tags, AAC_CHANNEL_FRONT, &i);
    if (num_front_channels < 0)
        return 0;
    num_side_channels =
        count_paired_channels(layout_map, tags, AAC_CHANNEL_SIDE, &i);
    if (num_side_channels < 0)
        return 0;
    num_back_channels =
        count_paired_channels(layout_map, tags, AAC_CHANNEL_BACK, &i);
    if (num_back_channels < 0)
        return 0;

    if (num_side_channels == 0 && num_back_channels >= 4) {
        num_side_channels = 2;
        num_back_channels -= 2;
    }

    i = 0;
    if (num_front_channels & 1) {
        e2c_vec[i] = (struct elem_to_channel) {
            .av_position  = AV_CH_FRONT_CENTER,
            .syn_ele      = TYPE_SCE,
            .elem_id      = layout_map[i][1],
            .aac_position = AAC_CHANNEL_FRONT
        };
        layout |= e2c_vec[i].av_position;
        i++;
        num_front_channels--;
    }
    if (num_front_channels >= 4) {
        i += assign_pair(e2c_vec, layout_map, i,
                         AV_CH_FRONT_LEFT_OF_CENTER,
                         AV_CH_FRONT_RIGHT_OF_CENTER,
                         AAC_CHANNEL_FRONT, &layout);
        num_front_channels -= 2;
    }
    if (num_front_channels >= 2) {
        i += assign_pair(e2c_vec, layout_map, i,
                         AV_CH_FRONT_LEFT,
                         AV_CH_FRONT_RIGHT,
                         AAC_CHANNEL_FRONT, &layout);
        num_front_channels -= 2;
    }
    while (num_front_channels >= 2) {
        i += assign_pair(e2c_vec, layout_map, i,
                         UINT64_MAX,
                         UINT64_MAX,
                         AAC_CHANNEL_FRONT, &layout);
        num_front_channels -= 2;
    }

    if (num_side_channels >= 2) {
        i += assign_pair(e2c_vec, layout_map, i,
                         AV_CH_SIDE_LEFT,
                         AV_CH_SIDE_RIGHT,
                         AAC_CHANNEL_FRONT, &layout);
        num_side_channels -= 2;
    }
    while (num_side_channels >= 2) {
        i += assign_pair(e2c_vec, layout_map, i,
                         UINT64_MAX,
                         UINT64_MAX,
                         AAC_CHANNEL_SIDE, &layout);
        num_side_channels -= 2;
    }

    while (num_back_channels >= 4) {
        i += assign_pair(e2c_vec, layout_map, i,
                         UINT64_MAX,
                         UINT64_MAX,
                         AAC_CHANNEL_BACK, &layout);
        num_back_channels -= 2;
    }
    if (num_back_channels >= 2) {
        i += assign_pair(e2c_vec, layout_map, i,
                         AV_CH_BACK_LEFT,
                         AV_CH_BACK_RIGHT,
                         AAC_CHANNEL_BACK, &layout);
        num_back_channels -= 2;
    }
    if (num_back_channels) {
        e2c_vec[i] = (struct elem_to_channel) {
            .av_position  = AV_CH_BACK_CENTER,
            .syn_ele      = TYPE_SCE,
            .elem_id      = layout_map[i][1],
            .aac_position = AAC_CHANNEL_BACK
        };
        layout |= e2c_vec[i].av_position;
        i++;
        num_back_channels--;
    }

    if (i < tags && layout_map[i][2] == AAC_CHANNEL_LFE) {
        e2c_vec[i] = (struct elem_to_channel) {
            .av_position  = AV_CH_LOW_FREQUENCY,
            .syn_ele      = TYPE_LFE,
            .elem_id      = layout_map[i][1],
            .aac_position = AAC_CHANNEL_LFE
        };
        layout |= e2c_vec[i].av_position;
        i++;
    }
    if (i < tags && layout_map[i][2] == AAC_CHANNEL_LFE) {
        e2c_vec[i] = (struct elem_to_channel) {
            .av_position  = AV_CH_LOW_FREQUENCY_2,
            .syn_ele      = TYPE_LFE,
            .elem_id      = layout_map[i][1],
            .aac_position = AAC_CHANNEL_LFE
        };
        layout |= e2c_vec[i].av_position;
        i++;
    }
    while (i < tags && layout_map[i][2] == AAC_CHANNEL_LFE) {
        e2c_vec[i] = (struct elem_to_channel) {
            .av_position  = UINT64_MAX,
            .syn_ele      = TYPE_LFE,
            .elem_id      = layout_map[i][1],
            .aac_position = AAC_CHANNEL_LFE
        };
        i++;
    }

    // The previous checks would end up at 8 at this point for 22.2
    if (tags == 16 && i == 8) {
        e2c_vec[i] = (struct elem_to_channel) {
            .av_position  = AV_CH_TOP_FRONT_CENTER,
            .syn_ele      = layout_map[i][0],
            .elem_id      = layout_map[i][1],
            .aac_position = layout_map[i][2]
        }; layout |= e2c_vec[i].av_position; i++;
        i += assign_pair(e2c_vec, layout_map, i,
                         AV_CH_TOP_FRONT_LEFT,
                         AV_CH_TOP_FRONT_RIGHT,
                         AAC_CHANNEL_FRONT,
                         &layout);
        i += assign_pair(e2c_vec, layout_map, i,
                         AV_CH_TOP_SIDE_LEFT,
                         AV_CH_TOP_SIDE_RIGHT,
                         AAC_CHANNEL_SIDE,
                         &layout);
        e2c_vec[i] = (struct elem_to_channel) {
            .av_position  = AV_CH_TOP_CENTER,
            .syn_ele      = layout_map[i][0],
            .elem_id      = layout_map[i][1],
            .aac_position = layout_map[i][2]
        }; layout |= e2c_vec[i].av_position; i++;
        i += assign_pair(e2c_vec, layout_map, i,
                         AV_CH_TOP_BACK_LEFT,
                         AV_CH_TOP_BACK_RIGHT,
                         AAC_CHANNEL_BACK,
                         &layout);
        e2c_vec[i] = (struct elem_to_channel) {
            .av_position  = AV_CH_TOP_BACK_CENTER,
            .syn_ele      = layout_map[i][0],
            .elem_id      = layout_map[i][1],
            .aac_position = layout_map[i][2]
        }; layout |= e2c_vec[i].av_position; i++;
        e2c_vec[i] = (struct elem_to_channel) {
            .av_position  = AV_CH_BOTTOM_FRONT_CENTER,
            .syn_ele      = layout_map[i][0],
            .elem_id      = layout_map[i][1],
            .aac_position = layout_map[i][2]
        }; layout |= e2c_vec[i].av_position; i++;
        i += assign_pair(e2c_vec, layout_map, i,
                         AV_CH_BOTTOM_FRONT_LEFT,
                         AV_CH_BOTTOM_FRONT_RIGHT,
                         AAC_CHANNEL_FRONT,
                         &layout);
    }

    total_non_cc_elements = n = i;

    if (tags == 16 && total_non_cc_elements == 16) {
        // For 22.2 reorder the result as needed
        FFSWAP(struct elem_to_channel, e2c_vec[2], e2c_vec[0]);   // FL & FR first (final), FC third
        FFSWAP(struct elem_to_channel, e2c_vec[2], e2c_vec[1]);   // FC second (final), FLc & FRc third
        FFSWAP(struct elem_to_channel, e2c_vec[6], e2c_vec[2]);   // LFE1 third (final), FLc & FRc seventh
        FFSWAP(struct elem_to_channel, e2c_vec[4], e2c_vec[3]);   // BL & BR fourth (final), SiL & SiR fifth
        FFSWAP(struct elem_to_channel, e2c_vec[6], e2c_vec[4]);   // FLc & FRc fifth (final), SiL & SiR seventh
        FFSWAP(struct elem_to_channel, e2c_vec[7], e2c_vec[6]);   // LFE2 seventh (final), SiL & SiR eight (final)
        FFSWAP(struct elem_to_channel, e2c_vec[9], e2c_vec[8]);   // TpFL & TpFR ninth (final), TFC tenth (final)
        FFSWAP(struct elem_to_channel, e2c_vec[11], e2c_vec[10]); // TC eleventh (final), TpSiL & TpSiR twelth
        FFSWAP(struct elem_to_channel, e2c_vec[12], e2c_vec[11]); // TpBL & TpBR twelth (final), TpSiL & TpSiR thirteenth (final)
    } else {
        // For everything else, utilize the AV channel position define as a
        // stable sort.
        do {
            int next_n = 0;
            for (i = 1; i < n; i++)
                if (e2c_vec[i - 1].av_position > e2c_vec[i].av_position) {
                    FFSWAP(struct elem_to_channel, e2c_vec[i - 1], e2c_vec[i]);
                    next_n = i;
                }
            n = next_n;
        } while (n > 0);

    }

    for (i = 0; i < total_non_cc_elements; i++) {
        layout_map[i][0] = e2c_vec[i].syn_ele;
        layout_map[i][1] = e2c_vec[i].elem_id;
        layout_map[i][2] = e2c_vec[i].aac_position;
    }

    return layout;
}