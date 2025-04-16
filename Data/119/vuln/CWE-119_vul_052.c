void __mtk_ppe_check_skb(struct mtk_ppe *ppe, struct sk_buff *skb, u16 hash)
{
	struct hlist_head *head = &ppe->foe_flow[hash / 2];
	struct mtk_foe_entry *hwe = &ppe->foe_table[hash];
	struct mtk_flow_entry *entry;
	struct mtk_foe_bridge key = {};
	struct ethhdr *eh;
	bool found = false;
	u8 *tag;

	spin_lock_bh(&ppe_lock);

	if (FIELD_GET(MTK_FOE_IB1_STATE, hwe->ib1) == MTK_FOE_STATE_BIND)
		goto out;

	hlist_for_each_entry(entry, head, list) {
		if (entry->type == MTK_FLOW_TYPE_L2_SUBFLOW) {
			if (unlikely(FIELD_GET(MTK_FOE_IB1_STATE, hwe->ib1) ==
				     MTK_FOE_STATE_BIND))
				continue;

			entry->hash = 0xffff;
			__mtk_foe_entry_clear(ppe, entry);
			continue;
		}

		if (found || !mtk_flow_entry_match(entry, hwe)) {
			if (entry->hash != 0xffff)
				entry->hash = 0xffff;
			continue;
		}

		entry->hash = hash;
		__mtk_foe_entry_commit(ppe, &entry->data, hash);
		found = true;
	}

	if (found)
		goto out;

	eh = eth_hdr(skb);
	ether_addr_copy(key.dest_mac, eh->h_dest);
	ether_addr_copy(key.src_mac, eh->h_source);
	tag = skb->data - 2;
	key.vlan = 0;
	switch (skb->protocol) {
#if IS_ENABLED(CONFIG_NET_DSA)
	case htons(ETH_P_XDSA):
		if (!netdev_uses_dsa(skb->dev) ||
		    skb->dev->dsa_ptr->tag_ops->proto != DSA_TAG_PROTO_MTK)
			goto out;

		tag += 4;
		if (get_unaligned_be16(tag) != ETH_P_8021Q)
			break;

		fallthrough;
#endif
	case htons(ETH_P_8021Q):
		key.vlan = get_unaligned_be16(tag + 2) & VLAN_VID_MASK;
		break;
	default:
		break;
	}

	entry = rhashtable_lookup_fast(&ppe->l2_flows, &key, mtk_flow_l2_ht_params);
	if (!entry)
		goto out;

	mtk_foe_entry_commit_subflow(ppe, entry, hash);

out:
	spin_unlock_bh(&ppe_lock);
}