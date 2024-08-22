#include "ping.h"

// 92 bytes from debian (10.12.250.77): Destination Host Unreachable
// IP Hdr Dump:
// 4500 0054 4f2b 4000 4001 d217 0a0c fa4d 0a0c 0b01 
// Vr HL TOS  Len   ID Flg  off TTL Pro  cks      Src      Dst     Data
// 4  5  00 0054 4f2b   2 0000  40  01 d217 10.12.250.77  10.12.11.1 
// ICMP: type 8, code 0, size 64, id 0x1a59, seq 0x0000

// 36 bytes from _gateway (10.0.2.2): Time to live exceeded
// IP Hdr Dump:
//  4500 0054 b702 4000 0101 aa8c 0a00 020f 0808 0404 
// Vr HL TOS  Len   ID Flg  off TTL Pro  cks      Src      Dst     Data
//  4  5  00 0054 b702   2 0000  01  01 aa8c 10.0.2.15  8.8.4.4 
// ICMP: type 8, code 0, size 64, id 0x09a2, seq 0x0000

void	unreachable_dump(struct iphdr ip_pkt, struct icmphdr icmp_pkt, \
						t_fping ping_pkt, t_info *info)
{
	(void)ip_pkt;
	(void)icmp_pkt;
	if (SWAP16(ping_pkt.ping.icmp.un.echo.id) != info->pid)
		return;
	printf("test\n");
}

void	timeexceed_dump(struct iphdr ip_pkt, struct icmphdr icmp_pkt, \
					t_fping ping_pkt, t_info *info)
{
	(void)ip_pkt;
	(void)icmp_pkt;
	if (SWAP16(ping_pkt.ping.icmp.un.echo.id) != info->pid)
		return;
	printf("test\n");
}

void	icmp_error(uint8_t *buf, int len, t_info *info)
{
	struct iphdr	ip_pkt;
	struct icmphdr	icmp_pkt;
	t_fping			ping_pkt;

	memcpy(&ip_pkt, &buf, sizeof(struct iphdr));
	memcpy(&icmp_pkt, &buf[sizeof(struct iphdr)], sizeof(struct icmphdr));
	memcpy(&ping_pkt.ip, \
		&buf[sizeof(struct iphdr) + sizeof(struct icmphdr)], \
		sizeof(struct iphdr));
	memcpy(&ping_pkt.ping, \
		&buf[sizeof(struct iphdr) * 2 + sizeof(struct icmphdr)], \
		sizeof(t_ping));
	printf("------- dump -------\n");
	for (int i = 0; i < len; i++)
		printf("%02X ", buf[i]);
	printf("\n");
	if (icmp_pkt.type == 3)
		unreachable_dump(ip_pkt, icmp_pkt, ping_pkt, info);
	else if (icmp_pkt.type == 11)
		timeexceed_dump(ip_pkt, icmp_pkt, ping_pkt, info);
}
