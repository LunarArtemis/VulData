NPY_NO_EXPORT PyObject *
PyArray_NewFromDescr_int(
        PyTypeObject *subtype, PyArray_Descr *descr, int nd,
        npy_intp const *dims, npy_intp const *strides, void *data,
        int flags, PyObject *obj, PyObject *base, int zeroed,
        int allow_emptystring)
{
    PyArrayObject_fields *fa;
    int i;
    npy_intp nbytes;

    if (descr->subarray) {
        PyObject *ret;
        npy_intp newdims[2*NPY_MAXDIMS];
        npy_intp *newstrides = NULL;
        memcpy(newdims, dims, nd*sizeof(npy_intp));
        if (strides) {
            newstrides = newdims + NPY_MAXDIMS;
            memcpy(newstrides, strides, nd*sizeof(npy_intp));
        }
        nd =_update_descr_and_dimensions(&descr, newdims,
                                         newstrides, nd);
        ret = PyArray_NewFromDescr_int(
                subtype, descr,
                nd, newdims, newstrides, data,
                flags, obj, base,
                zeroed, allow_emptystring);
        return ret;
    }

    if ((unsigned int)nd > (unsigned int)NPY_MAXDIMS) {
        PyErr_Format(PyExc_ValueError,
                     "number of dimensions must be within [0, %d]",
                     NPY_MAXDIMS);
        Py_DECREF(descr);
        return NULL;
    }

    /* Check datatype element size */
    nbytes = descr->elsize;
    if (PyDataType_ISUNSIZED(descr)) {
        if (!PyDataType_ISFLEXIBLE(descr)) {
            PyErr_SetString(PyExc_TypeError, "Empty data-type");
            Py_DECREF(descr);
            return NULL;
        }
        else if (PyDataType_ISSTRING(descr) && !allow_emptystring &&
                 data == NULL) {
            PyArray_DESCR_REPLACE(descr);
            if (descr == NULL) {
                return NULL;
            }
            if (descr->type_num == NPY_STRING) {
                nbytes = descr->elsize = 1;
            }
            else {
                nbytes = descr->elsize = sizeof(npy_ucs4);
            }
        }
    }

    /* Check dimensions and multiply them to nbytes */
    for (i = 0; i < nd; i++) {
        npy_intp dim = dims[i];

        if (dim == 0) {
            /*
             * Compare to PyArray_OverflowMultiplyList that
             * returns 0 in this case.
             */
            continue;
        }

        if (dim < 0) {
            PyErr_SetString(PyExc_ValueError,
                "negative dimensions are not allowed");
            Py_DECREF(descr);
            return NULL;
        }

        /*
         * Care needs to be taken to avoid integer overflow when
         * multiplying the dimensions together to get the total size of the
         * array.
         */
        if (npy_mul_with_overflow_intp(&nbytes, nbytes, dim)) {
            PyErr_SetString(PyExc_ValueError,
                "array is too big; `arr.size * arr.dtype.itemsize` "
                "is larger than the maximum possible size.");
            Py_DECREF(descr);
            return NULL;
        }
    }

    fa = (PyArrayObject_fields *) subtype->tp_alloc(subtype, 0);
    if (fa == NULL) {
        Py_DECREF(descr);
        return NULL;
    }
    fa->_buffer_info = NULL;
    fa->nd = nd;
    fa->dimensions = NULL;
    fa->data = NULL;

    if (data == NULL) {
        fa->flags = NPY_ARRAY_DEFAULT;
        if (flags) {
            fa->flags |= NPY_ARRAY_F_CONTIGUOUS;
            if (nd > 1) {
                fa->flags &= ~NPY_ARRAY_C_CONTIGUOUS;
            }
            flags = NPY_ARRAY_F_CONTIGUOUS;
        }
    }
    else {
        fa->flags = (flags & ~NPY_ARRAY_WRITEBACKIFCOPY);
        fa->flags &= ~NPY_ARRAY_UPDATEIFCOPY;
    }
    fa->descr = descr;
    fa->base = (PyObject *)NULL;
    fa->weakreflist = (PyObject *)NULL;

    if (nd > 0) {
        fa->dimensions = npy_alloc_cache_dim(2 * nd);
        if (fa->dimensions == NULL) {
            PyErr_NoMemory();
            goto fail;
        }
        fa->strides = fa->dimensions + nd;
        if (nd) {
            memcpy(fa->dimensions, dims, sizeof(npy_intp)*nd);
        }
        if (strides == NULL) {  /* fill it in */
            _array_fill_strides(fa->strides, dims, nd, descr->elsize,
                                flags, &(fa->flags));
        }
        else {
            /*
             * we allow strides even when we create
             * the memory, but be careful with this...
             */
            if (nd) {
                memcpy(fa->strides, strides, sizeof(npy_intp)*nd);
            }
        }
    }
    else {
        fa->dimensions = fa->strides = NULL;
        fa->flags |= NPY_ARRAY_F_CONTIGUOUS;
    }

    if (data == NULL) {
        /*
         * Allocate something even for zero-space arrays
         * e.g. shape=(0,) -- otherwise buffer exposure
         * (a.data) doesn't work as it should.
         * Could probably just allocate a few bytes here. -- Chuck
         */
        if (nbytes == 0) {
            nbytes = descr->elsize ? descr->elsize : 1;
        }
        /*
         * It is bad to have uninitialized OBJECT pointers
         * which could also be sub-fields of a VOID array
         */
        if (zeroed || PyDataType_FLAGCHK(descr, NPY_NEEDS_INIT)) {
            data = npy_alloc_cache_zero(nbytes);
        }
        else {
            data = npy_alloc_cache(nbytes);
        }
        if (data == NULL) {
            raise_memory_error(fa->nd, fa->dimensions, descr);
            goto fail;
        }
        fa->flags |= NPY_ARRAY_OWNDATA;
    }
    else {
        /*
         * If data is passed in, this object won't own it by default.
         * Caller must arrange for this to be reset if truly desired
         */
        fa->flags &= ~NPY_ARRAY_OWNDATA;
    }
    fa->data = data;

    /*
     * always update the flags to get the right CONTIGUOUS, ALIGN properties
     * not owned data and input strides may not be aligned and on some
     * platforms (debian sparc) malloc does not provide enough alignment for
     * long double types
     */
    PyArray_UpdateFlags((PyArrayObject *)fa, NPY_ARRAY_UPDATE_ALL);

    /* Set the base object. It's important to do it here so that
     * __array_finalize__ below receives it
     */
    if (base != NULL) {
        Py_INCREF(base);
        if (PyArray_SetBaseObject((PyArrayObject *)fa, base) < 0) {
            goto fail;
        }
    }

    /*
     * call the __array_finalize__
     * method if a subtype.
     * If obj is NULL, then call method with Py_None
     */
    if ((subtype != &PyArray_Type)) {
        PyObject *res, *func, *args;

        func = PyObject_GetAttr((PyObject *)fa, npy_ma_str_array_finalize);
        if (func && func != Py_None) {
            if (PyCapsule_CheckExact(func)) {
                /* A C-function is stored here */
                PyArray_FinalizeFunc *cfunc;
                cfunc = PyCapsule_GetPointer(func, NULL);
                Py_DECREF(func);
                if (cfunc == NULL) {
                    goto fail;
                }
                if (cfunc((PyArrayObject *)fa, obj) < 0) {
                    goto fail;
                }
            }
            else {
                args = PyTuple_New(1);
                if (obj == NULL) {
                    obj=Py_None;
                }
                Py_INCREF(obj);
                PyTuple_SET_ITEM(args, 0, obj);
                res = PyObject_Call(func, args, NULL);
                Py_DECREF(args);
                Py_DECREF(func);
                if (res == NULL) {
                    goto fail;
                }
                else {
                    Py_DECREF(res);
                }
            }
        }
        else Py_XDECREF(func);
    }
    return (PyObject *)fa;

 fail:
    Py_DECREF(fa);
    return NULL;
}