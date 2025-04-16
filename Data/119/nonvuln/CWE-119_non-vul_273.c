int mlx4_SET_PORT(struct mlx4_dev *dev, u8 port)
{
	struct mlx4_cmd_mailbox *mailbox;
	int err;

	if (dev->caps.port_type[port] == MLX4_PORT_TYPE_ETH)
		return 0;

	mailbox = mlx4_alloc_cmd_mailbox(dev);
	if (IS_ERR(mailbox))
		return PTR_ERR(mailbox);

	memset(mailbox->buf, 0, 256);

	((__be32 *) mailbox->buf)[1] = dev->caps.ib_port_def_cap[port];
	err = mlx4_cmd(dev, mailbox->dma, port, 0, MLX4_CMD_SET_PORT,
		       MLX4_CMD_TIME_CLASS_B);

	mlx4_free_cmd_mailbox(dev, mailbox);
	return err;
}