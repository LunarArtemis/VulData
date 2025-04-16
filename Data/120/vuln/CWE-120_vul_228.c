int32_t calculate_operand_data_length(const DnnOperand* oprd)
{
    // currently, we just support DNN_FLOAT
    return oprd->dims[0] * oprd->dims[1] * oprd->dims[2] * oprd->dims[3] * sizeof(float);
}