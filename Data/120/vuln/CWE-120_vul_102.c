void context_model_table::decouple_or_alloc_with_empty_data()
{
  if (refcnt && *refcnt==1) { return; }

  if (refcnt) {
    assert(*refcnt>1);
    (*refcnt)--;
  }

  if (D) printf("%p (alloc)\n",this);

  model = new context_model[CONTEXT_MODEL_TABLE_LENGTH];
  refcnt= new int;
  *refcnt=1;
}