static void
quic_connection_destroy(gpointer data, gpointer user_data _U_)
{
    quic_info_data_t *conn = (quic_info_data_t *)data;
    quic_ciphers_reset(&conn->client_initial_ciphers);
    quic_ciphers_reset(&conn->server_initial_ciphers);
    quic_ciphers_reset(&conn->client_handshake_ciphers);
    quic_ciphers_reset(&conn->server_handshake_ciphers);

    quic_hp_cipher_reset(&conn->client_pp.hp_cipher);
    quic_pp_cipher_reset(&conn->client_pp.pp_ciphers[0]);
    quic_pp_cipher_reset(&conn->client_pp.pp_ciphers[1]);

    quic_hp_cipher_reset(&conn->server_pp.hp_cipher);
    quic_pp_cipher_reset(&conn->server_pp.pp_ciphers[0]);
    quic_pp_cipher_reset(&conn->server_pp.pp_ciphers[1]);
}