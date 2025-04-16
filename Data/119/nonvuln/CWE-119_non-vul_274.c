void mlx4_unregister_vlan(struct mlx4_dev *dev, u8 port, int index)
{
	struct mlx4_vlan_table *table = &mlx4_priv(dev)->port[port].vlan_table;

	if (index < MLX4_VLAN_REGULAR) {
		mlx4_warn(dev, "Trying to free special vlan index %d\n", index);
		return;
	}

	mutex_lock(&table->mutex);
	if (!table->refs[index]) {
		mlx4_warn(dev, "No vlan entry for index %d\n", index);
		goto out;
	}
	if (--table->refs[index]) {
		mlx4_dbg(dev, "Have more references for index %d,"
			 "no need to modify vlan table\n", index);
		goto out;
	}
	table->entries[index] = 0;
	mlx4_set_port_vlan_table(dev, port, table->entries);
	--table->total;
out:
	mutex_unlock(&table->mutex);
}