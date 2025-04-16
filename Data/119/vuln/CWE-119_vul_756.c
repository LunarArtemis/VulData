void __key_link_end(struct key *keyring,
		    const struct keyring_index_key *index_key,
		    struct assoc_array_edit *edit)
	__releases(&keyring->sem)
	__releases(&keyring_serialise_link_sem)
{
	BUG_ON(index_key->type == NULL);
	kenter("%d,%s,", keyring->serial, index_key->type->name);

	if (index_key->type == &key_type_keyring)
		up_write(&keyring_serialise_link_sem);

	if (edit && !edit->dead_leaf) {
		key_payload_reserve(keyring,
				    keyring->datalen - KEYQUOTA_LINK_BYTES);
		assoc_array_cancel_edit(edit);
	}
	up_write(&keyring->sem);
}