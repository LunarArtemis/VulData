ExponentialFunction::ExponentialFunction(Object *funcObj, Dict *dict) {
  Object obj1, obj2;
  int i;

  ok = gFalse;

  //----- initialize the generic stuff
  if (!init(dict)) {
    goto err1;
  }
  if (m != 1) {
    error(errSyntaxError, -1, "Exponential function with more than one input");
    goto err1;
  }

  //----- C0
  if (dict->lookup("C0", &obj1)->isArray()) {
    if (hasRange && obj1.arrayGetLength() != n) {
      error(errSyntaxError, -1, "Function's C0 array is wrong length");
      goto err2;
    }
    n = obj1.arrayGetLength();
    for (i = 0; i < n; ++i) {
      obj1.arrayGet(i, &obj2);
      if (!obj2.isNum()) {
	error(errSyntaxError, -1, "Illegal value in function C0 array");
	goto err3;
      }
      c0[i] = obj2.getNum();
      obj2.free();
    }
  } else {
    if (hasRange && n != 1) {
      error(errSyntaxError, -1, "Function's C0 array is wrong length");
      goto err2;
    }
    n = 1;
    c0[0] = 0;
  }
  obj1.free();

  //----- C1
  if (dict->lookup("C1", &obj1)->isArray()) {
    if (obj1.arrayGetLength() != n) {
      error(errSyntaxError, -1, "Function's C1 array is wrong length");
      goto err2;
    }
    for (i = 0; i < n; ++i) {
      obj1.arrayGet(i, &obj2);
      if (!obj2.isNum()) {
	error(errSyntaxError, -1, "Illegal value in function C1 array");
	goto err3;
      }
      c1[i] = obj2.getNum();
      obj2.free();
    }
  } else {
    if (n != 1) {
      error(errSyntaxError, -1, "Function's C1 array is wrong length");
      goto err2;
    }
    c1[0] = 1;
  }
  obj1.free();

  //----- N (exponent)
  if (!dict->lookup("N", &obj1)->isNum()) {
    error(errSyntaxError, -1, "Function has missing or invalid N");
    goto err2;
  }
  e = obj1.getNum();
  obj1.free();

  isLinear = fabs(e-1.) < 1e-10;
  ok = gTrue;
  return;

 err3:
  obj2.free();
 err2:
  obj1.free();
 err1:
  return;
}