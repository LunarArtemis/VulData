static void *set_eth_seg(struct mlx5_wqe_eth_seg *eseg,
			 const struct ib_send_wr *wr, void *qend,
			 struct mlx5_ib_qp *qp, int *size)
{
	void *seg = eseg;

	memset(eseg, 0, sizeof(struct mlx5_wqe_eth_seg));

	if (wr->send_flags & IB_SEND_IP_CSUM)
		eseg->cs_flags = MLX5_ETH_WQE_L3_CSUM |
				 MLX5_ETH_WQE_L4_CSUM;

	seg += sizeof(struct mlx5_wqe_eth_seg);
	*size += sizeof(struct mlx5_wqe_eth_seg) / 16;

	if (wr->opcode == IB_WR_LSO) {
		struct ib_ud_wr *ud_wr = container_of(wr, struct ib_ud_wr, wr);
		int size_of_inl_hdr_start = sizeof(eseg->inline_hdr.start);
		u64 left, leftlen, copysz;
		void *pdata = ud_wr->header;

		left = ud_wr->hlen;
		eseg->mss = cpu_to_be16(ud_wr->mss);
		eseg->inline_hdr.sz = cpu_to_be16(left);

		/*
		 * check if there is space till the end of queue, if yes,
		 * copy all in one shot, otherwise copy till the end of queue,
		 * rollback and than the copy the left
		 */
		leftlen = qend - (void *)eseg->inline_hdr.start;
		copysz = min_t(u64, leftlen, left);

		memcpy(seg - size_of_inl_hdr_start, pdata, copysz);

		if (likely(copysz > size_of_inl_hdr_start)) {
			seg += ALIGN(copysz - size_of_inl_hdr_start, 16);
			*size += ALIGN(copysz - size_of_inl_hdr_start, 16) / 16;
		}

		if (unlikely(copysz < left)) { /* the last wqe in the queue */
			seg = mlx5_get_send_wqe(qp, 0);
			left -= copysz;
			pdata += copysz;
			memcpy(seg, pdata, left);
			seg += ALIGN(left, 16);
			*size += ALIGN(left, 16) / 16;
		}
	}

	return seg;
}