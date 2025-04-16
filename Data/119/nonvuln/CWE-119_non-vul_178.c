static void destroy_user_rq(struct mlx5_ib_dev *dev, struct ib_pd *pd,
			    struct mlx5_ib_rwq *rwq)
{
	struct mlx5_ib_ucontext *context;

	if (rwq->create_flags & MLX5_IB_WQ_FLAGS_DELAY_DROP)
		atomic_dec(&dev->delay_drop.rqs_cnt);

	context = to_mucontext(pd->uobject->context);
	mlx5_ib_db_unmap_user(context, &rwq->db);
	if (rwq->umem)
		ib_umem_release(rwq->umem);
}