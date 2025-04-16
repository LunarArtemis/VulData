extern PyArrayObject *
array_from_pyobj(const int type_num, npy_intp *dims, const int rank,
                 const int intent, PyObject *obj)
{
    /*
     * Note about reference counting
     *  -----------------------------
     * If the caller returns the array to Python, it must be done with
     * Py_BuildValue("N",arr).
     * Otherwise, if obj!=arr then the caller must call Py_DECREF(arr).
     *
     * Note on intent(cache,out,..)
     * ---------------------
     * Don't expect correct data when returning intent(cache) array.
     *
     */
    char mess[200];
    PyArrayObject *arr = NULL;
    PyArray_Descr *descr;
    char typechar;
    int elsize;

    if ((intent & F2PY_INTENT_HIDE) ||
        ((intent & F2PY_INTENT_CACHE) && (obj == Py_None)) ||
        ((intent & F2PY_OPTIONAL) && (obj == Py_None))) {
        /* intent(cache), optional, intent(hide) */
        if (count_negative_dimensions(rank, dims) > 0) {
            int i;
            strcpy(mess,
                   "failed to create intent(cache|hide)|optional array"
                   "-- must have defined dimensions but got (");
            for (i = 0; i < rank; ++i)
                sprintf(mess + strlen(mess), "%" NPY_INTP_FMT ",", dims[i]);
            strcat(mess, ")");
            PyErr_SetString(PyExc_ValueError, mess);
            return NULL;
        }
        arr = (PyArrayObject *)PyArray_New(&PyArray_Type, rank, dims, type_num,
                                           NULL, NULL, 1,
                                           !(intent & F2PY_INTENT_C), NULL);
        if (arr == NULL)
            return NULL;
        if (!(intent & F2PY_INTENT_CACHE))
            PyArray_FILLWBYTE(arr, 0);
        return arr;
    }

    descr = PyArray_DescrFromType(type_num);
    /* compatibility with NPY_CHAR */
    if (type_num == NPY_STRING) {
        PyArray_DESCR_REPLACE(descr);
        if (descr == NULL) {
            return NULL;
        }
        descr->elsize = 1;
        descr->type = NPY_CHARLTR;
    }
    elsize = descr->elsize;
    typechar = descr->type;
    Py_DECREF(descr);
    if (PyArray_Check(obj)) {
        arr = (PyArrayObject *)obj;

        if (intent & F2PY_INTENT_CACHE) {
            /* intent(cache) */
            if (PyArray_ISONESEGMENT(arr) && PyArray_ITEMSIZE(arr) >= elsize) {
                if (check_and_fix_dimensions(arr, rank, dims)) {
                    return NULL;
                }
                if (intent & F2PY_INTENT_OUT)
                    Py_INCREF(arr);
                return arr;
            }
            strcpy(mess, "failed to initialize intent(cache) array");
            if (!PyArray_ISONESEGMENT(arr))
                strcat(mess, " -- input must be in one segment");
            if (PyArray_ITEMSIZE(arr) < elsize)
                sprintf(mess + strlen(mess),
                        " -- expected at least elsize=%d but got "
                        "%" NPY_INTP_FMT,
                        elsize, (npy_intp)PyArray_ITEMSIZE(arr));
            PyErr_SetString(PyExc_ValueError, mess);
            return NULL;
        }

        /* here we have always intent(in) or intent(inout) or intent(inplace)
         */

        if (check_and_fix_dimensions(arr, rank, dims)) {
            return NULL;
        }
        /*
        printf("intent alignment=%d\n", F2PY_GET_ALIGNMENT(intent));
        printf("alignment check=%d\n", F2PY_CHECK_ALIGNMENT(arr, intent));
        int i;
        for (i=1;i<=16;i++)
          printf("i=%d isaligned=%d\n", i, ARRAY_ISALIGNED(arr, i));
        */
        if ((!(intent & F2PY_INTENT_COPY)) &&
            PyArray_ITEMSIZE(arr) == elsize &&
            ARRAY_ISCOMPATIBLE(arr, type_num) &&
            F2PY_CHECK_ALIGNMENT(arr, intent)) {
            if ((intent & F2PY_INTENT_C) ? PyArray_ISCARRAY_RO(arr)
                                         : PyArray_ISFARRAY_RO(arr)) {
                if ((intent & F2PY_INTENT_OUT)) {
                    Py_INCREF(arr);
                }
                /* Returning input array */
                return arr;
            }
        }
        if (intent & F2PY_INTENT_INOUT) {
            strcpy(mess, "failed to initialize intent(inout) array");
            /* Must use PyArray_IS*ARRAY because intent(inout) requires
             * writable input */
            if ((intent & F2PY_INTENT_C) && !PyArray_ISCARRAY(arr))
                strcat(mess, " -- input not contiguous");
            if (!(intent & F2PY_INTENT_C) && !PyArray_ISFARRAY(arr))
                strcat(mess, " -- input not fortran contiguous");
            if (PyArray_ITEMSIZE(arr) != elsize)
                sprintf(mess + strlen(mess),
                        " -- expected elsize=%d but got %" NPY_INTP_FMT,
                        elsize, (npy_intp)PyArray_ITEMSIZE(arr));
            if (!(ARRAY_ISCOMPATIBLE(arr, type_num)))
                sprintf(mess + strlen(mess),
                        " -- input '%c' not compatible to '%c'",
                        PyArray_DESCR(arr)->type, typechar);
            if (!(F2PY_CHECK_ALIGNMENT(arr, intent)))
                sprintf(mess + strlen(mess), " -- input not %d-aligned",
                        F2PY_GET_ALIGNMENT(intent));
            PyErr_SetString(PyExc_ValueError, mess);
            return NULL;
        }

        /* here we have always intent(in) or intent(inplace) */

        {
            PyArrayObject *retarr;
            retarr = (PyArrayObject *)PyArray_New(
                    &PyArray_Type, PyArray_NDIM(arr), PyArray_DIMS(arr),
                    type_num, NULL, NULL, 1, !(intent & F2PY_INTENT_C), NULL);
            if (retarr == NULL)
                return NULL;
            F2PY_REPORT_ON_ARRAY_COPY_FROMARR;
            if (PyArray_CopyInto(retarr, arr)) {
                Py_DECREF(retarr);
                return NULL;
            }
            if (intent & F2PY_INTENT_INPLACE) {
                if (swap_arrays(arr, retarr))
                    return NULL; /* XXX: set exception */
                Py_XDECREF(retarr);
                if (intent & F2PY_INTENT_OUT)
                    Py_INCREF(arr);
            }
            else {
                arr = retarr;
            }
        }
        return arr;
    }

    if ((intent & F2PY_INTENT_INOUT) || (intent & F2PY_INTENT_INPLACE) ||
        (intent & F2PY_INTENT_CACHE)) {
        PyErr_Format(PyExc_TypeError,
                     "failed to initialize intent(inout|inplace|cache) "
                     "array, input '%s' object is not an array",
                     Py_TYPE(obj)->tp_name);
        return NULL;
    }

    {
        PyArray_Descr *descr = PyArray_DescrFromType(type_num);
        /* compatibility with NPY_CHAR */
        if (type_num == NPY_STRING) {
            PyArray_DESCR_REPLACE(descr);
            if (descr == NULL) {
                return NULL;
            }
            descr->elsize = 1;
            descr->type = NPY_CHARLTR;
        }
        F2PY_REPORT_ON_ARRAY_COPY_FROMANY;
        arr = (PyArrayObject *)PyArray_FromAny(
                obj, descr, 0, 0,
                ((intent & F2PY_INTENT_C) ? NPY_ARRAY_CARRAY
                                          : NPY_ARRAY_FARRAY) |
                        NPY_ARRAY_FORCECAST,
                NULL);
        if (arr == NULL)
            return NULL;
        if (check_and_fix_dimensions(arr, rank, dims)) {
            return NULL;
        }
        return arr;
    }
}