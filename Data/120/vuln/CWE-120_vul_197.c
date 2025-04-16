static int
_check_dirW(LPCWSTR src, LPCWSTR dest)
{
    WIN32_FILE_ATTRIBUTE_DATA src_info;
    WCHAR dest_parent[MAX_PATH];
    WCHAR src_resolved[MAX_PATH] = L"";

    /* dest_parent = os.path.dirname(dest) */
    wcscpy(dest_parent, dest);
    _dirnameW(dest_parent);
    /* src_resolved = os.path.join(dest_parent, src) */
    _joinW(src_resolved, dest_parent, src);
    return (
        GetFileAttributesExW(src_resolved, GetFileExInfoStandard, &src_info)
        && src_info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY
    );
}