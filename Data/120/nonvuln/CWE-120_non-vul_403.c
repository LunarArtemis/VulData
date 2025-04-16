static void new_frame_in_env(scheme *sc, pointer old_env)
{
  pointer new_frame;

  /* The interaction-environment has about 300 variables in it. */
  if (old_env == sc->NIL) {
    new_frame = mk_vector(sc, 461);
  } else {
    new_frame = sc->NIL;
  }

  sc->envir = immutable_cons(sc, new_frame, old_env);
  setenvironment(sc->envir);
}