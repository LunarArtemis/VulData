static njs_ret_t
njs_array_prototype_slice_copy(njs_vm_t *vm, njs_value_t *this,
    int64_t start, int64_t length)
{
    size_t             size;
    u_char             *dst;
    uint32_t           n;
    njs_ret_t          ret;
    njs_array_t        *array;
    njs_value_t        *value, name;
    const u_char       *src, *end;
    njs_slice_prop_t   string_slice;
    njs_string_prop_t  string;

    array = njs_array_alloc(vm, length, NJS_ARRAY_SPARE);
    if (nxt_slow_path(array == NULL)) {
        return NXT_ERROR;
    }

    njs_set_array(&vm->retval, array);

    if (length != 0) {
        n = 0;

        if (nxt_fast_path(njs_is_array(this))) {
            value = njs_array_start(this);

            do {
                /* GC: retain long string and object in values[start]. */
                array->start[n++] = value[start++];
                length--;
            } while (length != 0);

        } else if (njs_is_string(this) || this->type == NJS_OBJECT_STRING) {

            if (this->type == NJS_OBJECT_STRING) {
                this = &this->data.u.object_value->value;
            }

            string_slice.start = start;
            string_slice.length = length;
            string_slice.string_length = njs_string_prop(&string, this);

            njs_string_slice_string_prop(&string, &string, &string_slice);

            src = string.start;
            end = src + string.size;

            if (string.length == 0) {
                /* Byte string. */
                do {
                    value = &array->start[n++];
                    dst = njs_string_short_start(value);
                    *dst = *src++;
                    njs_string_short_set(value, 1, 0);

                    length--;
                } while (length != 0);

            } else {
                /* UTF-8 or ASCII string. */
                do {
                    value = &array->start[n++];
                    dst = njs_string_short_start(value);
                    dst = nxt_utf8_copy(dst, &src, end);
                    size = dst - njs_string_short_start(value);
                    njs_string_short_set(value, size, 1);

                    length--;
                } while (length != 0);
            }

        } else if (njs_is_object(this)) {

            do {
                njs_uint32_to_string(&name, start++);

                value = &array->start[n++];
                ret = njs_value_property(vm, this, &name, value, 0);

                if (ret != NXT_OK) {
                    *value = njs_value_invalid;
                }

                length--;
            } while (length != 0);
        }
    }

    return NXT_OK;
}