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

void	send_packet(int sock, t_ping pkt, t_info *info)
{
	struct sockaddr_in	dest_addr;

	memset(&dest_addr, 0, sizeof(dest_addr));
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_addr.s_addr = htonl(inet_addr(info->ip));
	sendto(sock, &pkt, sizeof(pkt), 0 , (struct sockaddr *)&dest_addr, sizeof(dest_addr));
	info->send_cnt++;
}

t_fping	recv_packet(int sock)
{
	t_fping				recv_pkt;
	struct sockaddr_in	src_addr;
	socklen_t			src_len;
	uint8_t				buf[1024];
	int 				len;

	len = recvfrom(sock, buf, 1024, 0, (struct sockaddr *)&src_addr, &src_len);
	if (len == -1)
	{
		dprintf(2, "recvfrom failed\n");
		exit(1);
	}
	memcpy(&recv_pkt, buf, sizeof(recv_pkt.ip));
	memcpy(&recv_pkt.ping, &buf[20], sizeof(recv_pkt.ping));
	return recv_pkt;
}

void	parse_packet(t_fping pkt, t_info *info)
{
	if (pkt.ping.icmp.type != 0)
	{
		dprintf(2, "icmp proto error\n");
		return;
	}
	printf("%ld byte from ", SWAP16(pkt.ip.tot_len) - sizeof(pkt.ip));
	printf("%s: ", int_to_str_ip(pkt.ip.saddr));
	printf("icmp_seq: %d ", SWAP16(pkt.ping.icmp.un.echo.sequence));
	printf("ttl=%d time=%.3lf ms\n", pkt.ip.ttl, diff_timeval(pkt.ping.time));
	info->recv_cnt++;
}
