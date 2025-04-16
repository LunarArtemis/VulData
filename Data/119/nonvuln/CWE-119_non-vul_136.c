static void set_sig_seg(struct mlx5_rwqe_sig *sig, int size)
{
	sig->signature = calc_sig(sig, size);
}