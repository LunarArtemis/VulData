static int _yr_scan_add_match_to_list(
    YR_MATCH* match,
    YR_MATCHES* matches_list,
    int replace_if_exists)
{
  int result = ERROR_SUCCESS;

#if YR_DEBUG_VERBOSITY > 0
  int32_t count_orig = matches_list->count;
#endif

  YR_MATCH* insertion_point = matches_list->tail;

  if (matches_list->count == YR_MAX_STRING_MATCHES)
  {
    result = ERROR_TOO_MANY_MATCHES;
    goto _exit;
  }

  while (insertion_point != NULL)
  {
    if ((match->base + match->offset) ==
        (insertion_point->base + insertion_point->offset))
    {
      if (replace_if_exists)
      {
        insertion_point->match_length = match->match_length;
        insertion_point->data_length = match->data_length;
        insertion_point->data = match->data;
      }

      goto _exit;  // return ERROR_SUCCESS
    }

    if ((match->base + match->offset) >
        (insertion_point->base + insertion_point->offset))
      break;

    insertion_point = insertion_point->prev;
  }

  match->prev = insertion_point;

  if (insertion_point != NULL)
  {
    match->next = insertion_point->next;
    insertion_point->next = match;
  }
  else
  {
    match->next = matches_list->head;
    matches_list->head = match;
  }

  matches_list->count++;

  if (match->next != NULL)
    match->next->prev = match;
  else
    matches_list->tail = match;

_exit:;

  YR_DEBUG_FPRINTF(
      2,
      stderr,
      "- %s(replace_if_exists=%d) {} = %d //"
      " match->base=0x%" PRIx64 " match->offset=%" PRIi64
      " matches_list->count=%u += %u\n",
      __FUNCTION__,
      replace_if_exists,
      result,
      match->base,
      match->offset,
      count_orig,
      matches_list->count - count_orig);

  return result;
}