#include "ping.h"

uint16_t	calculate_checksum(uint16_t *data, int len)
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
	return (uint16_t)(~sum);
}

t_ping	make_packet(int seq)
{
	t_ping	pkt;
	int		i;

	memset(&pkt, 0, sizeof(pkt));
	pkt.icmp.type = ICMP_ECHO;
	pkt.icmp.code = 0;
	pkt.icmp.un.echo.id = SWAP16(getpid());
	pkt.icmp.un.echo.sequence = SWAP16(seq);
	gettimeofday(&pkt.time, NULL);
	i = 0;
	while (i < 40)
	{
		pkt.dummy[i] = i;
		i++;
	}
	pkt.icmp.checksum = calculate_checksum((uint16_t *)&pkt, sizeof(pkt));
	return pkt;
}

void	send_packet(t_ping pkt, t_info *info)
{
	struct sockaddr_in	dest_addr;
	int					len;

	memset(&dest_addr, 0, sizeof(dest_addr));
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_addr.s_addr = inet_addr(info->ip);
	len = sendto(info->sock, &pkt, sizeof(pkt), 0,\
		 (struct sockaddr *)&dest_addr, sizeof(dest_addr));
	if (len == -1) {
		dprintf(2, "errno : %d\n", errno);
		error_handling("ft_ping: sendto failed\n");
	}
	info->send_cnt++;
}

t_fping	*recv_packet(t_info *info)
{
	t_fping				*recv_pkt;
	struct sockaddr_in	src_addr;
	socklen_t			src_len;
	uint8_t				buf[1024];
	int 				len;

	src_len = sizeof(src_addr);
	len = recvfrom(info->sock, buf, 1024, MSG_DONTWAIT,\
		 (struct sockaddr *)&src_addr, &src_len);
	if (errno == EAGAIN && len == -1)
		return NULL;
	else if (len == -1)
		error_handling("ft_ping: recvfrom failed\n");
	recv_pkt = (t_fping *)malloc(sizeof(t_fping));
	if (!recv_pkt)
		error_handling("ft_ping: malloc failed\n");
	memcpy(&recv_pkt->ip, buf, sizeof(recv_pkt->ip));
	memcpy(&recv_pkt->ping, &buf[20], sizeof(recv_pkt->ping));
	if (recv_pkt->ping.icmp.type != 0)
	{
		free(recv_pkt);
		icmp_error(buf, len, info);
		return NULL;
	}
	return recv_pkt;
}

void	parse_packet(t_fping pkt, t_info *info)
{
	double	diff_time;

	if (pkt.ping.icmp.un.echo.id != SWAP16(info->pid))
		return;
	if (info->prev_seq != SWAP16(pkt.ping.icmp.un.echo.sequence))
		info->recv_cnt++;
	diff_time = diff_timeval(pkt.ping.time);
	printf("%ld byte from ", SWAP16(pkt.ip.tot_len) - sizeof(pkt.ip));
	printf("%s: ", int_to_str_ip(pkt.ip.saddr));
	printf("icmp_seq: %d ", SWAP16(pkt.ping.icmp.un.echo.sequence));
	printf("ttl=%d time=%.3lf ms", pkt.ip.ttl, diff_time);
	if (info->prev_seq == SWAP16(pkt.ping.icmp.un.echo.sequence))
		printf(" (DUP!)");
	printf("\n");
	info->prev_seq = SWAP16(pkt.ping.icmp.un.echo.sequence);
	info->total_recv_cnt++;
	update_statistics(diff_time, info);
}
