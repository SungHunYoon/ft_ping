#ifndef _PING_H_
#define _PING_H_

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>

#define SWAP16(x) (((x >> 8) & 0xFF) | ((x << 8) & 0xFF00))

typedef struct s_ping {
	struct icmphdr	icmp;
	struct timeval	time;
	char			dummy[40];
} t_ping;

typedef struct s_fping {
	struct iphdr	ip;
	struct s_ping	ping;
} t_fping;

typedef struct s_info {
	int		opt;
	int		pid;
	char	host[16];
	char	ip[16];
	size_t	send_cnt;
	size_t	recv_cnt;
} t_info;

void	args_parsing(int argc, char **args, t_info *info);

void	ping_process(t_info *info);
void	ping_finish(t_info *info);

t_ping	make_packet(int seq);
void	send_packet(int sock, t_ping pkt, t_info *info);
t_fping	recv_packet(int sock);
void	parse_packet(t_fping pkt, t_info *info);

double	diff_timeval(struct timeval time);
char	*int_to_str_ip(uint32_t addr);
char	*domain_to_ip(char *domain);

#endif