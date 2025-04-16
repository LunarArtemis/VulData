static int sendControllerEventInternal(short controllerNumber, short buttonFlags, unsigned char leftTrigger, unsigned char rightTrigger,
    short leftStickX, short leftStickY, short rightStickX, short rightStickY)
{
    PPACKET_HOLDER holder;
    int err;

    if (!initialized) {
        return -2;
    }

    holder = malloc(sizeof(*holder));
    if (holder == NULL) {
        return -1;
    }

    if (ServerMajorVersion == 3) {
        // Generation 3 servers don't support multiple controllers so we send
        // the legacy packet
        holder->packetLength = sizeof(NV_CONTROLLER_PACKET);
        holder->packet.controller.header.packetType = htonl(PACKET_TYPE_CONTROLLER);
        holder->packet.controller.headerA = C_HEADER_A;
        holder->packet.controller.headerB = C_HEADER_B;
        holder->packet.controller.buttonFlags = buttonFlags;
        holder->packet.controller.leftTrigger = leftTrigger;
        holder->packet.controller.rightTrigger = rightTrigger;
        holder->packet.controller.leftStickX = leftStickX;
        holder->packet.controller.leftStickY = leftStickY;
        holder->packet.controller.rightStickX = rightStickX;
        holder->packet.controller.rightStickY = rightStickY;
        holder->packet.controller.tailA = C_TAIL_A;
        holder->packet.controller.tailB = C_TAIL_B;
    }
    else {
        // Generation 4+ servers support passing the controller number
        holder->packetLength = sizeof(NV_MULTI_CONTROLLER_PACKET);
        holder->packet.multiController.header.packetType = htonl(PACKET_TYPE_MULTI_CONTROLLER);
        holder->packet.multiController.headerA = MC_HEADER_A;
        // On Gen 5 servers, the header code is decremented by one
        if (ServerMajorVersion >= 5) {
            holder->packet.multiController.headerA--;
        }
        holder->packet.multiController.headerB = MC_HEADER_B;
        holder->packet.multiController.controllerNumber = controllerNumber;
        holder->packet.multiController.midA = MC_ACTIVE_CONTROLLER_FLAGS;
        holder->packet.multiController.midB = MC_MID_B;
        holder->packet.multiController.buttonFlags = buttonFlags;
        holder->packet.multiController.leftTrigger = leftTrigger;
        holder->packet.multiController.rightTrigger = rightTrigger;
        holder->packet.multiController.leftStickX = leftStickX;
        holder->packet.multiController.leftStickY = leftStickY;
        holder->packet.multiController.rightStickX = rightStickX;
        holder->packet.multiController.rightStickY = rightStickY;
        holder->packet.multiController.tailA = MC_TAIL_A;
        holder->packet.multiController.tailB = MC_TAIL_B;
    }

    err = LbqOfferQueueItem(&packetQueue, holder, &holder->entry);
    if (err != LBQ_SUCCESS) {
        free(holder);
    }

    return err;
}