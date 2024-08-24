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
	printf("%2d %2d  %02x %04x %04x %3d %04x  %02x  %02x %04x",
		pkt.icmp.icmp_ip.ip_v, pkt.icmp.icmp_ip.ip_hl,
		pkt.icmp.icmp_ip.ip_tos, ntohs(pkt.icmp.icmp_ip.ip_len),
		ntohs(pkt.icmp.icmp_ip.ip_id),
		(ntohs(pkt.icmp.icmp_ip.ip_off) & 0xE000) >> 13,
		ntohs(pkt.icmp.icmp_ip.ip_off) & 0x1FFF,
		pkt.icmp.icmp_ip.ip_ttl, pkt.icmp.icmp_ip.ip_p,
		ntohs(pkt.icmp.icmp_ip.ip_sum));
	printf(" %s ", inet_ntoa(pkt.icmp.icmp_ip.ip_src));
	printf(" %s ", inet_ntoa(pkt.icmp.icmp_ip.ip_dst));
	printf("\nICMP: type %d, code %d, size %zu, id 0x%04x, seq 0x%04x\n",
		pkt.ping.icmp_type, pkt.ping.icmp_code,
		sizeof(pkt.ping),
		ntohs(pkt.ping.id),
		ntohs(pkt.ping.seq));
}

static void	common_dump(t_fping pkt)
{
	uint8_t	*raw;
	int		i;

	raw = (uint8_t *)&pkt.icmp.icmp_ip;
	printf("IP Hdr Dump:\n ");
	i = 0;
	while (i < 10)
	{
		printf("%02x%02x ", raw[i * 2], raw[i * 2 + 1]);
		i++;
	}
	print_pkt_info(pkt);
}

static void	unreachable_dump(t_fping pkt, t_info *info)
{
	if (ntohs(pkt.ping.id) != info->pid)
		return ;
	printf("%lu bytes from ", ntohs(pkt.ip.ip_len) - sizeof(struct ip));
	if (!ip_to_domain(pkt.ip.ip_dst))
		printf("%s: ", inet_ntoa(pkt.ip.ip_dst));
	else
	{
		printf("%s (%s): ", ip_to_domain(pkt.ip.ip_dst),
			inet_ntoa(pkt.ip.ip_dst));
	}
	if (pkt.icmp.icmp_code == ICMP_UNREACH_HOST)
		printf("Destination Host Unreachable");
	else if (pkt.icmp.icmp_code == ICMP_UNREACH_PORT)
		printf("Destination Port Unreachable");
	else if (pkt.icmp.icmp_code == ICMP_UNREACH_NEEDFRAG)
		printf("Fragmentation needed and DF set");
	else if (pkt.icmp.icmp_code == ICMP_UNREACH_FILTER_PROHIB)
		printf("Packet Filtered");
	printf("\n");
	if (info->opt)
		common_dump(pkt);
}

static void	timeexceed_dump(t_fping pkt, t_info *info)
{
	if (ntohs(pkt.ping.id) != info->pid)
		return ;
	printf("%lu bytes from ", ntohs(pkt.ip.ip_len) - sizeof(struct ip));
	if (!ip_to_domain(pkt.ip.ip_dst))
		printf("%s: ", inet_ntoa(pkt.ip.ip_dst));
	else
	{
		printf("%s (%s): ", ip_to_domain(pkt.ip.ip_dst),
			inet_ntoa(pkt.ip.ip_dst));
	}
	if (pkt.icmp.icmp_code == ICMP_TIMXCEED_INTRANS)
		printf("Time to live exceeded");
	else if (pkt.icmp.icmp_code == ICMP_TIMXCEED_REASS)
		printf("Frag reassembly time exceeded");
	printf("\n");
	if (info->opt)
		common_dump(pkt);
}

void	icmp_error(uint8_t *buf, t_info *info)
{
	t_fping		pkt;

	memcpy(&pkt.ip, buf, sizeof(struct ip));
	memcpy(&pkt.icmp, &buf[sizeof(struct ip)], sizeof(struct icmp));
	memcpy(&pkt.ping, \
		&buf[sizeof(struct ip) + sizeof(struct icmp)], \
		sizeof(t_ping));
	if (pkt.icmp.icmp_type == ICMP_UNREACH)
		unreachable_dump(pkt, info);
	else if (pkt.icmp.icmp_type == ICMP_TIMXCEED)
		timeexceed_dump(pkt, info);
}
