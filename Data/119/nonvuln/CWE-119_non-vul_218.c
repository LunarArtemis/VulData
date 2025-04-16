static void set_reg_data_seg(struct mlx5_wqe_data_seg *dseg,
			     struct mlx5_ib_mr *mr,
			     struct mlx5_ib_pd *pd)
{
	int bcount = mr->desc_size * mr->ndescs;

	dseg->addr = cpu_to_be64(mr->desc_map);
	dseg->byte_count = cpu_to_be32(ALIGN(bcount, 64));
	dseg->lkey = cpu_to_be32(pd->ibpd.local_dma_lkey);
}