static int _yr_compiler_set_namespace(
    YR_COMPILER* compiler,
    const char* namespace_)
{
  YR_NAMESPACE* ns = (YR_NAMESPACE*) yr_arena_get_ptr(
      compiler->arena, YR_NAMESPACES_TABLE, 0);

  bool found = false;

  for (int i = 0; i < compiler->num_namespaces; i++, ns++)
  {
    if (strcmp(ns->name, namespace_) == 0)
    {
      found = true;
      compiler->current_namespace_idx = i;
      break;
    }
  }

  if (!found)
  {
    YR_ARENA_REF ref;

    FAIL_ON_ERROR(yr_arena_allocate_struct(
        compiler->arena,
        YR_NAMESPACES_TABLE,
        sizeof(YR_NAMESPACE),
        &ref,
        offsetof(YR_NAMESPACE, name),
        EOL));

    ns = (YR_NAMESPACE*) yr_arena_ref_to_ptr(compiler->arena, &ref);

    FAIL_ON_ERROR(_yr_compiler_store_string(compiler, namespace_, &ref));

    ns->name = (const char*) yr_arena_ref_to_ptr(compiler->arena, &ref);
    ns->idx = compiler->num_namespaces;

    compiler->current_namespace_idx = compiler->num_namespaces;
    compiler->num_namespaces++;
  }

  return ERROR_SUCCESS;
}