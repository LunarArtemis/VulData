static void l2cap_data_channel(struct l2cap_conn *conn, u16 cid,
			       struct sk_buff *skb)
{
	struct l2cap_chan *chan;

	chan = l2cap_get_chan_by_scid(conn, cid);
	if (!chan) {
		if (cid == L2CAP_CID_A2MP) {
			chan = a2mp_channel_create(conn, skb);
			if (!chan) {
				kfree_skb(skb);
				return;
			}

			l2cap_chan_lock(chan);
		} else {
			BT_DBG("unknown cid 0x%4.4x", cid);
			/* Drop packet and return */
			kfree_skb(skb);
			return;
		}
	}

	BT_DBG("chan %p, len %d", chan, skb->len);

	/* If we receive data on a fixed channel before the info req/rsp
	 * procedure is done simply assume that the channel is supported
	 * and mark it as ready.
	 */
	if (chan->chan_type == L2CAP_CHAN_FIXED)
		l2cap_chan_ready(chan);

	if (chan->state != BT_CONNECTED)
		goto drop;

	switch (chan->mode) {
	case L2CAP_MODE_LE_FLOWCTL:
	case L2CAP_MODE_EXT_FLOWCTL:
		if (l2cap_ecred_data_rcv(chan, skb) < 0)
			goto drop;

		goto done;

	case L2CAP_MODE_BASIC:
		/* If socket recv buffers overflows we drop data here
		 * which is *bad* because L2CAP has to be reliable.
		 * But we don't have any other choice. L2CAP doesn't
		 * provide flow control mechanism. */

		if (chan->imtu < skb->len) {
			BT_ERR("Dropping L2CAP data: receive buffer overflow");
			goto drop;
		}

		if (!chan->ops->recv(chan, skb))
			goto done;
		break;

	case L2CAP_MODE_ERTM:
	case L2CAP_MODE_STREAMING:
		l2cap_data_rcv(chan, skb);
		goto done;

	default:
		BT_DBG("chan %p: bad mode 0x%2.2x", chan, chan->mode);
		break;
	}

drop:
	kfree_skb(skb);

done:
	l2cap_chan_unlock(chan);
	l2cap_chan_put(chan);
}