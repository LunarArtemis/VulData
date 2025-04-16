static pointer oblist_initial_value(scheme *sc)
{
  return mk_vector(sc, 461); /* probably should be bigger */
}