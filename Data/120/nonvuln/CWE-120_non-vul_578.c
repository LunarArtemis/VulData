inline bool EqualToBacking(const rgba_t *top, const rgba_t *bottom,
                           const rgba_t *backing) {
    if (N == 1) return *top == backing[0] && *bottom == backing[1];
    return *top == backing[0] && *(top + 1) == backing[1] &&
           *bottom == backing[2] && *(bottom + 1) == backing[3];
}