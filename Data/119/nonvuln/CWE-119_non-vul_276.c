void mlx4_unregister_mac(struct mlx4_dev *dev, u8 port, int index)
{
	struct mlx4_mac_table *table = &mlx4_priv(dev)->port[port].mac_table;

	mutex_lock(&table->mutex);
	if (!table->refs[index]) {
		mlx4_warn(dev, "No MAC entry for index %d\n", index);
		goto out;
	}
	if (--table->refs[index]) {
		mlx4_warn(dev, "Have more references for index %d,"
			  "no need to modify MAC table\n", index);
		goto out;
	}
	table->entries[index] = 0;
	mlx4_set_port_mac_table(dev, port, table->entries);
	--table->total;
out:
	mutex_unlock(&table->mutex);
}