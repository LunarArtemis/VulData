int
do_checksum(tcpedit_t *tcpedit, uint8_t *data, int proto, int len) {
    ipv4_hdr_t *ipv4;
    ipv6_hdr_t *ipv6;
    tcp_hdr_t *tcp;
    udp_hdr_t *udp;
    icmpv4_hdr_t *icmp;
    icmpv6_hdr_t *icmp6;
    int ip_hl;
    int sum;

    sum = 0;
    ipv4 = NULL;
    ipv6 = NULL;
    assert(data);

    if (!data || len <= 0 || len > 65535) {
        tcpedit_setwarn(tcpedit, "%s", "Unable to checksum packets with no L3+ data");
        return TCPEDIT_WARN;
    }

    ipv4 = (ipv4_hdr_t *)data;
    if (ipv4->ip_v == 6) {
        ipv6 = (ipv6_hdr_t *)data;
        ipv4 = NULL;

        proto = get_ipv6_l4proto(ipv6, len);
        dbgx(3, "layer4 proto is 0x%hx", (uint16_t)proto);

        ip_hl = (u_char*)get_layer4_v6(ipv6, len) - (u_char*)data;
        dbgx(3, "ip_hl proto is 0x%d", ip_hl);

        len -= (ip_hl - TCPR_IPV6_H);
    } else {
        ip_hl = ipv4->ip_hl << 2;
    }

    switch (proto) {

        case IPPROTO_TCP:
            tcp = (tcp_hdr_t *)(data + ip_hl);
#ifdef STUPID_SOLARIS_CHECKSUM_BUG
            tcp->th_sum = tcp->th_off << 2;
            return (TCPEDIT_OK);
#endif
            tcp->th_sum = 0;

            /* Note, we do both src & dst IP's at the same time, that's why the
             * length is 2x a single IP
             */
            if (ipv6 != NULL) {
                sum = do_checksum_math((uint16_t *)&ipv6->ip_src, 32);
            } else {
                sum = do_checksum_math((uint16_t *)&ipv4->ip_src, 8);
            }
            sum += ntohs(IPPROTO_TCP + len);
            sum += do_checksum_math((uint16_t *)tcp, len);
            tcp->th_sum = CHECKSUM_CARRY(sum);
            break;

        case IPPROTO_UDP:
            udp = (udp_hdr_t *)(data + ip_hl);
            /* No need to recalculate UDP checksums if already 0 */
            if (udp->uh_sum == 0) 
                break; 
            udp->uh_sum = 0;
            if (ipv6 != NULL) {
                sum = do_checksum_math((uint16_t *)&ipv6->ip_src, 32);
            } else {
                sum = do_checksum_math((uint16_t *)&ipv4->ip_src, 8);
            }
            sum += ntohs(IPPROTO_UDP + len);
            sum += do_checksum_math((uint16_t *)udp, len);
            udp->uh_sum = CHECKSUM_CARRY(sum);
            break;

        case IPPROTO_ICMP:
            icmp = (icmpv4_hdr_t *)(data + ip_hl);
            icmp->icmp_sum = 0;
            if (ipv6 != NULL) {
                sum = do_checksum_math((uint16_t *)&ipv6->ip_src, 32);
                icmp->icmp_sum = CHECKSUM_CARRY(sum);                
            }
            sum += do_checksum_math((uint16_t *)icmp, len);
            icmp->icmp_sum = CHECKSUM_CARRY(sum);
            break;

        case IPPROTO_ICMP6:
            icmp6 = (icmpv6_hdr_t *)(data + ip_hl);
            icmp6->icmp_sum = 0;
            if (ipv6 != NULL) {
                sum = do_checksum_math((u_int16_t *)&ipv6->ip_src, 32);
            }
            sum += ntohs(IPPROTO_ICMP6 + len);
            sum += do_checksum_math((u_int16_t *)icmp6, len);
            icmp6->icmp_sum = CHECKSUM_CARRY(sum);
            break;


        case IPPROTO_IP:
            if (ipv4) {
                ipv4->ip_sum = 0;
                sum = do_checksum_math((uint16_t *)data, ip_hl);
                ipv4->ip_sum = CHECKSUM_CARRY(sum);
            }
            break;

        case IPPROTO_IGMP:
        case IPPROTO_GRE:
        case IPPROTO_OSPF:
        case IPPROTO_OSPF_LSA:
        case IPPROTO_VRRP:
        case TCPR_PROTO_CDP: 
        case TCPR_PROTO_ISL:
        default:
            tcpedit_setwarn(tcpedit, "Unsupported protocol for checksum: 0x%x", proto);
            return TCPEDIT_WARN;
    }

    return TCPEDIT_OK;
}