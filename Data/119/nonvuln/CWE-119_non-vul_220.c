static inline bool is_valid_mask(int mask, int req, int opt)
{
	if ((mask & req) != req)
		return false;

	if (mask & ~(req | opt))
		return false;

	return true;
}