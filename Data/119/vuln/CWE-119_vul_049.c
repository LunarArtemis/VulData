static Status
_GetCountedString(char **wire_inout, ClientPtr client, char **str)
{
    char *wire, *next;
    CARD16 len;

    wire = *wire_inout;
    len = *(CARD16 *) wire;
    if (client->swapped) {
        swaps(&len);
    }
    next = wire + XkbPaddedSize(len + 2);
    /* Check we're still within the size of the request */
    if (client->req_len <
        bytes_to_int32(next - (char *) client->requestBuffer))
        return BadValue;
    *str = malloc(len + 1);
    if (!*str)
        return BadAlloc;
    memcpy(*str, &wire[2], len);
    *(*str + len) = '\0';
    *wire_inout = next;
    return Success;
}