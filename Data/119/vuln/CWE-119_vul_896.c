void MarkObjectByPointer(Object** p) {
    if (!(*p)->IsHeapObject()) return;

    HeapObject* object = HeapObject::cast(*p);

    if (collector_->heap()->PurgeLeftTrimmedObject(p)) return;

    MarkBit mark_bit = Marking::MarkBitFrom(object);
    if (Marking::IsBlackOrGrey(mark_bit)) return;

    Map* map = object->map();
    // Mark the object.
    collector_->SetMark(object, mark_bit);

    // Mark the map pointer and body, and push them on the marking stack.
    MarkBit map_mark = Marking::MarkBitFrom(map);
    collector_->MarkObject(map, map_mark);
    MarkCompactMarkingVisitor::IterateBody(map, object);

    // Mark all the objects reachable from the map and body.  May leave
    // overflowed objects in the heap.
    collector_->EmptyMarkingDeque();
  }