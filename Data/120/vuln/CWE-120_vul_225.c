static json_t * register_new_attestation(struct config_module * config, json_t * j_params, json_t * j_scheme_data, json_t * j_credential) {
  json_t * j_return, * j_client_data = NULL, * j_error, * j_result, * j_pubkey = NULL, * j_cert = NULL, * j_query, * j_element = NULL;
  unsigned char * client_data = NULL, * challenge_b64 = NULL, * att_obj = NULL, * cbor_bs_handle = NULL, rpid_hash[32], * fmt = NULL, * credential_id_b64 = NULL, * cbor_auth_data, * cred_pub_key, cert_x[256], cert_y[256], pubkey_export[1024];
  char * challenge_hash = NULL, * message = NULL;
  const char * rpid = NULL;
  size_t client_data_len = 0, challenge_b64_len = 0, att_obj_len = 0, rpid_hash_len = 32, fmt_len = 0, credential_id_len = 0, credential_id_b64_len, cbor_auth_data_len, cred_pub_key_len, cert_x_len = 0, cert_y_len = 0, pubkey_export_len = 1024, index = 0, cbor_bs_handle_len, rpid_len;
  uint32_t counter = 0;
  int ret = G_OK, res, status, has_x = 0, has_y = 0, key_type_valid = 0, key_alg_valid = 0;
  unsigned int i;
  struct cbor_load_result cbor_result;
  cbor_item_t * item = NULL, * key = NULL, * auth_data = NULL, * att_stmt = NULL, * cbor_cose = NULL, * cbor_key, * cbor_value;
  gnutls_pubkey_t g_key = NULL;
  gnutls_datum_t g_x, g_y;
  gnutls_ecc_curve_t curve = GNUTLS_ECC_CURVE_INVALID;

  if (j_scheme_data != NULL) {
    j_error = json_array();
    if (j_error != NULL) {
      do {
        if (json_string_null_or_empty(json_object_get(json_object_get(j_scheme_data, "credential"), "rawId"))) {
          json_array_append_new(j_error, json_string("rawId mandatory"));
          ret = G_ERROR_PARAM;
          break;
        }
        if (json_string_null_or_empty(json_object_get(json_object_get(json_object_get(j_scheme_data, "credential"), "response"), "clientDataJSON"))) {
          json_array_append_new(j_error, json_string("clientDataJSON mandatory"));
          ret = G_ERROR_PARAM;
          break;
        }
        if ((client_data = o_malloc(json_string_length(json_object_get(json_object_get(json_object_get(j_scheme_data, "credential"), "response"), "clientDataJSON"))+4)) == NULL) {
          y_log_message(Y_LOG_LEVEL_ERROR, "register_new_attestation - Error allocating resources for client_data");
          json_array_append_new(j_error, json_string("Internal error"));
          ret = G_ERROR_MEMORY;
          break;
        }
        if (!o_base64_decode((const unsigned char *)json_string_value(json_object_get(json_object_get(json_object_get(j_scheme_data, "credential"), "response"), "clientDataJSON")), json_string_length(json_object_get(json_object_get(json_object_get(j_scheme_data, "credential"), "response"), "clientDataJSON")), client_data, &client_data_len)) {
          y_log_message(Y_LOG_LEVEL_DEBUG, "register_new_attestation - Error o_base64_decode client_data");
          json_array_append_new(j_error, json_string("Internal error"));
          ret = G_ERROR_PARAM;
          break;
        }
        client_data[client_data_len] = '\0';
        j_client_data = json_loads((const char *)client_data, JSON_DECODE_ANY, NULL);
        if (j_client_data == NULL) {
          json_array_append_new(j_error, json_string("Error parsing JSON client data"));
          ret = G_ERROR_PARAM;
          break;
        }
        // Step 3
        if (0 != o_strcmp(json_string_value(json_object_get(j_client_data, "type")), "webauthn.create")) {
          json_array_append_new(j_error, json_string("clientDataJSON.type invalid"));
          ret = G_ERROR_PARAM;
          break;
        }
        // Step 4
        if (json_string_null_or_empty(json_object_get(j_client_data, "challenge"))) {
          json_array_append_new(j_error, json_string("clientDataJSON.challenge mandatory"));
          ret = G_ERROR_PARAM;
          break;
        }
        if ((challenge_b64 = o_malloc(json_string_length(json_object_get(j_client_data, "challenge"))+3)) == NULL) {
          y_log_message(Y_LOG_LEVEL_ERROR, "register_new_attestation - Error allocating resources for challenge_b64");
          json_array_append_new(j_error, json_string("Internal error"));
          ret = G_ERROR_MEMORY;
          break;
        }
        if (!o_base64url_2_base64((unsigned char *)json_string_value(json_object_get(j_client_data, "challenge")), json_string_length(json_object_get(j_client_data, "challenge")), challenge_b64, &challenge_b64_len)) {
          json_array_append_new(j_error, json_string("clientDataJSON.challenge invalid format"));
          ret = G_ERROR_PARAM;
          break;
        }
        challenge_b64[challenge_b64_len] = '\0';
        if ((challenge_hash = generate_hash(config->hash_algorithm, (const char *)challenge_b64)) == NULL) {
          y_log_message(Y_LOG_LEVEL_ERROR, "register_new_attestation - Error generate_hash for challenge_b64");
          json_array_append_new(j_error, json_string("Internal error"));
          ret = G_ERROR;
          break;
        }
        if (0 != o_strcmp(challenge_hash, json_string_value(json_object_get(j_credential, "challenge_hash")))) {
          json_array_append_new(j_error, json_string("clientDataJSON.challenge invalid"));
          ret = G_ERROR_PARAM;
          break;
        }
        // Step 5
        if (json_string_null_or_empty(json_object_get(j_client_data, "origin"))) {
          json_array_append_new(j_error, json_string("clientDataJSON.origin mandatory"));
          ret = G_ERROR_PARAM;
          break;
        }
        if (0 != o_strcmp(json_string_value(json_object_get(j_params, "rp-origin")), json_string_value(json_object_get(j_client_data, "origin")))) {
          message = msprintf("clientDataJSON.origin invalid - Client send %s, required %s", json_string_value(json_object_get(j_client_data, "origin")), json_string_value(json_object_get(j_params, "rp-origin")));
          json_array_append_new(j_error, json_string(message));
          o_free(message);
          ret = G_ERROR_PARAM;
          break;
        }
        // Step 6 ??

        if (json_string_null_or_empty(json_object_get(json_object_get(json_object_get(j_scheme_data, "credential"), "response"), "attestationObject"))) {
          json_array_append_new(j_error, json_string("attestationObject required"));
          ret = G_ERROR_PARAM;
          break;
        }

        if ((att_obj = o_malloc(json_string_length(json_object_get(json_object_get(json_object_get(j_scheme_data, "credential"), "response"), "attestationObject"))+4)) == NULL) {
          y_log_message(Y_LOG_LEVEL_ERROR, "register_new_attestation - Error allocating resources for o_malloc");
          ret = G_ERROR_MEMORY;
          break;
        }
        if (!o_base64_decode((unsigned char *)json_string_value(json_object_get(json_object_get(json_object_get(j_scheme_data, "credential"), "response"), "attestationObject")), json_string_length(json_object_get(json_object_get(json_object_get(j_scheme_data, "credential"), "response"), "attestationObject")), att_obj, &att_obj_len)) {
          json_array_append_new(j_error, json_string("attestationObject invalid base64"));
          ret = G_ERROR_PARAM;
          break;
        }

        // Step 7
        item = cbor_load(att_obj, att_obj_len, &cbor_result);
        if (cbor_result.error.code != CBOR_ERR_NONE) {
          json_array_append_new(j_error, json_string("attestationObject invalid cbor"));
          ret = G_ERROR_PARAM;
          break;
        }
        if (!cbor_isa_map(item)) {
          json_array_append_new(j_error, json_string("attestationObject invalid cbor item"));
          ret = G_ERROR_PARAM;
          break;
        }

        // Check attestation object
        if (cbor_map_size(item) != 3) {
          json_array_append_new(j_error, json_string("attestationObject invalid cbor item"));
          ret = G_ERROR_PARAM;
          break;
        }

        for (i=0; i<3; i++) {
          key = cbor_map_handle(item)[i].key;
          if (cbor_isa_string(key)) {
            if (0 == o_strncmp((const char *)cbor_string_handle(key), "fmt", MIN(o_strlen("fmt"), cbor_string_length(key)))) {
              if (!cbor_isa_string(cbor_map_handle(item)[i].value)) {
                json_array_append_new(j_error, json_string("CBOR map value 'fmt' isnt't a string"));
                ret = G_ERROR_PARAM;
                break;
              } else {
                fmt_len = cbor_string_length(cbor_map_handle(item)[i].value);
                fmt = cbor_string_handle(cbor_map_handle(item)[i].value);
              }
            } else if (0 == o_strncmp((const char *)cbor_string_handle(key), "attStmt", MIN(o_strlen("attStmt"), cbor_string_length(key)))) {
              att_stmt = cbor_map_handle(item)[i].value;
            } else if (0 == o_strncmp((const char *)cbor_string_handle(key), "authData", MIN(o_strlen("authData"), cbor_string_length(key)))) {
              auth_data = cbor_map_handle(item)[i].value;
              if (!cbor_isa_bytestring(auth_data) || cbor_bytestring_length(auth_data) < 56 || cbor_bytestring_is_indefinite(auth_data)) {
                json_array_append_new(j_error, json_string("CBOR map value 'authData' is invalid"));
                ret = G_ERROR_PARAM;
                break;
              }
            } else {
              message = msprintf("CBOR map element %d is not an expected item", i);
              json_array_append_new(j_error, json_string(message));
              o_free(message);
              ret = G_ERROR_PARAM;
              break;
            }
          }
        }

        // Step 9
        if (auth_data == NULL) {
          json_array_append_new(j_error, json_string("authData invalid"));
          ret = G_ERROR_PARAM;
          break;
        }

        cbor_bs_handle = cbor_bytestring_handle(auth_data);
        cbor_bs_handle_len = cbor_bytestring_length(auth_data);
        if (o_strstr(json_string_value(json_object_get(j_params, "rp-origin")), "://") == NULL) {
          y_log_message(Y_LOG_LEVEL_DEBUG, "register_new_attestation - rp-origin invalid");
          json_array_append_new(j_error, json_string("Internal error"));
          ret = G_ERROR_PARAM;
          break;
        }

        if (o_strstr(json_string_value(json_object_get(j_params, "rp-origin")), "://") != NULL) {
          rpid = o_strstr(json_string_value(json_object_get(j_params, "rp-origin")), "://")+3;
        } else {
          rpid = json_string_value(json_object_get(j_params, "rp-origin"));
        }
        if (o_strchr(rpid, ':') != NULL) {
          rpid_len = (size_t)(o_strchr(rpid, ':') - rpid);
        } else {
          rpid_len = o_strlen(rpid);
        }

        if (!generate_digest_raw(digest_SHA256, (unsigned char *)rpid, rpid_len, rpid_hash, &rpid_hash_len)) {
          y_log_message(Y_LOG_LEVEL_ERROR, "register_new_attestation - Error generate_digest_raw");
          json_array_append_new(j_error, json_string("Internal error"));
          ret = G_ERROR_PARAM;
          break;
        }

        if (0 != memcmp(cbor_bs_handle, rpid_hash, rpid_hash_len)) {
          json_array_append_new(j_error, json_string("authData.rpIdHash invalid"));
          ret = G_ERROR_PARAM;
          break;
        }

        // Step 10
        if (!(cbor_bs_handle[FLAGS_OFFSET] & FLAG_USER_PRESENT)) {
          json_array_append_new(j_error, json_string("authData.userPresent not set"));
          ret = G_ERROR_PARAM;
          break;
        }
        if (!(cbor_bs_handle[FLAGS_OFFSET] & FLAG_AT)) {
          json_array_append_new(j_error, json_string("authData.Attested credential data not set"));
          ret = G_ERROR_PARAM;
          break;
        }

        // Step 11 ignored for now
        //y_log_message(Y_LOG_LEVEL_DEBUG, "authData.userVerified: %d", !!(cbor_bs_handle[FLAGS_OFFSET] & FLAG_USER_VERIFY));

        // Step 12 ignored for now (no extension)
        //y_log_message(Y_LOG_LEVEL_DEBUG, "authData.Extension: %d", !!(cbor_bs_handle[FLAGS_OFFSET] & FLAG_ED));

        credential_id_len = (size_t)(cbor_bs_handle[CRED_ID_L_OFFSET+1] | (cbor_bs_handle[CRED_ID_L_OFFSET] << 8));
        if (cbor_bs_handle_len < CRED_ID_L_OFFSET+2+credential_id_len) {
          json_array_append_new(j_error, json_string("auth_data invalid size"));
          ret = G_ERROR_PARAM;
          break;
        }

        credential_id_b64 = o_malloc(credential_id_len*2);
        if (credential_id_b64 == NULL) {
          y_log_message(Y_LOG_LEVEL_ERROR, "register_new_attestation - Error o_malloc for credential_id_b64");
          json_array_append_new(j_error, json_string("Internal error"));
          ret = G_ERROR_MEMORY;
          break;
        }

        if (!o_base64_encode(cbor_bs_handle+CRED_ID_L_OFFSET+2, credential_id_len, credential_id_b64, &credential_id_b64_len)) {
          y_log_message(Y_LOG_LEVEL_DEBUG, "register_new_attestation - Error o_base64_encode for credential_id_b64");
          json_array_append_new(j_error, json_string("Internal error"));
          ret = G_ERROR_PARAM;
          break;
        }

        // Compare credential_id_b64 with rawId
        if (memcmp(credential_id_b64, json_string_value(json_object_get(json_object_get(j_scheme_data, "credential"), "rawId")), MIN(json_string_length(json_object_get(json_object_get(j_scheme_data, "credential"), "rawId")), credential_id_b64_len))) {
          json_array_append_new(j_error, json_string("Invalid rawId"));
          ret = G_ERROR_PARAM;
          break;
        }

        // Extract public key from auth_data COSE structure

        // Extract credential ID
        cbor_auth_data_len = cbor_bytestring_length(auth_data);
        cbor_auth_data = cbor_bytestring_handle(auth_data);

        cred_pub_key = cbor_auth_data+CREDENTIAL_ID_OFFSET+credential_id_len;
        cred_pub_key_len = cbor_auth_data_len-CREDENTIAL_ID_OFFSET-credential_id_len;
        cbor_cose = cbor_load(cred_pub_key, cred_pub_key_len, &cbor_result);
        if (cbor_result.error.code != CBOR_ERR_NONE) {
          json_array_append_new(j_error, json_string("Invalid COSE key"));
          y_log_message(Y_LOG_LEVEL_DEBUG, "register_new_attestation - Error cbor_load cbor_cose");
          ret = G_ERROR_PARAM;
          break;
        }

        if (!cbor_isa_map(cbor_cose)) {
          json_array_append_new(j_error, json_string("Invalid COSE key"));
          y_log_message(Y_LOG_LEVEL_DEBUG, "register_new_attestation - Error cbor_cose not a map");
          ret = G_ERROR_PARAM;
          break;
        }

        for (i=0; i<cbor_map_size(cbor_cose); i++) {
          cbor_key = cbor_map_handle(cbor_cose)[i].key;
          cbor_value = cbor_map_handle(cbor_cose)[i].value;
          if (cbor_isa_negint(cbor_key) && cbor_get_int(cbor_key) == 1 && cbor_isa_bytestring(cbor_value)) {
            has_x = 1;
            memcpy(cert_x, cbor_bytestring_handle(cbor_value), cbor_bytestring_length(cbor_value));
            cert_x_len = cbor_bytestring_length(cbor_value);
            g_x.data = cert_x;
            g_x.size = (unsigned int)cbor_bytestring_length(cbor_value);
          } else if (cbor_isa_negint(cbor_key) && cbor_get_int(cbor_key) == 2 && cbor_isa_bytestring(cbor_value)) {
            has_y = 1;
            memcpy(cert_y, cbor_bytestring_handle(cbor_value), cbor_bytestring_length(cbor_value));
            cert_y_len = cbor_bytestring_length(cbor_value);
            g_y.data = cert_y;
            g_y.size = (unsigned int)cbor_bytestring_length(cbor_value);
          } else if (cbor_isa_uint(cbor_key) && cbor_get_int(cbor_key) == 1 && cbor_isa_uint(cbor_value) && cbor_get_int(cbor_value) == 2) {
            key_type_valid = 1;
          } else if (cbor_isa_uint(cbor_key) && cbor_get_int(cbor_key) == 3 && cbor_isa_negint(cbor_value)) {
            if (cbor_get_int(cbor_value) == 6 || cbor_get_int(cbor_value) == 34 || cbor_get_int(cbor_value) == 35) {
              json_array_foreach(json_object_get(j_params, "pubKey-cred-params"), index, j_element) {
                if (cbor_get_int(cbor_value) == 6 && json_integer_value(json_object_get(j_element, "alg")) == ECDSA256) {
                  key_alg_valid = 1;
                  curve = GNUTLS_ECC_CURVE_SECP256R1;
                } else if (cbor_get_int(cbor_value) == 34 && json_integer_value(json_object_get(j_element, "alg")) == ECDSA384) {
                  key_alg_valid = 1;
                  curve = GNUTLS_ECC_CURVE_SECP384R1;
                } else if (cbor_get_int(cbor_value) == 35 && json_integer_value(json_object_get(j_element, "alg")) == ECDSA512) {
                  key_alg_valid = 1;
                  curve = GNUTLS_ECC_CURVE_SECP521R1;
                }
              }
            }
          }
        }

        if (!has_x || !has_y || !key_type_valid || !key_alg_valid) {
          json_array_append_new(j_error, json_string("Invalid COSE key"));
          y_log_message(Y_LOG_LEVEL_DEBUG, "register_new_attestation - Error invalid COSE key has_x %d && has_y %d && key_type_valid %d && key_alg_valid %d", has_x, has_y, key_type_valid, key_alg_valid);
          ret = G_ERROR_PARAM;
          break;
        }

        if (gnutls_pubkey_init(&g_key)) {
          json_array_append_new(j_error, json_string("Internal error"));
          y_log_message(Y_LOG_LEVEL_DEBUG, "register_new_attestation - Error gnutls_pubkey_init");
          ret = G_ERROR_PARAM;
          break;
        }
        if (gnutls_pubkey_import_ecc_raw(g_key, curve, &g_x, &g_y) < 0) {
          json_array_append_new(j_error, json_string("Internal error"));
          y_log_message(Y_LOG_LEVEL_DEBUG, "register_new_attestation - error gnutls_pubkey_import_ecc_raw");
          ret = G_ERROR_PARAM;
          break;
        }
        if ((ret = gnutls_pubkey_export(g_key, GNUTLS_X509_FMT_PEM, pubkey_export, &pubkey_export_len)) < 0) {
          json_array_append_new(j_error, json_string("Error exporting pubkey"));
          y_log_message(Y_LOG_LEVEL_DEBUG, "register_new_attestation - Error gnutls_pubkey_export: %d", ret);
          ret = G_ERROR_PARAM;
          break;
        }

        // Steps 13-14
        if (0 == o_strncmp("packed", (char *)fmt, MIN(fmt_len, o_strlen("packed"))) && (json_object_get(json_object_get(j_params, "fmt"), "packed") == json_true())) {
          j_result = check_attestation_packed(j_params, auth_data, att_stmt, client_data, g_key);
          if (check_result_value(j_result, G_ERROR_PARAM)) {
            json_array_extend(j_error, json_object_get(j_result, "error"));
            ret = G_ERROR_PARAM;
          } else if (!check_result_value(j_result, G_OK)) {
            ret = G_ERROR_PARAM;
            y_log_message(Y_LOG_LEVEL_ERROR, "register_new_attestation - Error check_attestation_packed");
            json_array_append_new(j_error, json_string("internal error"));
          } else {
            j_cert = json_incref(json_object_get(json_object_get(j_result, "data"), "certificate"));
          }
          json_decref(j_result);
        } else if (0 == o_strncmp("tpm", (char *)fmt, MIN(fmt_len, o_strlen("tpm"))) && (json_object_get(json_object_get(j_params, "fmt"), "tpm") == json_true())) {
          json_array_append_new(j_error, json_string("Format 'tpm' not supported yet"));
          ret = G_ERROR_PARAM;
        } else if (0 == o_strncmp("android-key", (char *)fmt, MIN(fmt_len, o_strlen("android-key"))) && (json_object_get(json_object_get(j_params, "fmt"), "android-key") == json_true())) {
          json_array_append_new(j_error, json_string("Format 'android-key' not supported yet"));
          ret = G_ERROR_PARAM;
        } else if (0 == o_strncmp("android-safetynet", (char *)fmt, MIN(fmt_len, o_strlen("android-safetynet"))) && (json_object_get(json_object_get(j_params, "fmt"), "android-safetynet") == json_true())) {
          j_result = check_attestation_android_safetynet(j_params, auth_data, att_stmt, client_data);
          if (check_result_value(j_result, G_ERROR_PARAM)) {
            json_array_extend(j_error, json_object_get(j_result, "error"));
            ret = G_ERROR_PARAM;
          } else if (!check_result_value(j_result, G_OK)) {
            ret = G_ERROR_PARAM;
            y_log_message(Y_LOG_LEVEL_ERROR, "register_new_attestation - Error check_attestation_android_safetynet");
            json_array_append_new(j_error, json_string("internal error"));
          } else {
            j_cert = json_incref(json_object_get(json_object_get(j_result, "data"), "certificate"));
          }
          json_decref(j_result);
        } else if (0 == o_strncmp("fido-u2f", (char *)fmt, MIN(fmt_len, o_strlen("fido-u2f"))) && (json_object_get(json_object_get(j_params, "fmt"), "fido-u2f") == json_true())) {
          j_result = check_attestation_fido_u2f(j_params, (cbor_auth_data+CREDENTIAL_ID_OFFSET), credential_id_len, cert_x, cert_x_len, cert_y, cert_y_len, att_stmt, rpid_hash, rpid_hash_len, client_data);
          if (check_result_value(j_result, G_ERROR_PARAM)) {
            json_array_extend(j_error, json_object_get(j_result, "error"));
            ret = G_ERROR_PARAM;
          } else if (!check_result_value(j_result, G_OK)) {
            ret = G_ERROR_PARAM;
            y_log_message(Y_LOG_LEVEL_ERROR, "register_new_attestation - Error check_attestation_fido_u2f");
            json_array_append_new(j_error, json_string("internal error"));
          } else {
            j_cert = json_incref(json_object_get(json_object_get(j_result, "data"), "certificate"));
          }
          json_decref(j_result);
        } else if (0 == o_strncmp("apple", (char *)fmt, MIN(fmt_len, o_strlen("apple"))) && (json_object_get(json_object_get(j_params, "fmt"), "apple") == json_true() || json_object_get(j_params, "force-fmt-apple") == json_true())) {
          j_result = check_attestation_apple(j_params, auth_data, att_stmt, client_data, g_key);
          if (check_result_value(j_result, G_ERROR_PARAM)) {
            json_array_extend(j_error, json_object_get(j_result, "error"));
            ret = G_ERROR_PARAM;
          } else if (!check_result_value(j_result, G_OK)) {
            ret = G_ERROR_PARAM;
            y_log_message(Y_LOG_LEVEL_ERROR, "register_new_attestation - Error check_attestation_apple");
            json_array_append_new(j_error, json_string("internal error"));
          } else {
            j_cert = json_incref(json_object_get(json_object_get(j_result, "data"), "certificate"));
          }
          json_decref(j_result);
        } else if (0 == o_strncmp("none", (char *)fmt, MIN(fmt_len, o_strlen("none"))) && (json_object_get(json_object_get(j_params, "fmt"), "none") == json_true() || json_object_get(j_params, "force-fmt-none") == json_true())) {
          if (att_stmt != NULL && cbor_isa_map(att_stmt) && cbor_map_is_definite(att_stmt) && !cbor_map_size(att_stmt)) {
            j_cert = json_string("");
          } else {
            y_log_message(Y_LOG_LEVEL_DEBUG, "register_new_attestation - response type 'none' has invalid format");
            json_array_append_new(j_error, json_string("response invalid"));
            ret = G_ERROR_PARAM;
          }
        } else {
          message = msprintf("Format '%.*s' is not supported by Glewlwyd WebAuthn scheme", fmt_len, fmt);
          json_array_append_new(j_error, json_string(message));
          o_free(message);
          ret = G_ERROR_PARAM;
        }
      } while (0); // This is not a loop, but a structure where you can easily cancel the rest of the process with breaks

      if (ret != G_OK) {
        if (json_array_size(j_error)) {
          j_return = json_pack("{sisO}", "result", ret, "error", j_error);
        } else {
          j_return = json_pack("{si}", "result", ret);
        }
      } else {
        if ((res = check_certificate(config, j_params, json_string_value(json_object_get(json_object_get(j_scheme_data, "credential"), "rawId")), json_integer_value(json_object_get(j_credential, "gswu_id")))) == G_OK) {
          j_return = json_pack("{sis[s]}", "result", G_ERROR_PARAM, "error", "Credential already registered");
          status = 2;
        } else if (res == G_ERROR_UNAUTHORIZED) {
          j_return = json_pack("{sis[s]}", "result", G_ERROR_PARAM, "error", "Credential unauthorized");
          status = 2;
        } else if (res != G_ERROR_NOT_FOUND) {
          j_return = json_pack("{sis[s]}", "result", G_ERROR_PARAM, "error", "Internal error");
          y_log_message(Y_LOG_LEVEL_ERROR, "register_new_attestation - Error check_certificate");
          status = 2;
        } else {
          j_return = json_pack("{si}", "result", G_OK);
          status = 1;
        }
        counter = (uint32_t)(cbor_bs_handle[COUNTER_OFFSET+3] | (cbor_bs_handle[COUNTER_OFFSET+2] << 8) | (cbor_bs_handle[COUNTER_OFFSET+1] << 16) | (cbor_bs_handle[COUNTER_OFFSET] << 24));
        // Store credential in the database
        j_query = json_pack("{sss{siss%sOss%sOsi}s{sO}}",
                            "table",
                            G_TABLE_WEBAUTHN_CREDENTIAL,
                            "set",
                              "gswc_status",
                              status,
                              "gswc_name",
                              fmt,
                              fmt_len,
                              "gswc_credential_id",
                              json_object_get(json_object_get(j_scheme_data, "credential"), "rawId"),
                              "gswc_public_key",
                              pubkey_export,
                              pubkey_export_len,
                              "gswc_certificate",
                              j_cert,
                              "gswc_counter",
                              counter,
                            "where",
                              "gswc_id",
                              json_object_get(j_credential, "gswc_id"));
        res = h_update(config->conn, j_query, NULL);
        json_decref(j_query);
        if (res != H_OK) {
          y_log_message(Y_LOG_LEVEL_ERROR, "register_new_attestation - Error h_update");
        }
      }
      json_decref(j_error);
      json_decref(j_client_data);
      json_decref(j_pubkey);
      json_decref(j_cert);
      o_free(client_data);
      o_free(challenge_b64);
      o_free(challenge_hash);
      o_free(att_obj);
      o_free(credential_id_b64);
      gnutls_pubkey_deinit(g_key);
      if (item != NULL) {
        cbor_decref(&item);
      }
      if (cbor_cose != NULL) {
        cbor_decref(&cbor_cose);
      }
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "register_new_attestation - Error allocating resources for j_error");
      j_return = json_pack("{si}", "result", G_ERROR);
    }
  } else {
    j_return = json_pack("{sis[s]}", "result", G_ERROR_PARAM, "error", "scheme_data mandatory");
  }
  return j_return;
}