static bool tunnel_offload_supported(struct mlx5_core_dev *dev)
{
	return  (MLX5_CAP_ETH(dev, tunnel_stateless_vxlan) ||
		 MLX5_CAP_ETH(dev, tunnel_stateless_gre) ||
		 MLX5_CAP_ETH(dev, tunnel_stateless_geneve_rx));
}