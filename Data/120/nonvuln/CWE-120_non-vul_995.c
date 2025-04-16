static inline uint16_t vmid_to_domainid(uint16_t vm_id)
{
	return vm_id + 1U;
}