static int
_is_absW(const WCHAR *path)
{
    return path[0] == L'\\' || path[0] == L'/' || path[1] == L':';

}