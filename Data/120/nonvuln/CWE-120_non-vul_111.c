static void file_queue_finish()
{
  for (int i = 0; i < YR_MAX_THREADS; i++) cli_semaphore_release(&used_slots);
}