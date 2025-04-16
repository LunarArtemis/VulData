YR_API YR_MEMORY_BLOCK* yr_process_get_next_memory_block(
    YR_MEMORY_BLOCK_ITERATOR* iterator)
{
  YR_PROC_ITERATOR_CTX* context = (YR_PROC_ITERATOR_CTX*) iterator->context;
  YR_PROC_INFO* proc_info = context->proc_info;

  kern_return_t kr;
  mach_msg_type_number_t info_count;
  mach_port_t object;
  vm_region_basic_info_data_64_t info;
  vm_size_t size = 0;

  uint64_t current_begin = (vm_address_t) context->current_block.base +
                           context->current_block.size;
  vm_address_t address = current_begin;
  uint64_t max_process_memory_chunk;

  yr_get_configuration(
      YR_CONFIG_MAX_PROCESS_MEMORY_CHUNK, (void*) &max_process_memory_chunk);

  iterator->last_error = ERROR_SUCCESS;

  do
  {
    info_count = VM_REGION_BASIC_INFO_COUNT_64;

    kr = vm_region_64(
        proc_info->task,
        &address,
        &size,
        VM_REGION_BASIC_INFO,
        (vm_region_info_t) &info,
        &info_count,
        &object);

    if (kr == KERN_SUCCESS)
    {
      size_t chunk_size = size - (size_t) (current_begin - address);

      if (((uint64_t) chunk_size) > max_process_memory_chunk)
      {
        chunk_size = (size_t) max_process_memory_chunk;
      }

      context->current_block.base = (size_t) current_begin;
      context->current_block.size = chunk_size;

      return &context->current_block;
    }

    current_begin = address;

  } while (kr != KERN_INVALID_ADDRESS);

  return NULL;
}