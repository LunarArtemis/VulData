static void set_reg_umr_inline_seg(void *seg, struct mlx5_ib_qp *qp,
				   struct mlx5_ib_mr *mr, int mr_list_size)
{
	void *qend = qp->sq.qend;
	void *addr = mr->descs;
	int copy;

	if (unlikely(seg + mr_list_size > qend)) {
		copy = qend - seg;
		memcpy(seg, addr, copy);
		addr += copy;
		mr_list_size -= copy;
		seg = mlx5_get_send_wqe(qp, 0);
	}
	memcpy(seg, addr, mr_list_size);
	seg += mr_list_size;
}