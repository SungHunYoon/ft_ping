#include "ping.h"

// 92 bytes from debian (10.12.250.77): Destination Host Unreachable
// IP Hdr Dump:
//  4500 0054 4da5 4000 4001 d39d 0a0c fa4d 0a0c 0b01 
// Vr HL TOS  Len   ID Flg  off TTL Pro  cks      Src      Dst     Data
//  4  5  00 0054 4da5   2 0000  40  01 d39d 10.12.250.77  10.12.11.1 
// ICMP: type 8, code 0, size 64, id 0x0dde, seq 0x0003

void	print_pkt_info(t_fping pkt)
{
	char	*src;
	char	*dst;

	src = strdup(int_to_str_ip(pkt.ip.saddr));
	dst = strdup(int_to_str_ip(pkt.ip.daddr));
	printf("\nVr HL TOS  LEN   ID Flg  Off TTL ");
	printf("Pro  cks      Src      Dst     Data\n");
	printf("%2d %2d  %02x %04x %04x %3d %04x  %02x  %02x %04x %s  %s\n",
		pkt.ip.version, pkt.ip.ihl, pkt.ip.tos, SWAP16(pkt.ip.tot_len),
		pkt.ip.id, (pkt.ip.frag_off & 0xE0) >> 5, 
		SWAP16(pkt.ip.frag_off) & 0x1F,
		pkt.ip.ttl, pkt.ip.protocol, SWAP16(pkt.ip.check),
		src, dst);
	printf("ICMP: type %d, code %d, size %zu, id 0x%04x, seq 0x%04x\n",
		pkt.ping.icmp.type, pkt.ping.icmp.code, 
		sizeof(pkt.ping), 
		SWAP16(pkt.ping.icmp.un.echo.id), 
		SWAP16(pkt.ping.icmp.un.echo.sequence));
	free(src);
	free(dst);
}

void	common_dump(t_fping pkt)
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

void	unreachable_dump(struct iphdr ip_pkt, struct icmphdr icmp_pkt, \
						t_fping ping_pkt, t_info *info)
{
	if (SWAP16(ping_pkt.ping.icmp.un.echo.id) != info->pid)
		return;
	printf("%lu bytes from %s (%s): ", 
		SWAP16(ip_pkt.tot_len) - sizeof(struct iphdr),
		ip_to_domain(ip_pkt.daddr), int_to_str_ip(ip_pkt.daddr));
	if (icmp_pkt.code == 1)
		printf("Destination Host Unreachable");
	else if (icmp_pkt.code == 3)
		printf("Destination Port Unreachable");
	else if (icmp_pkt.code == 4)
		printf("Fragmentation Required but DF bit is set");
	else if (icmp_pkt.code == 13)
		printf("Forbidden");
	printf("\n");
	if (info->opt)
		common_dump(ping_pkt);
}

void	timeexceed_dump(struct iphdr ip_pkt, struct icmphdr icmp_pkt, \
					t_fping ping_pkt, t_info *info)
{
	if (SWAP16(ping_pkt.ping.icmp.un.echo.id) != info->pid)
		return;
	printf("%lu bytes from %s (%s): ",
		SWAP16(ip_pkt.tot_len) - sizeof(struct iphdr),
		ip_to_domain(ip_pkt.daddr), int_to_str_ip(ip_pkt.daddr));
	if (icmp_pkt.code == 0)
		printf("Time to Live Exceeded");
	else if (icmp_pkt.code == 1)
		printf("Fragment Reassembly Time Exceeded");
	printf("\n");
	if (info->opt)
		common_dump(ping_pkt);
}

void	icmp_error(uint8_t *buf, t_info *info)
{
	struct iphdr	ip_pkt;
	struct icmphdr	icmp_pkt;
	t_fping			ping_pkt;

	memcpy(&ip_pkt, buf, sizeof(struct iphdr));
	memcpy(&icmp_pkt, &buf[sizeof(struct iphdr)], sizeof(struct icmphdr));
	memcpy(&ping_pkt.ip, \
		&buf[sizeof(struct iphdr) + sizeof(struct icmphdr)], \
		sizeof(struct iphdr));
	memcpy(&ping_pkt.ping, \
		&buf[sizeof(struct iphdr) * 2 + sizeof(struct icmphdr)], \
		sizeof(t_ping));
	if (icmp_pkt.type == 3)
		unreachable_dump(ip_pkt, icmp_pkt, ping_pkt, info);
	else if (icmp_pkt.type == 11)
		timeexceed_dump(ip_pkt, icmp_pkt, ping_pkt, info);
}
