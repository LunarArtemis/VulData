static void destroy_qp_kernel(struct mlx5_ib_dev *dev, struct mlx5_ib_qp *qp)
{
	kvfree(qp->sq.wqe_head);
	kvfree(qp->sq.w_list);
	kvfree(qp->sq.wrid);
	kvfree(qp->sq.wr_data);
	kvfree(qp->rq.wrid);
	mlx5_db_free(dev->mdev, &qp->db);
	mlx5_buf_free(dev->mdev, &qp->buf);
}