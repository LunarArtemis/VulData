static gboolean
check_dcid_on_coalesced_packet(tvbuff_t *tvb, const quic_datagram *dgram_info,
                               gboolean is_first_packet, quic_cid_t *first_packet_dcid)
{
    guint offset = 0;
    guint8 first_byte;
    quic_cid_t dcid = {.len=0};

    first_byte = tvb_get_guint8(tvb, offset);
    offset++;
    if (first_byte & 0x80) {
        offset += 4; /* Skip version */
        dcid.len = tvb_get_guint8(tvb, offset);
        offset++;
        if (dcid.len && dcid.len <= QUIC_MAX_CID_LENGTH) {
            tvb_memcpy(tvb, dcid.cid, offset, dcid.len);
        }
    } else {
        quic_info_data_t *conn = dgram_info->conn;
        gboolean from_server = dgram_info->from_server;
        if (conn) {
            dcid.len = from_server ? conn->client_cids.data.len : conn->server_cids.data.len;
            if (dcid.len) {
                tvb_memcpy(tvb, dcid.cid, offset, dcid.len);
            }
        } else {
            /* If we don't have a valid quic_info_data_t structure for this flow,
               we can't really validate the CID. */
            return TRUE;
        }
    }

    if (is_first_packet) {
        *first_packet_dcid = dcid;
        return TRUE; /* Nothing to check */
    }

    return quic_connection_equal(&dcid, first_packet_dcid);
}