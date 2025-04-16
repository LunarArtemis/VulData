static nxt_int_t
njs_module_read(njs_vm_t *vm, int fd, nxt_str_t *text)
{
    u_char       *p;
    ssize_t      n;
    struct stat  sb;

    if (fstat(fd, &sb) == -1) {
        goto fail;
    }

    text->length = nxt_length(NJS_MODULE_START);

    if (S_ISREG(sb.st_mode) && sb.st_size) {
        text->length += sb.st_size;
    }

    text->length += nxt_length(NJS_MODULE_END);

    text->start = nxt_mp_alloc(vm->mem_pool, text->length);
    if (text->start == NULL) {
        goto fail;
    }

    p = nxt_cpymem(text->start, NJS_MODULE_START, nxt_length(NJS_MODULE_START));

    n = read(fd, p, sb.st_size);

    if (n < 0) {
        goto fail;
    }

    if (n != sb.st_size) {
        goto fail;
    }

    p += n;

    memcpy(p, NJS_MODULE_END, nxt_length(NJS_MODULE_END));

    return NXT_OK;

fail:

    if (text->start != NULL) {
        nxt_mp_free(vm->mem_pool, text->start);
    }

    return NXT_ERROR;
}