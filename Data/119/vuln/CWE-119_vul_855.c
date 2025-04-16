static bool objc_build_refs(RCoreObjc *objc) {
	ut64 off;
	if (!objc->_const || !objc->_selrefs) {
		return false;
	}

	const ut64 va_const = objc->_const->vaddr;
	size_t ss_const = objc->_const->vsize;
	const ut64 va_selrefs = objc->_selrefs->vaddr;
	size_t ss_selrefs = objc->_selrefs->vsize;

	// TODO: check if ss_const or ss_selrefs are too big before going further
	size_t maxsize = R_MAX (ss_const, ss_selrefs);
	maxsize = R_MIN (maxsize, objc->file_size);

	ut8 *buf = calloc (1, maxsize);
	if (!buf) {
		return false;
	}
	const size_t word_size = objc->word_size; // assuming 8 because of the read_le64
	if (!r_io_read_at (objc->core->io, objc->_const->vaddr, buf, ss_const)) {
		eprintf ("aao: Cannot read the whole const section %zu\n", ss_const);
		return false;
	}
	for (off = 0; off + word_size < ss_const && off + word_size < maxsize; off += word_size) {
		ut64 va = va_const + off;
		ut64 xrefs_to = r_read_le64 (buf + off);
		if (isValid (xrefs_to)) {
			array_add (objc, va, xrefs_to);
		}
	}
	if (!r_io_read_at (objc->core->io, va_selrefs, buf, ss_selrefs)) {
		eprintf ("aao: Cannot read the whole selrefs section\n");
		return false;
	}
	for (off = 0; off + word_size < ss_selrefs && off + word_size < maxsize; off += word_size) {
		ut64 va = va_selrefs + off;
		ut64 xrefs_to = r_read_le64 (buf + off);
		if (isValid (xrefs_to)) {
			array_add (objc, xrefs_to, va);
		}
	}
	free (buf);
	return true;
}