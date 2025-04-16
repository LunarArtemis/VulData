static ecma_value_t
ecma_builtin_typedarray_prototype_filter (ecma_value_t this_arg, /**< this object */
                                          ecma_typedarray_info_t *info_p, /**< object info */
                                          ecma_value_t cb_func_val, /**< callback function */
                                          ecma_value_t cb_this_arg) /**< 'this' of the callback function */
{
  ecma_typedarray_getter_fn_t getter_cb = ecma_get_typedarray_getter_fn (info_p->id);

  ecma_object_t *func_object_p = ecma_get_object_from_value (cb_func_val);
  ecma_value_t ret_value = ECMA_VALUE_ERROR;

  // TODO: 22.2.3.9, 7-8.
  if (info_p->length == 0)
  {
    return ecma_op_create_typedarray_with_type_and_length (info_p->id, 0);
  }

  JMEM_DEFINE_LOCAL_ARRAY (pass_value_list_p, info_p->length * info_p->element_size, lit_utf8_byte_t);

  lit_utf8_byte_t *pass_value_p = pass_value_list_p;
  uint32_t byte_pos = 0;

  for (uint32_t index = 0; index < info_p->length; index++)
  {
    ecma_value_t current_index = ecma_make_uint32_value (index);
    ecma_value_t get_value = getter_cb (info_p->buffer_p + byte_pos);

    JERRY_ASSERT (ecma_is_value_number (get_value) || ecma_is_value_bigint (get_value));

    ecma_value_t call_args[] = { get_value, current_index, this_arg };

    ecma_value_t call_value = ecma_op_function_call (func_object_p, cb_this_arg, call_args, 3);

    ecma_fast_free_value (current_index);
    ecma_fast_free_value (get_value);

    if (ECMA_IS_VALUE_ERROR (call_value))
    {
      goto cleanup;
    }

    if (ecma_arraybuffer_is_detached (info_p->array_buffer_p))
    {
      ecma_free_value (call_value);
      ecma_raise_type_error (ECMA_ERR_MSG (ecma_error_arraybuffer_is_detached));
      goto cleanup;
    }

    if (ecma_op_to_boolean (call_value))
    {
      memcpy (pass_value_p, info_p->buffer_p + byte_pos, info_p->element_size);
      pass_value_p += info_p->element_size;
    }

    byte_pos += info_p->element_size;

    ecma_free_value (call_value);
  }

  uint32_t pass_num = (uint32_t) ((pass_value_p - pass_value_list_p) >> info_p->shift);

  ecma_value_t collected = ecma_make_number_value (pass_num);
  ret_value = ecma_typedarray_species_create (this_arg, &collected, 1);
  ecma_free_value (collected);

  if (!ECMA_IS_VALUE_ERROR (ret_value))
  {
    ecma_object_t *obj_p = ecma_get_object_from_value (ret_value);

    JERRY_ASSERT (ecma_typedarray_get_offset (obj_p) == 0);

    memcpy (ecma_typedarray_get_buffer (obj_p),
            pass_value_list_p,
            (size_t) (pass_value_p - pass_value_list_p));
  }

cleanup:
  JMEM_FINALIZE_LOCAL_ARRAY (pass_value_list_p);

  return ret_value;
}