AsyncReadPixelsCompletedQuery::AsyncReadPixelsCompletedQuery(
    QueryManager* manager, GLenum target, int32 shm_id, uint32 shm_offset)
    : Query(manager, target, shm_id, shm_offset) {
}