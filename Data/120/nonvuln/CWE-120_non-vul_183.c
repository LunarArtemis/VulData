static void _yr_scan_remove_match_from_list(
    YR_MATCH* match,
    YR_MATCHES* matches_list)
{
  if (match->prev != NULL)
    match->prev->next = match->next;

  if (match->next != NULL)
    match->next->prev = match->prev;

  if (matches_list->head == match)
    matches_list->head = match->next;

  if (matches_list->tail == match)
    matches_list->tail = match->prev;

  matches_list->count--;
  match->next = NULL;
  match->prev = NULL;
}