int mlx4_get_port_ib_caps(struct mlx4_dev *dev, u8 port, __be32 *caps)
{
	struct mlx4_cmd_mailbox *inmailbox, *outmailbox;
	u8 *inbuf, *outbuf;
	int err;

	inmailbox = mlx4_alloc_cmd_mailbox(dev);
	if (IS_ERR(inmailbox))
		return PTR_ERR(inmailbox);

	outmailbox = mlx4_alloc_cmd_mailbox(dev);
	if (IS_ERR(outmailbox)) {
		mlx4_free_cmd_mailbox(dev, inmailbox);
		return PTR_ERR(outmailbox);
	}

	inbuf = inmailbox->buf;
	outbuf = outmailbox->buf;
	memset(inbuf, 0, 256);
	memset(outbuf, 0, 256);
	inbuf[0] = 1;
	inbuf[1] = 1;
	inbuf[2] = 1;
	inbuf[3] = 1;
	*(__be16 *) (&inbuf[16]) = cpu_to_be16(0x0015);
	*(__be32 *) (&inbuf[20]) = cpu_to_be32(port);

	err = mlx4_cmd_box(dev, inmailbox->dma, outmailbox->dma, port, 3,
			   MLX4_CMD_MAD_IFC, MLX4_CMD_TIME_CLASS_C);
	if (!err)
		*caps = *(__be32 *) (outbuf + 84);
	mlx4_free_cmd_mailbox(dev, inmailbox);
	mlx4_free_cmd_mailbox(dev, outmailbox);
	return err;
}