static int mlx5_ib_umem_get(struct mlx5_ib_dev *dev,
			    struct ib_pd *pd,
			    unsigned long addr, size_t size,
			    struct ib_umem **umem,
			    int *npages, int *page_shift, int *ncont,
			    u32 *offset)
{
	int err;

	*umem = ib_umem_get(pd->uobject->context, addr, size, 0, 0);
	if (IS_ERR(*umem)) {
		mlx5_ib_dbg(dev, "umem_get failed\n");
		return PTR_ERR(*umem);
	}

	mlx5_ib_cont_pages(*umem, addr, 0, npages, page_shift, ncont, NULL);

	err = mlx5_ib_get_buf_offset(addr, *page_shift, offset);
	if (err) {
		mlx5_ib_warn(dev, "bad offset\n");
		goto err_umem;
	}

	mlx5_ib_dbg(dev, "addr 0x%lx, size %zu, npages %d, page_shift %d, ncont %d, offset %d\n",
		    addr, size, *npages, *page_shift, *ncont, *offset);

	return 0;

err_umem:
	ib_umem_release(*umem);
	*umem = NULL;

	return err;
}