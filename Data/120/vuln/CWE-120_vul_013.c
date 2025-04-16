YR_API YR_MEMORY_BLOCK* yr_process_get_next_memory_block(
    YR_MEMORY_BLOCK_ITERATOR* iterator)
{
  YR_PROC_ITERATOR_CTX* context = (YR_PROC_ITERATOR_CTX*) iterator->context;
  YR_PROC_INFO* proc_info = (YR_PROC_INFO*) context->proc_info;

  int mib[] = {CTL_KERN, KERN_PROC_VMMAP, proc_info->pid};
  size_t len = sizeof(struct kinfo_vmentry);

  iterator->last_error = ERROR_SUCCESS;

  uint64_t current_begin = context->current_block.base +
                           context->current_block.size;

  uint64_t max_process_memory_chunk;

  yr_get_configuration(
      YR_CONFIG_MAX_PROCESS_MEMORY_CHUNK, (void*) &max_process_memory_chunk);

  if (proc_info->old_end <= current_begin)
  {
    if (sysctl(mib, 3, &proc_info->vm_entry, &len, NULL, 0) < 0)
      return NULL;

    // no more blocks
    if (proc_info->old_end == proc_info->vm_entry.kve_end)
      return NULL;

    current_begin = proc_info->vm_entry.kve_start;
    proc_info->old_end = proc_info->vm_entry.kve_end;

    proc_info->vm_entry.kve_start = proc_info->vm_entry.kve_start + 1;
  }

  context->current_block.base = current_begin;
  context->current_block.size = yr_min(
      proc_info->old_end - current_begin, max_process_memory_chunk);

  assert(context->current_block.size > 0);

  return &context->current_block;
}