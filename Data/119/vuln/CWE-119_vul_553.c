size_t SkRegion::readFromMemory(const void* storage, size_t length) {
    SkRBufferWithSizeCheck  buffer(storage, length);
    SkRegion                tmp;
    int32_t                 count;

    if (buffer.readS32(&count) && (count >= 0) && buffer.read(&tmp.fBounds, sizeof(tmp.fBounds))) {
        if (count == 0) {
            tmp.fRunHead = SkRegion_gRectRunHeadPtr;
        } else {
            int32_t ySpanCount, intervalCount;
            if (buffer.readS32(&ySpanCount) && buffer.readS32(&intervalCount)) {
                tmp.allocateRuns(count, ySpanCount, intervalCount);
                buffer.read(tmp.fRunHead->writable_runs(), count * sizeof(RunType));
            }
        }
    }
    size_t sizeRead = 0;
    if (buffer.isValid()) {
        this->swap(tmp);
        sizeRead = buffer.pos();
    }
    return sizeRead;
}