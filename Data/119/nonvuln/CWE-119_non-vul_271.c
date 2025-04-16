static int mlx4_set_port_mac_table(struct mlx4_dev *dev, u8 port,
				   __be64 *entries)
{
	struct mlx4_cmd_mailbox *mailbox;
	u32 in_mod;
	int err;

	mailbox = mlx4_alloc_cmd_mailbox(dev);
	if (IS_ERR(mailbox))
		return PTR_ERR(mailbox);

	memcpy(mailbox->buf, entries, MLX4_MAC_TABLE_SIZE);

	in_mod = MLX4_SET_PORT_MAC_TABLE << 8 | port;
	err = mlx4_cmd(dev, mailbox->dma, in_mod, 1, MLX4_CMD_SET_PORT,
		       MLX4_CMD_TIME_CLASS_B);

	mlx4_free_cmd_mailbox(dev, mailbox);
	return err;
}