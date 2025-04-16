FixedArrayBase* Heap::LeftTrimFixedArray(FixedArrayBase* object,
                                         int elements_to_trim) {
  DCHECK(!object->IsFixedTypedArrayBase());
  DCHECK(!object->IsByteArray());
  const int element_size = object->IsFixedArray() ? kPointerSize : kDoubleSize;
  const int bytes_to_trim = elements_to_trim * element_size;
  Map* map = object->map();

  // For now this trick is only applied to objects in new and paged space.
  // In large object space the object's start must coincide with chunk
  // and thus the trick is just not applicable.
  DCHECK(!lo_space()->Contains(object));
  DCHECK(object->map() != fixed_cow_array_map());

  // Ensure that the no handle-scope has more than one pointer to the same
  // backing-store.
  SLOW_DCHECK(CountHandlesForObject(object) <= 1);

  STATIC_ASSERT(FixedArrayBase::kMapOffset == 0);
  STATIC_ASSERT(FixedArrayBase::kLengthOffset == kPointerSize);
  STATIC_ASSERT(FixedArrayBase::kHeaderSize == 2 * kPointerSize);

  const int len = object->length();
  DCHECK(elements_to_trim <= len);

  // Calculate location of new array start.
  Address new_start = object->address() + bytes_to_trim;

  // Technically in new space this write might be omitted (except for
  // debug mode which iterates through the heap), but to play safer
  // we still do it.
  CreateFillerObjectAt(object->address(), bytes_to_trim,
                       ClearRecordedSlots::kYes);
  // Initialize header of the trimmed array. Since left trimming is only
  // performed on pages which are not concurrently swept creating a filler
  // object does not require synchronization.
  DCHECK(CanMoveObjectStart(object));
  Object** former_start = HeapObject::RawField(object, 0);
  int new_start_index = elements_to_trim * (element_size / kPointerSize);
  former_start[new_start_index] = map;
  former_start[new_start_index + 1] = Smi::FromInt(len - elements_to_trim);
  FixedArrayBase* new_object =
      FixedArrayBase::cast(HeapObject::FromAddress(new_start));

  // Remove recorded slots for the new map and length offset.
  ClearRecordedSlot(new_object, HeapObject::RawField(new_object, 0));
  ClearRecordedSlot(new_object, HeapObject::RawField(
                                    new_object, FixedArrayBase::kLengthOffset));

  // Maintain consistency of live bytes during incremental marking
  Marking::TransferMark(this, object->address(), new_start);
  AdjustLiveBytes(new_object, -bytes_to_trim, Heap::CONCURRENT_TO_SWEEPER);

  // Notify the heap profiler of change in object layout.
  OnMoveEvent(new_object, object, new_object->Size());
  return new_object;
}