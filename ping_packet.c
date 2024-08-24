/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping_packet.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sungyoon <sungyoon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/24 13:48:31 by sungyoon          #+#    #+#             */
/*   Updated: 2024/08/24 13:50:27 by sungyoon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ping.h"

static uint16_t	calculate_checksum(uint16_t *data, int len)
{
	uint32_t	sum;

	sum = 0;
	while (len > 1)
	{
		sum += *data++;
		len -= 2;
	}
	if (len == 1)
		sum += *(uint8_t *)data;
	while (sum >> 16)
		sum = (sum & 0xFFFF) + (sum >> 16);
	return ((uint16_t)(~sum));
}

t_ping	make_packet(int seq)
{
	t_ping	pkt;
	int		i;

	memset(&pkt, 0, sizeof(pkt));
	pkt.icmp.icmp_type = ICMP_ECHO;
	pkt.icmp.icmp_code = 0;
	pkt.icmp.icmp_id = htons(getpid());
	pkt.icmp.icmp_seq = htons(seq);
	gettimeofday(&pkt.time, NULL);
	i = 0;
	while (i < 40)
	{
		pkt.dummy[i] = i;
		i++;
	}
	pkt.icmp.icmp_cksum = calculate_checksum((uint16_t *)&pkt, sizeof(pkt));
	return (pkt);
}

void	send_packet(t_ping pkt, t_info *info)
{
	struct sockaddr_in	dest_addr;
	int					len;

	memset(&dest_addr, 0, sizeof(dest_addr));
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_addr.s_addr = inet_addr(info->ip);
	len = sendto(info->sock, &pkt, sizeof(pkt), 0, \
		(struct sockaddr *)&dest_addr, sizeof(dest_addr));
	if (len == -1)
		error_handling("ft_ping: sendto failed\n");
	info->send_cnt++;
}

t_fping	*recv_packet(t_info *info)
{
	t_fping				*recv_pkt;
	struct sockaddr_in	src_addr;
	socklen_t			src_len;
	uint8_t				buf[1024];
	int					len;

	src_len = sizeof(src_addr);
	len = recvfrom(info->sock, buf, 1024, MSG_DONTWAIT, \
		(struct sockaddr *)&src_addr, &src_len);
	if (errno == EAGAIN && len == -1)
		return (NULL);
	else if (len == -1)
		error_handling("ft_ping: recvfrom failed\n");
	recv_pkt = (t_fping *)malloc(sizeof(t_fping));
	if (!recv_pkt)
		error_handling("ft_ping: malloc failed\n");
	memcpy(&recv_pkt->ip, buf, sizeof(recv_pkt->ip));
	memcpy(&recv_pkt->ping, &buf[20], sizeof(recv_pkt->ping));
	if (recv_pkt->ping.icmp.icmp_type != ICMP_ECHOREPLY)
	{
		free(recv_pkt);
		icmp_error(buf, info);
		return (NULL);
	}
	return (recv_pkt);
}

void	parse_packet(t_fping pkt, t_info *info)
{
	double	diff_time;

	if (pkt.ping.icmp.icmp_id != ntohs(info->pid))
		return ;
	if (info->prev_seq != ntohs(pkt.ping.icmp.icmp_seq))
		info->recv_cnt++;
	diff_time = diff_timeval(pkt.ping.time);
	printf("%ld byte from ", ntohs(pkt.ip.ip_len) - sizeof(pkt.ip));
	printf("%s: ", inet_ntoa(pkt.ip.ip_src));
	printf("icmp_seq: %d ", ntohs(pkt.ping.icmp.icmp_seq));
	printf("ttl=%d time=%.3lf ms", pkt.ip.ip_ttl, diff_time);
	if (info->prev_seq == ntohs(pkt.ping.icmp.icmp_seq))
		printf(" (DUP!)");
	printf("\n");
	info->prev_seq = ntohs(pkt.ping.icmp.icmp_seq);
	info->total_recv_cnt++;
	update_statistics(diff_time, info);
}
