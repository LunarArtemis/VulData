static PyObject *
os_symlink_impl(PyObject *module, path_t *src, path_t *dst,
                int target_is_directory, int dir_fd)
/*[clinic end generated code: output=08ca9f3f3cf960f6 input=e820ec4472547bc3]*/
{
#ifdef MS_WINDOWS
    DWORD result;
#else
    int result;
#endif

#ifdef MS_WINDOWS
    if (!check_CreateSymbolicLink()) {
        PyErr_SetString(PyExc_NotImplementedError,
            "CreateSymbolicLink functions not found");
        return NULL;
        }
    if (!win32_can_symlink) {
        PyErr_SetString(PyExc_OSError, "symbolic link privilege not held");
        return NULL;
        }
#endif

    if ((src->narrow && dst->wide) || (src->wide && dst->narrow)) {
        PyErr_SetString(PyExc_ValueError,
            "symlink: src and dst must be the same type");
        return NULL;
    }

#ifdef MS_WINDOWS

    Py_BEGIN_ALLOW_THREADS
    /* if src is a directory, ensure target_is_directory==1 */
    target_is_directory |= _check_dirW(src->wide, dst->wide);
    result = Py_CreateSymbolicLinkW(dst->wide, src->wide,
                                    target_is_directory);
    Py_END_ALLOW_THREADS

    if (!result)
        return path_error2(src, dst);

#else

    Py_BEGIN_ALLOW_THREADS
#if HAVE_SYMLINKAT
    if (dir_fd != DEFAULT_DIR_FD)
        result = symlinkat(src->narrow, dir_fd, dst->narrow);
    else
#endif
        result = symlink(src->narrow, dst->narrow);
    Py_END_ALLOW_THREADS

    if (result)
        return path_error2(src, dst);
#endif

    Py_RETURN_NONE;
}