static void print_hex_string(const uint8_t* data, int length)
{
  for (int i = 0; i < min(64, length); i++)
    _tprintf(_T("%s%02X"), (i == 0 ? _T("") : _T(" ")), data[i]);

  if (length > 64)
    _tprintf(_T(" ..."));
}