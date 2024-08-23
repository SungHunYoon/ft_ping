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
#include <errno.h>
#include <math.h>

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
	int		sock;
	int		opt;
	int		pid;
	char	*host;
	char	*ip;
	int		prev_seq;
	size_t	send_cnt;
	size_t	recv_cnt;
	size_t	total_recv_cnt;
	double	min;
	double	max;
	double	avg;
	double	m2;
	double	stddev;
} t_info;

int		host_parse(char *str, t_info *info);
void	args_parsing(int argc, char **args, t_info *info);

void	ping_process(t_info *info);
void	ping_finish(t_info *info);

t_ping	make_packet(int seq);
void	send_packet(t_ping pkt, t_info *info);
t_fping	*recv_packet(t_info *info);
void	parse_packet(t_fping pkt, t_info *info);

void	error_handling(char *str);
double	diff_timeval(struct timeval time);
char	*int_to_str_ip(uint32_t addr);
char	*domain_to_ip(char **domain);
char	*ip_to_domain(uint32_t addr);

void	update_statistics(double x, t_info *info);

void	icmp_error(uint8_t *buf, t_info *info);

#endif