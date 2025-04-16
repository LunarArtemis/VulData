static char_t* file_queue_get(time_t deadline)
{
  char_t* result;

  if (cli_semaphore_wait(&used_slots, deadline) == ERROR_SCAN_TIMEOUT)
    return NULL;

  cli_mutex_lock(&queue_mutex);

  if (queue_head == queue_tail)  // queue is empty
  {
    result = NULL;
  }
  else
  {
    result = file_queue[queue_head].path;
    queue_head = (queue_head + 1) % (MAX_QUEUED_FILES + 1);
  }

  cli_mutex_unlock(&queue_mutex);
  cli_semaphore_release(&unused_slots);

  return result;
}