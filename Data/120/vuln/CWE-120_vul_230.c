int dnn_execute_layer_pad(DnnOperand *operands, const int32_t *input_operand_indexes,
                          int32_t output_operand_index, const void *parameters)
{
    int32_t before_paddings;
    int32_t after_paddings;
    float* output;
    const LayerPadParams *params = (const LayerPadParams *)parameters;

    // suppose format is <N, H, W, C>
    int32_t input_operand_index = input_operand_indexes[0];
    int number = operands[input_operand_index].dims[0];
    int height = operands[input_operand_index].dims[1];
    int width = operands[input_operand_index].dims[2];
    int channel = operands[input_operand_index].dims[3];
    const float *input = operands[input_operand_index].data;

    int new_number = number + params->paddings[0][0] + params->paddings[0][1];
    int new_height = height + params->paddings[1][0] + params->paddings[1][1];
    int new_width = width + params->paddings[2][0] + params->paddings[2][1];
    int new_channel = channel + params->paddings[3][0] + params->paddings[3][1];

    int c_stride = channel;
    int wc_stride = c_stride * width;
    int hwc_stride = wc_stride * height;

    int new_c_stride = new_channel;
    int new_wc_stride = new_c_stride * new_width;
    int new_hwc_stride = new_wc_stride * new_height;

    DnnOperand *output_operand = &operands[output_operand_index];
    output_operand->dims[0] = new_number;
    output_operand->dims[1] = new_height;
    output_operand->dims[2] = new_width;
    output_operand->dims[3] = new_channel;
    output_operand->data_type = operands[input_operand_index].data_type;
    output_operand->length = calculate_operand_data_length(output_operand);
    output_operand->data = av_realloc(output_operand->data, output_operand->length);
    if (!output_operand->data)
        return -1;
    output = output_operand->data;

    // copy the original data
    for (int n = 0; n < number; n++) {
        for (int h = 0; h < height; h++) {
            for (int w = 0; w < width; w++) {
                const float *src = input + n * hwc_stride + h * wc_stride + w * c_stride;
                float *dst = output + (n + params->paddings[0][0]) * new_hwc_stride
                                    + (h + params->paddings[1][0]) * new_wc_stride
                                    + (w + params->paddings[2][0]) * new_c_stride
                                    + params->paddings[3][0];
                memcpy(dst, src, channel * sizeof(float));
            }
        }
    }

    // handle the first dimension
    before_paddings = params->paddings[0][0];
    after_paddings = params->paddings[0][1];
    for (int n = 0; n < before_paddings; n++) {
        float *dst = output + n * new_hwc_stride;
        if (params->mode == LPMP_CONSTANT) {
            for (int i = 0; i < new_hwc_stride; i++) {
                dst[i] = params->constant_values;
            }
        }
        else {
            int buddy = before_get_buddy(n, before_paddings, params->mode);
            float *src = output + buddy * new_hwc_stride;
            memcpy(dst, src, new_hwc_stride * sizeof(float));
        }
    }
    for (int n = 0; n < after_paddings; n++) {
        int given = number + before_paddings + n;
        float *dst = output + given * new_hwc_stride;
        if (params->mode == LPMP_CONSTANT) {
            for (int i = 0; i < new_hwc_stride; i++) {
                dst[i] = params->constant_values;
            }
        } else {
            int buddy = after_get_buddy(given, number + before_paddings, params->mode);
            float *src = output + buddy * new_hwc_stride;
            memcpy(dst, src, new_hwc_stride * sizeof(float));
        }
    }

    // handle the second dimension
    before_paddings = params->paddings[1][0];
    after_paddings = params->paddings[1][1];
    for (int n = 0; n < new_number; n++) {
        float *start = output + n * new_hwc_stride;
        for (int h = 0; h < before_paddings; h++) {
            float *dst = start + h * new_wc_stride;
            if (params->mode == LPMP_CONSTANT) {
                for (int i = 0; i < new_wc_stride; i++) {
                    dst[i] = params->constant_values;
                }
            } else {
                int buddy = before_get_buddy(h, before_paddings, params->mode);
                float *src = start + buddy * new_wc_stride;
                memcpy(dst, src, new_wc_stride * sizeof(float));
            }
        }
        for (int h = 0; h < after_paddings; h++) {
            int given = height + before_paddings + h;
            float *dst = start + given * new_wc_stride;
            if (params->mode == LPMP_CONSTANT) {
                for (int i = 0; i < new_wc_stride; i++) {
                    dst[i] = params->constant_values;
                }
            } else {
                int buddy = after_get_buddy(given, height + before_paddings, params->mode);
                float *src = start + buddy * new_wc_stride;
                memcpy(dst, src, new_wc_stride * sizeof(float));
            }
        }
    }

    // handle the third dimension
    before_paddings = params->paddings[2][0];
    after_paddings = params->paddings[2][1];
    for (int n = 0; n < new_number; n++) {
        for (int h = 0; h < new_height; h++) {
            float *start = output + n * new_hwc_stride + h * new_wc_stride;
            for (int w = 0; w < before_paddings; w++) {
                float *dst = start + w * new_c_stride;
                if (params->mode == LPMP_CONSTANT) {
                    for (int i = 0; i < new_c_stride; i++) {
                        dst[i] = params->constant_values;
                    }
                } else {
                    int buddy = before_get_buddy(w, before_paddings, params->mode);
                    float *src = start + buddy * new_c_stride;
                    memcpy(dst, src, new_c_stride * sizeof(float));
                }
            }
            for (int w = 0; w < after_paddings; w++) {
                int given = width + before_paddings + w;
                float *dst = start + given * new_c_stride;
                if (params->mode == LPMP_CONSTANT) {
                    for (int i = 0; i < new_c_stride; i++) {
                        dst[i] = params->constant_values;
                    }
                } else {
                    int buddy = after_get_buddy(given, width + before_paddings, params->mode);
                    float *src = start + buddy * new_c_stride;
                    memcpy(dst, src, new_c_stride * sizeof(float));
                }
            }
        }
    }

    // handle the fourth dimension
    before_paddings = params->paddings[3][0];
    after_paddings = params->paddings[3][1];
    for (int n = 0; n < new_number; n++) {
        for (int h = 0; h < new_height; h++) {
            for (int w = 0; w < new_width; w++) {
                float *start = output + n * new_hwc_stride + h * new_wc_stride + w * new_c_stride;
                for (int c = 0; c < before_paddings; c++) {
                    float *dst = start + c;
                    if (params->mode == LPMP_CONSTANT) {
                        *dst = params->constant_values;
                    } else {
                        int buddy = before_get_buddy(c, before_paddings, params->mode);
                        float *src = start + buddy;
                        *dst = *src;
                    }
                }
                for (int c = 0; c < after_paddings; c++) {
                    int given = channel + before_paddings + c;
                    float *dst = start + given;
                    if (params->mode == LPMP_CONSTANT) {
                        *dst = params->constant_values;
                    } else {
                        int buddy = after_get_buddy(given, channel + before_paddings, params->mode);
                        float *src = start + buddy;
                        *dst = *src;
                    }
                }
            }
        }
    }

    return 0;
}