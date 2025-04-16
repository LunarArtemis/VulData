static void print_string(const uint8_t* data, int length)
{
  for (int i = 0; i < length; i++)
  {
    if (data[i] >= 32 && data[i] <= 126)
      _tprintf(_T("%c"), data[i]);
    else
      _tprintf(_T("\\x%02X"), data[i]);
  }
}