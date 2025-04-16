static void file_queue_destroy()
{
  cli_mutex_destroy(&queue_mutex);
  cli_semaphore_destroy(&unused_slots);
  cli_semaphore_destroy(&used_slots);
}