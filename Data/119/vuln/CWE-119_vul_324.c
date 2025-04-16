bool AsyncReadPixelsCompletedQuery::End(base::subtle::Atomic32 submit_count) {
  if (!AddToPendingQueue(submit_count)) {
    return false;
  }
  manager()->decoder()->WaitForReadPixels(
      base::Bind(&AsyncReadPixelsCompletedQuery::Complete,
                 AsWeakPtr()));

  return true;
}