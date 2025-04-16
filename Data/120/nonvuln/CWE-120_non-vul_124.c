static int file_queue_put(const char_t* file_path, time_t deadline)
{
  if (cli_semaphore_wait(&unused_slots, deadline) == ERROR_SCAN_TIMEOUT)
    return ERROR_SCAN_TIMEOUT;

  cli_mutex_lock(&queue_mutex);

  file_queue[queue_tail].path = _tcsdup(file_path);
  queue_tail = (queue_tail + 1) % (MAX_QUEUED_FILES + 1);

  cli_mutex_unlock(&queue_mutex);
  cli_semaphore_release(&used_slots);

  return ERROR_SUCCESS;
}