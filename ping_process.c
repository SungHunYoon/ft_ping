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
	{
		dprintf(2, "Create socket failed\n");
		exit(1);
	}
	return raw_socket;
}

void	ping_process(t_info *info)
{
	int		raw_socket;
	t_ping	pkt;
	t_fping	recv_pkt;
	int		seq;

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
		recv_pkt = recv_packet(raw_socket);
		parse_packet(recv_pkt, info);
		seq++;
		sleep(1);
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
	printf("round-trip min/avg/max/stddev = ");
	printf("%.3lf/%.3lf/%.3lf/%.3lf ms\n", 1.0, 1.0, 1.0, 1.0);
}
