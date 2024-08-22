#include "ping.h"

volatile sig_atomic_t g_stop;

void	sigint_handler(int signo)
{
	(void)signo;
	g_stop = 1;
}

int	raw_socket_open(void)
{
	int	raw_socket;

	raw_socket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if(raw_socket < 0)
		error_handling("ft_ping: create socket failed\n");
	return raw_socket;
}

void	ping_recv_proc(int sock, t_info *info, struct timeval time)
{
	t_fping	*recv_pkt;

	while (diff_timeval(time) <= 1000.0 && !g_stop)
	{
		recv_pkt = recv_packet(sock);
		if (!recv_pkt)
			continue;
		parse_packet(*recv_pkt, info);
	}
	return;
}

void	ping_process(t_info *info)
{
	int				raw_socket;
	t_ping			pkt;
	struct timeval	time;
	int				seq;

	signal(SIGINT, sigint_handler);
	raw_socket = raw_socket_open();
	printf("PING %s (%s): ", info->host, info->ip);
	printf("%ld data bytes", sizeof(pkt) - sizeof(pkt.icmp));
	if (info->opt == 1)
		printf(", id 0x%x = %d", info->pid, info->pid);
	printf("\n");
	seq = 0;
	while (!g_stop)
	{
		pkt = make_packet(seq);
		send_packet(raw_socket, pkt, info);
		gettimeofday(&time, NULL);
		ping_recv_proc(raw_socket, info, time);
		seq++;
	}
}

void	ping_finish(t_info *info)
{
	fflush(stdout);
	printf("--- %s ping statistics ---\n", info->host);
	printf("%zu packets transmitted, ", info->send_cnt);
	printf("%zu packets received, ", info->recv_cnt);
	printf("%d%% packet loss\n", \
		(int)((info->send_cnt - info->recv_cnt) * 100 / info->send_cnt));
	if (info->recv_cnt)
	{
		printf("round-trip min/avg/max/stddev = ");
		printf("%.3lf/%.3lf/%.3lf/%.3lf ms\n", \
			info->min, info->avg, info->max, info->stddev);
	}
}
