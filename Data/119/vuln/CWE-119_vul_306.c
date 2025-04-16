err_status_t
crypto_policy_set_from_profile_for_rtp(crypto_policy_t *policy, 
				       srtp_profile_t profile) {

  /* set SRTP policy from the SRTP profile in the key set */
  switch(profile) {
  case srtp_profile_aes128_cm_sha1_80:
    crypto_policy_set_aes_cm_128_hmac_sha1_80(policy);
    crypto_policy_set_aes_cm_128_hmac_sha1_80(policy);
    break;
  case srtp_profile_aes128_cm_sha1_32:
    crypto_policy_set_aes_cm_128_hmac_sha1_32(policy);
    crypto_policy_set_aes_cm_128_hmac_sha1_80(policy);
    break;
  case srtp_profile_null_sha1_80:
    crypto_policy_set_null_cipher_hmac_sha1_80(policy);
    crypto_policy_set_null_cipher_hmac_sha1_80(policy);
    break;
  case srtp_profile_aes256_cm_sha1_80:
    crypto_policy_set_aes_cm_256_hmac_sha1_80(policy);
    crypto_policy_set_aes_cm_256_hmac_sha1_80(policy);
    break;
  case srtp_profile_aes256_cm_sha1_32:
    crypto_policy_set_aes_cm_256_hmac_sha1_32(policy);
    crypto_policy_set_aes_cm_256_hmac_sha1_80(policy);
    break;
    /* the following profiles are not (yet) supported */
  case srtp_profile_null_sha1_32:
  default:
    return err_status_bad_param;
  }

  return err_status_ok;
}