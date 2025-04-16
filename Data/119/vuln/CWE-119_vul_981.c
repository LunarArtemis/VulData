static void
advance_to_end_of_signature(guint8 **signature,
                            guint8  *signature_length)
{
    gboolean done = FALSE;
    gint8 current_type;
    gint8 end_type = ARG_INVALID;

    while(*(++(*signature)) && --(*signature_length) > 0 && !done) {
        current_type = **signature;

        /* Were we looking for the end of a structure or dictionary? If so, did we find it? */
        if(end_type != ARG_INVALID) {
            if(end_type == current_type) {
                done = TRUE; /* Found the end of the structure or dictionary. All done. */
            }

            continue;
        }

        switch(current_type)
        {
        case ARG_ARRAY:
            advance_to_end_of_signature(signature, signature_length);
            break;
        case ARG_STRUCT:
            end_type = ')';
            advance_to_end_of_signature(signature, signature_length);
            break;
        case ARG_DICT_ENTRY:
            end_type = '}';
            advance_to_end_of_signature(signature, signature_length);
            break;

        case ARG_BYTE:
        case ARG_DOUBLE:
        case ARG_UINT64:
        case ARG_INT64:
        case ARG_SIGNATURE:
        case ARG_HANDLE:
        case ARG_INT32:
        case ARG_UINT32:
        case ARG_BOOLEAN:
        case ARG_INT16:
        case ARG_UINT16:
        case ARG_STRING:
        case ARG_VARIANT:
        case ARG_OBJ_PATH:
            done = TRUE;
            break;

        default:    /* Unrecognized signature. Bail out. */
            done = TRUE;
            break;
        }
    }
}