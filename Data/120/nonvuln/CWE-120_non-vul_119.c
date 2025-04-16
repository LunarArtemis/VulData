static void print_escaped(const uint8_t* data, size_t length)
{
  for (size_t i = 0; i < length; i++)
  {
    switch (data[i])
    {
    case '\"':
    case '\'':
    case '\\':
      _tprintf(_T("\\%" PF_C), data[i]);
      break;

    default:
      if (data[i] >= 127)
        _tprintf(_T("\\%03o"), data[i]);
      else if (data[i] >= 32)
        _tprintf(_T("%" PF_C), data[i]);
      else if (cescapes[data[i]] != 0)
        _tprintf(_T("\\%" PF_C), cescapes[data[i]]);
      else
        _tprintf(_T("\\%03o"), data[i]);
    }
  }
}