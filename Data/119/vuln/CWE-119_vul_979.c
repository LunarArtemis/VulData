static gint
handle_message_body_parameters(tvbuff_t    *tvb,
                               packet_info *pinfo,
                               proto_tree  *header_tree,
                               guint       encoding,
                               gint        offset,
                               gint32      body_length,
                               guint8      *signature,
                               guint8      signature_length)
{
    gint        packet_length, end_of_body;
    proto_tree *tree;
    proto_item *item;
    const gint  starting_offset = offset;

    packet_length = tvb_reported_length(tvb);

    /* Add a subtree/row for the message body parameters. */
    item = proto_tree_add_item(header_tree, hf_alljoyn_mess_body_parameters, tvb, offset, body_length, ENC_NA);
    tree = proto_item_add_subtree(item, ett_alljoyn_mess_body_parameters);

    end_of_body = offset + body_length;

    if(end_of_body > packet_length) {
        end_of_body = packet_length;
    }

    while(offset < end_of_body && signature && *signature) {
        offset = parse_arg(tvb,
                           pinfo,
                           NULL,
                           encoding,
                           offset,
                           tree,    /* Add the args to the Parameters tree. */
                           FALSE,
                           *signature,
                           HDR_INVALID,
                           &signature,
                           &signature_length,
                           starting_offset);
    }

    return offset;
}