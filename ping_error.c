/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping_error.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sungyoon <sungyoon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/24 13:48:24 by sungyoon          #+#    #+#             */
/*   Updated: 2024/08/24 13:52:08 by sungyoon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ping.h"

static void	print_pkt_info(t_fping pkt)
{
	printf("\nVr HL TOS  LEN   ID Flg  Off TTL ");
	printf("Pro  cks      Src\tDst\tData\n");
	printf("%2d %2d  %02x %04x %04x %3d %04x  %02x  %02x %04x\n",
		pkt.ip.ip_v, pkt.ip.ip_hl, pkt.ip.ip_tos, ntohs(pkt.ip.ip_len),
		ntohs(pkt.ip.ip_id), (pkt.ip.ip_off & 0xE000) >> 13,
		ntohs(pkt.ip.ip_off) & 0x1FFF,
		pkt.ip.ip_ttl, pkt.ip.ip_p, ntohs(pkt.ip.ip_sum));
	printf(" %s ", inet_ntoa(pkt.ip.ip_src));
	printf(" %s ", inet_ntoa(pkt.ip.ip_dst));
	printf("ICMP: type %d, code %d, size %zu, id 0x%04x, seq 0x%04x\n",
		pkt.ping.icmp.icmp_type, pkt.ping.icmp.icmp_code,
		sizeof(pkt.ping),
		ntohs(pkt.ping.icmp.icmp_id),
		ntohs(pkt.ping.icmp.icmp_seq));
}

static void	common_dump(t_fping pkt)
{
	uint8_t	*raw;
	int		i;

	raw = (uint8_t *)&pkt;
	printf("IP Hdr Dump:\n ");
	i = 0;
	while (i < 10)
	{
		printf("%02x%02x ", raw[i * 2], raw[i * 2 + 1]);
		i++;
	}
	print_pkt_info(pkt);
}

static void	unreachable_dump(struct ip ip_pkt, struct icmp icmp_pkt, \
						t_fping ping_pkt, t_info *info)
{
	if (ntohs(ping_pkt.ping.icmp.icmp_id) != info->pid)
		return ;
	printf("%lu bytes from ", ntohs(ip_pkt.ip_len) - sizeof(struct ip));
	if (!ip_to_domain(ip_pkt.ip_dst))
		printf("%s: ", inet_ntoa(ip_pkt.ip_dst));
	else
	{
		printf("%s (%s): ", ip_to_domain(ip_pkt.ip_dst),
			inet_ntoa(ip_pkt.ip_dst));
	}
	if (icmp_pkt.icmp_code == ICMP_UNREACH_HOST)
		printf("Destination Host Unreachable");
	else if (icmp_pkt.icmp_code == ICMP_UNREACH_PORT)
		printf("Destination Port Unreachable");
	else if (icmp_pkt.icmp_code == ICMP_UNREACH_NEEDFRAG)
		printf("Fragmentation needed and DF set");
	else if (icmp_pkt.icmp_code == ICMP_UNREACH_FILTER_PROHIB)
		printf("Packet Filtered");
	printf("\n");
	if (info->opt)
		common_dump(ping_pkt);
}

static void	timeexceed_dump(struct ip ip_pkt, struct icmp icmp_pkt, \
					t_fping ping_pkt, t_info *info)
{
	if (ntohs(ping_pkt.ping.icmp.icmp_id) != info->pid)
		return ;
	printf("%lu bytes from ", ntohs(ip_pkt.ip_len) - sizeof(struct ip));
	if (!ip_to_domain(ip_pkt.ip_dst))
		printf("%s: ", inet_ntoa(ip_pkt.ip_dst));
	else
	{
		printf("%s (%s): ", ip_to_domain(ip_pkt.ip_dst),
			inet_ntoa(ip_pkt.ip_dst));
	}
	if (icmp_pkt.icmp_code == ICMP_TIMXCEED_INTRANS)
		printf("Time to live exceeded");
	else if (icmp_pkt.icmp_code == ICMP_TIMXCEED_REASS)
		printf("Frag reassembly time exceeded");
	printf("\n");
	if (info->opt)
		common_dump(ping_pkt);
}

void	icmp_error(uint8_t *buf, t_info *info)
{
	struct ip	ip_pkt;
	struct icmp	icmp_pkt;
	t_fping		ping_pkt;

	memcpy(&ip_pkt, buf, sizeof(struct ip));
	memcpy(&icmp_pkt, &buf[sizeof(struct ip)], sizeof(struct icmp));
	memcpy(&ping_pkt.ip, \
		&buf[sizeof(struct ip) + sizeof(struct icmp)], \
		sizeof(struct ip));
	memcpy(&ping_pkt.ping, \
		&buf[sizeof(struct ip) * 2 + sizeof(struct icmp)], \
		sizeof(t_ping));
	if (icmp_pkt.icmp_type == ICMP_UNREACH)
		unreachable_dump(ip_pkt, icmp_pkt, ping_pkt, info);
	else if (icmp_pkt.icmp_type == ICMP_TIMXCEED)
		timeexceed_dump(ip_pkt, icmp_pkt, ping_pkt, info);
}
