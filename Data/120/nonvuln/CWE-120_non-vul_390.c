static INLINE void new_slot_spec_in_env(scheme *sc, pointer env,
                                        pointer variable, pointer value)
{
  pointer slot = immutable_cons(sc, variable, value);

  if (is_vector(car(env))) {
    int location = hash_fn(symname(variable), ivalue_unchecked(car(env)));

    set_vector_elem(car(env), location,
                    immutable_cons(sc, slot, vector_elem(car(env), location)));
  } else {
    car(env) = immutable_cons(sc, slot, car(env));
  }
}