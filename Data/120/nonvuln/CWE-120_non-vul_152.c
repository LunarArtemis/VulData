YR_API void yr_scanner_set_flags(YR_SCANNER* scanner, int flags)
{
  // For backward compatibility, if neither SCAN_FLAGS_REPORT_RULES_MATCHING
  // nor SCAN_FLAGS_REPORT_RULES_NOT_MATCHING are specified, both are assumed.

  if (!(flags & SCAN_FLAGS_REPORT_RULES_MATCHING) &&
      !(flags & SCAN_FLAGS_REPORT_RULES_NOT_MATCHING))
  {
    flags |= SCAN_FLAGS_REPORT_RULES_MATCHING |
             SCAN_FLAGS_REPORT_RULES_NOT_MATCHING;
  }

  scanner->flags = flags;
}