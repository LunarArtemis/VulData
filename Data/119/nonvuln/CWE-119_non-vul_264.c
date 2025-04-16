static void netlbl_cipsov4_doi_free(struct rcu_head *entry)
{
	struct cipso_v4_doi *ptr;

	ptr = container_of(entry, struct cipso_v4_doi, rcu);
	switch (ptr->type) {
	case CIPSO_V4_MAP_STD:
		kfree(ptr->map.std->lvl.cipso);
		kfree(ptr->map.std->lvl.local);
		kfree(ptr->map.std->cat.cipso);
		kfree(ptr->map.std->cat.local);
		break;
	}
	kfree(ptr);
}