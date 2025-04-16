static int encryptData(const unsigned char* plaintext, int plaintextLen,
                       unsigned char* ciphertext, int* ciphertextLen) {
    int ret;
    int len;
    
    if (ServerMajorVersion >= 7) {
        EVP_CIPHER_CTX_init(&cipherContext);

        // Gen 7 servers use 128-bit AES GCM
        if (EVP_EncryptInit_ex(&cipherContext, EVP_aes_128_gcm(), NULL, NULL, NULL) != 1) {
            ret = -1;
            goto gcm_cleanup;
        }
        
        // Gen 7 servers uses 16 byte IVs
        if (EVP_CIPHER_CTX_ctrl(&cipherContext, EVP_CTRL_GCM_SET_IVLEN, 16, NULL) != 1) {
            ret = -1;
            goto gcm_cleanup;
        }
        
        // Initialize again but now provide our key and current IV
        if (EVP_EncryptInit_ex(&cipherContext, NULL, NULL,
                               (const unsigned char*)StreamConfig.remoteInputAesKey, currentAesIv) != 1) {
            ret = -1;
            goto gcm_cleanup;
        }
        
        // Encrypt into the caller's buffer, leaving room for the auth tag to be prepended
        if (EVP_EncryptUpdate(&cipherContext, &ciphertext[16], ciphertextLen, plaintext, plaintextLen) != 1) {
            ret = -1;
            goto gcm_cleanup;
        }
        
        // GCM encryption won't ever fill ciphertext here but we have to call it anyway
        if (EVP_EncryptFinal_ex(&cipherContext, ciphertext, &len) != 1) {
            ret = -1;
            goto gcm_cleanup;
        }
        LC_ASSERT(len == 0);
        
        // Read the tag into the caller's buffer
        if (EVP_CIPHER_CTX_ctrl(&cipherContext, EVP_CTRL_GCM_GET_TAG, 16, ciphertext) != 1) {
            ret = -1;
            goto gcm_cleanup;
        }
        
        // Increment the ciphertextLen to account for the tag
        *ciphertextLen += 16;
        
        ret = 0;
        
    gcm_cleanup:
        EVP_CIPHER_CTX_cleanup(&cipherContext);
    }
    else {
        unsigned char paddedData[MAX_INPUT_PACKET_SIZE];
        int paddedLength;
        
        if (!cipherInitialized) {
            EVP_CIPHER_CTX_init(&cipherContext);
            cipherInitialized = 1;

            // Prior to Gen 7, 128-bit AES CBC is used for encryption
            if (EVP_EncryptInit_ex(&cipherContext, EVP_aes_128_cbc(), NULL,
                                   (const unsigned char*)StreamConfig.remoteInputAesKey, currentAesIv) != 1) {
                ret = -1;
                goto cbc_cleanup;
            }
        }
        
        // Pad the data to the required block length
        memcpy(paddedData, plaintext, plaintextLen);
        paddedLength = addPkcs7PaddingInPlace(paddedData, plaintextLen);
        
        if (EVP_EncryptUpdate(&cipherContext, ciphertext, ciphertextLen, paddedData, paddedLength) != 1) {
            ret = -1;
            goto cbc_cleanup;
        }
        
        ret = 0;

    cbc_cleanup:
        // Nothing to do
        ;
    }
    
    return ret;
}