static int _yr_scan_verify_chained_string_match(
    YR_STRING* matching_string,
    YR_SCAN_CONTEXT* context,
    const uint8_t* match_data,
    uint64_t match_base,
    uint64_t match_offset,
    int32_t match_length)
{
  YR_DEBUG_FPRINTF(
      2,
      stderr,
      "- %s (match_data=%p match_base=%" PRIx64 " match_offset=0x%" PRIx64
      " match_length=%'d) {} \n",
      __FUNCTION__,
      match_data,
      match_base,
      match_offset,
      match_length);

  YR_STRING* string;
  YR_MATCH* match;
  YR_MATCH* next_match;
  YR_MATCH* new_match;

  uint64_t lowest_offset;
  uint64_t ending_offset;
  int32_t full_chain_length;

  bool add_match = false;

  if (matching_string->chained_to == NULL)
  {
    // The matching string is the head of the chain, this match should be
    // added to the list of unconfirmed matches. The match will remain
    // unconfirmed until all the strings in the chain are found with the
    // correct distances between them.
    add_match = true;
  }
  else
  {
    // If some unconfirmed match exists, the lowest possible offset where the
    // whole string chain can match is the offset of the first string in the
    // list of unconfirmed matches. Unconfirmed matches are sorted in ascending
    // offset order. If no unconfirmed match exists, the lowest possible offset
    // is the offset of the current match.
    match = context->unconfirmed_matches[matching_string->idx].head;

    if (match != NULL)
      lowest_offset = match->offset;
    else
      lowest_offset = match_offset;

    // Iterate over the list of unconfirmed matches for the string that
    // precedes the currently matching string. If we have a string chain like:
    // S1 <- S2 <- S3, and we just found a match for S2, we are iterating the
    // list of unconfirmed matches of S1.
    match = context->unconfirmed_matches[matching_string->chained_to->idx].head;

    while (match != NULL)
    {
      // Store match->next so that we can use it later for advancing in the
      // list, if _yr_scan_remove_match_from_list is called, match->next is
      // set to NULL, that's why we store its current value before that happens.
      next_match = match->next;

      // The unconfirmed match starts at match->offset and finishes at
      // ending_offset.
      ending_offset = match->offset + match->match_length;

      if (ending_offset + matching_string->chain_gap_max < lowest_offset)
      {
        // If the current match is too far away from the unconfirmed match,
        // remove the unconfirmed match from the list because it has been
        // negatively confirmed (i.e: we can be sure that this unconfirmed
        // match can't be an actual match)
        _yr_scan_remove_match_from_list(
            match,
            &context->unconfirmed_matches[matching_string->chained_to->idx]);
      }
      else if (
          ending_offset + matching_string->chain_gap_max >= match_offset &&
          ending_offset + matching_string->chain_gap_min <= match_offset)
      {
        // If the distance between the end of the unconfirmed match and the
        // start of the current match is within the range specified in the
        // regexp or hex string, this could be an actual match.
        add_match = true;
        break;
      }

      match = next_match;
    }
  }

  if (add_match)
  {
    uint32_t max_match_data;

    FAIL_ON_ERROR(
        yr_get_configuration(YR_CONFIG_MAX_MATCH_DATA, &max_match_data))

    if (STRING_IS_CHAIN_TAIL(matching_string))
    {
      // The matching string is the tail of the string chain. It must be
      // chained to some other string.
      assert(matching_string->chained_to != NULL);

      // Iterate over the list of unconfirmed matches of the preceding string
      // in the chain and update the chain_length field for each of them. This
      // is a recursive operation that will update the chain_length field for
      // every unconfirmed match in all the strings in the chain up to the head
      // of the chain.
      match =
          context->unconfirmed_matches[matching_string->chained_to->idx].head;

      while (match != NULL)
      {
        ending_offset = match->offset + match->match_length;

        if (ending_offset + matching_string->chain_gap_max >= match_offset &&
            ending_offset + matching_string->chain_gap_min <= match_offset)
        {
          _yr_scan_update_match_chain_length(
              context, matching_string->chained_to, match, 1);
        }

        match = match->next;
      }

      full_chain_length = 0;
      string = matching_string;

      while (string->chained_to != NULL)
      {
        full_chain_length++;
        string = string->chained_to;
      }

      // "string" points now to the head of the strings chain.
      match = context->unconfirmed_matches[string->idx].head;

      // Iterate over the list of unconfirmed matches of the head of the chain,
      // and move to the list of confirmed matches those with a chain_length
      // equal to full_chain_length, which means that the whole chain has been
      // confirmed to match.
      while (match != NULL)
      {
        next_match = match->next;

        if (match->chain_length == full_chain_length)
        {
          _yr_scan_remove_match_from_list(
              match, &context->unconfirmed_matches[string->idx]);

          match->match_length = (int32_t)(
              match_offset - match->offset + match_length);

          match->data_length = yr_min(
              match->match_length, (int32_t) max_match_data);

          match->data = yr_notebook_alloc(
              context->matches_notebook, match->data_length);

          if (match->data == NULL)
            return ERROR_INSUFFICIENT_MEMORY;

          memcpy(
              (void*) match->data,
              match_data - match_offset + match->offset,
              match->data_length);

          FAIL_ON_ERROR(_yr_scan_add_match_to_list(
              match, &context->matches[string->idx], false));
        }

        match = next_match;
      }
    }
    else  // It's a part of a chain, but not the tail.
    {
      new_match = yr_notebook_alloc(
          context->matches_notebook, sizeof(YR_MATCH));

      if (new_match == NULL)
        return ERROR_INSUFFICIENT_MEMORY;

      new_match->base = match_base;
      new_match->offset = match_offset;
      new_match->match_length = match_length;
      new_match->chain_length = 0;
      new_match->prev = NULL;
      new_match->next = NULL;
      new_match->is_private = STRING_IS_PRIVATE(matching_string);

      // A copy of the matching data is written to the matches_arena, the
      // amount of data copies is limited by YR_CONFIG_MAX_MATCH_DATA.
      new_match->data_length = yr_min(match_length, (int32_t) max_match_data);

      if (new_match->data_length > 0)
      {
        new_match->data = yr_notebook_alloc(
            context->matches_notebook, new_match->data_length);

        if (new_match->data == NULL)
          return ERROR_INSUFFICIENT_MEMORY;

        memcpy((void*) new_match->data, match_data, new_match->data_length);
      }
      else
      {
        new_match->data = NULL;
      }

      // Add the match to the list of unconfirmed matches because the string
      // is part of a chain but not its tail, so we can't be sure the this is
      // an actual match until finding the remaining parts of the chain.
      FAIL_ON_ERROR(_yr_scan_add_match_to_list(
          new_match,
          &context->unconfirmed_matches[matching_string->idx],
          false));
    }
  }

  return ERROR_SUCCESS;
}