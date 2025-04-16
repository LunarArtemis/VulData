inline void StoreBacking(rgba_t *backing, const rgba_t *top,
                         const rgba_t *bottom) {
    if (N == 1) {
        backing[0] = *top;
        backing[1] = *bottom;
    }
    else {
        backing[0] = top[0];
        backing[1] = top[1];
        backing[2] = bottom[0];
        backing[3] = bottom[1];
    }
}