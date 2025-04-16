static void inputSendThreadProc(void* context) {
    SOCK_RET err;
    PPACKET_HOLDER holder;
    char encryptedBuffer[MAX_INPUT_PACKET_SIZE];
    int encryptedSize;

    while (!PltIsThreadInterrupted(&inputSendThread)) {
        int encryptedLengthPrefix;

        err = LbqWaitForQueueElement(&packetQueue, (void**)&holder);
        if (err != LBQ_SUCCESS) {
            return;
        }

        // If it's a multi-controller packet we can do batching
        if (holder->packet.multiController.header.packetType == htonl(PACKET_TYPE_MULTI_CONTROLLER)) {
            PPACKET_HOLDER controllerBatchHolder;
            PNV_MULTI_CONTROLLER_PACKET origPkt;
            int dirs[6];

            memset(dirs, 0, sizeof(dirs));

            origPkt = &holder->packet.multiController;
            for (;;) {
                PNV_MULTI_CONTROLLER_PACKET newPkt;

                // Peek at the next packet
                if (LbqPeekQueueElement(&packetQueue, (void**)&controllerBatchHolder) != LBQ_SUCCESS) {
                    break;
                }

                // If it's not a controller packet, we're done
                if (controllerBatchHolder->packet.multiController.header.packetType != htonl(PACKET_TYPE_MULTI_CONTROLLER)) {
                    break;
                }

                // Check if it's able to be batched
                newPkt = &controllerBatchHolder->packet.multiController;
                if (newPkt->buttonFlags != origPkt->buttonFlags ||
                    newPkt->controllerNumber != origPkt->controllerNumber ||
                    !checkDirs(origPkt->leftTrigger, newPkt->leftTrigger, &dirs[0]) ||
                    !checkDirs(origPkt->rightTrigger, newPkt->rightTrigger, &dirs[1]) ||
                    !checkDirs(origPkt->leftStickX, newPkt->leftStickX, &dirs[2]) ||
                    !checkDirs(origPkt->leftStickY, newPkt->leftStickY, &dirs[3]) ||
                    !checkDirs(origPkt->rightStickX, newPkt->rightStickX, &dirs[4]) ||
                    !checkDirs(origPkt->rightStickY, newPkt->rightStickY, &dirs[5])) {
                    // Batching not allowed
                    break;
                }

                // Remove the batchable controller packet
                if (LbqPollQueueElement(&packetQueue, (void**)&controllerBatchHolder) != LBQ_SUCCESS) {
                    break;
                }

                // Update the original packet
                origPkt->leftTrigger = newPkt->leftTrigger;
                origPkt->rightTrigger = newPkt->rightTrigger;
                origPkt->leftStickX = newPkt->leftStickX;
                origPkt->leftStickY = newPkt->leftStickY;
                origPkt->rightStickX = newPkt->rightStickX;
                origPkt->rightStickY = newPkt->rightStickY;

                // Free the batched packet holder
                free(controllerBatchHolder);
            }
        }
        // If it's a mouse move packet, we can also do batching
        else if (holder->packet.mouseMove.header.packetType == htonl(PACKET_TYPE_MOUSE_MOVE)) {
            PPACKET_HOLDER mouseBatchHolder;
            int totalDeltaX = (short)htons(holder->packet.mouseMove.deltaX);
            int totalDeltaY = (short)htons(holder->packet.mouseMove.deltaY);

            for (;;) {
                int partialDeltaX;
                int partialDeltaY;

                // Peek at the next packet
                if (LbqPeekQueueElement(&packetQueue, (void**)&mouseBatchHolder) != LBQ_SUCCESS) {
                    break;
                }

                // If it's not a mouse move packet, we're done
                if (mouseBatchHolder->packet.mouseMove.header.packetType != htonl(PACKET_TYPE_MOUSE_MOVE)) {
                    break;
                }

                partialDeltaX = (short)htons(mouseBatchHolder->packet.mouseMove.deltaX);
                partialDeltaY = (short)htons(mouseBatchHolder->packet.mouseMove.deltaY);

                // Check for overflow
                if (partialDeltaX + totalDeltaX > INT16_MAX ||
                    partialDeltaX + totalDeltaX < INT16_MIN ||
                    partialDeltaY + totalDeltaY > INT16_MAX ||
                    partialDeltaY + totalDeltaY < INT16_MIN) {
                    // Total delta would overflow our 16-bit short
                    break;
                }

                // Remove the batchable mouse move packet
                if (LbqPollQueueElement(&packetQueue, (void**)&mouseBatchHolder) != LBQ_SUCCESS) {
                    break;
                }

                totalDeltaX += partialDeltaX;
                totalDeltaY += partialDeltaY;

                // Free the batched packet holder
                free(mouseBatchHolder);
            }

            // Update the original packet
            holder->packet.mouseMove.deltaX = htons((short)totalDeltaX);
            holder->packet.mouseMove.deltaY = htons((short)totalDeltaY);
        }

        // Encrypt the message into the output buffer while leaving room for the length
        encryptedSize = sizeof(encryptedBuffer) - 4;
        err = encryptData((const unsigned char*)&holder->packet, holder->packetLength,
            (unsigned char*)&encryptedBuffer[4], &encryptedSize);
        free(holder);
        if (err != 0) {
            Limelog("Input: Encryption failed: %d\n", (int)err);
            ListenerCallbacks.connectionTerminated(err);
            return;
        }

        // Prepend the length to the message
        encryptedLengthPrefix = htonl((unsigned long)encryptedSize);
        memcpy(&encryptedBuffer[0], &encryptedLengthPrefix, 4);

        if (ServerMajorVersion < 5) {
            // Send the encrypted payload
            err = send(inputSock, (const char*) encryptedBuffer,
                (int) (encryptedSize + sizeof(encryptedLengthPrefix)), 0);
            if (err <= 0) {
                Limelog("Input: send() failed: %d\n", (int) LastSocketError());
                ListenerCallbacks.connectionTerminated(LastSocketError());
                return;
            }
        }
        else {
            // For reasons that I can't understand, NVIDIA decides to use the last 16
            // bytes of ciphertext in the most recent game controller packet as the IV for
            // future encryption. I think it may be a buffer overrun on their end but we'll have
            // to mimic it to work correctly.
            if (ServerMajorVersion >= 7 && encryptedSize >= 16 + sizeof(currentAesIv)) {
                memcpy(currentAesIv,
                       &encryptedBuffer[4 + encryptedSize - sizeof(currentAesIv)],
                       sizeof(currentAesIv));
            }
            
            err = (SOCK_RET)sendInputPacketOnControlStream((unsigned char*) encryptedBuffer,
                (int) (encryptedSize + sizeof(encryptedLengthPrefix)));
            if (err < 0) {
                Limelog("Input: sendInputPacketOnControlStream() failed: %d\n", (int) err);
                ListenerCallbacks.connectionTerminated(LastSocketError());
                return;
            }
        }
    }
}