static int flask_copyin_string(XEN_GUEST_HANDLE_PARAM(char) u_buf, char **buf, uint32_t size)
{
    char *tmp = xmalloc_bytes(size + 1);
    if ( !tmp )
        return -ENOMEM;

    if ( copy_from_guest(tmp, u_buf, size) )
    {
        xfree(tmp);
        return -EFAULT;
    }
    tmp[size] = 0;

    *buf = tmp;
    return 0;
}