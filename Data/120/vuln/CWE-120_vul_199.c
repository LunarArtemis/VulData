static void
_joinW(WCHAR *dest_path, const WCHAR *root, const WCHAR *rest)
{
    size_t root_len;

    if (_is_absW(rest)) {
        wcscpy(dest_path, rest);
        return;
    }

    root_len = wcslen(root);

    wcscpy(dest_path, root);
    if(root_len) {
        dest_path[root_len] = L'\\';
        root_len++;
    }
    wcscpy(dest_path+root_len, rest);
}