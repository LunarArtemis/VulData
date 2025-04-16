boolean NET_ReadSettings(net_packet_t *packet, net_gamesettings_t *settings)
{
    boolean success;
    int i;

    success = NET_ReadInt8(packet, (unsigned int *) &settings->ticdup)
           && NET_ReadInt8(packet, (unsigned int *) &settings->extratics)
           && NET_ReadInt8(packet, (unsigned int *) &settings->deathmatch)
           && NET_ReadInt8(packet, (unsigned int *) &settings->nomonsters)
           && NET_ReadInt8(packet, (unsigned int *) &settings->fast_monsters)
           && NET_ReadInt8(packet, (unsigned int *) &settings->respawn_monsters)
           && NET_ReadInt8(packet, (unsigned int *) &settings->episode)
           && NET_ReadInt8(packet, (unsigned int *) &settings->map)
           && NET_ReadSInt8(packet, &settings->skill)
           && NET_ReadInt8(packet, (unsigned int *) &settings->gameversion)
           && NET_ReadInt8(packet, (unsigned int *) &settings->lowres_turn)
           && NET_ReadInt8(packet, (unsigned int *) &settings->new_sync)
           && NET_ReadInt32(packet, (unsigned int *) &settings->timelimit)
           && NET_ReadSInt8(packet, (signed int *) &settings->loadgame)
           && NET_ReadInt8(packet, (unsigned int *) &settings->random)
           && NET_ReadInt8(packet, (unsigned int *) &settings->num_players)
           && NET_ReadSInt8(packet, (signed int *) &settings->consoleplayer);

    if (!success)
    {
        return false;
    }

    for (i = 0; i < settings->num_players; ++i)
    {
        if (!NET_ReadInt8(packet,
                          (unsigned int *) &settings->player_classes[i]))
        {
            return false;
        }
    }

    return true;
}