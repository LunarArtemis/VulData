static void destroy_flow_rule_vport_sq(struct mlx5_ib_dev *dev,
				       struct mlx5_ib_sq *sq)
{
	if (sq->flow_rule)
		mlx5_del_flow_rules(sq->flow_rule);
}