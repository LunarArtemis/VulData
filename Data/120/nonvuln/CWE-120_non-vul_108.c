static int file_queue_init()
{
  int result;

  queue_tail = 0;
  queue_head = 0;

  result = cli_mutex_init(&queue_mutex);

  if (result != 0)
    return result;

  result = cli_semaphore_init(&used_slots, 0);

  if (result != 0)
    return result;

  return cli_semaphore_init(&unused_slots, MAX_QUEUED_FILES);
}