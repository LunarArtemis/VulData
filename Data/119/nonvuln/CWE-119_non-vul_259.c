static void mlx5_ib_wq_event(struct mlx5_core_qp *core_qp, int type)
{
	struct mlx5_ib_rwq *rwq = to_mibrwq(core_qp);
	struct mlx5_ib_dev *dev = to_mdev(rwq->ibwq.device);
	struct ib_event event;

	if (rwq->ibwq.event_handler) {
		event.device     = rwq->ibwq.device;
		event.element.wq = &rwq->ibwq;
		switch (type) {
		case MLX5_EVENT_TYPE_WQ_CATAS_ERROR:
			event.event = IB_EVENT_WQ_FATAL;
			break;
		default:
			mlx5_ib_warn(dev, "Unexpected event type %d on WQ %06x\n", type, core_qp->qpn);
			return;
		}

		rwq->ibwq.event_handler(&event, rwq->ibwq.wq_context);
	}
}