/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping_process.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sungyoon <sungyoon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/24 13:48:34 by sungyoon          #+#    #+#             */
/*   Updated: 2024/08/24 19:17:32 by sungyoon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ping.h"

volatile sig_atomic_t	g_stop;

static void	sigint_handler(int signo)
{
	(void)signo;
	g_stop = 1;
}

static void	raw_socket_open(t_info *info)
{
	int			broadcast;
	socklen_t	optlen;
	uint8_t		ttl;

	info->sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (info->sock < 0)
		error_handling("ft_ping: create socket failed\n");
	broadcast = 1;
	optlen = sizeof(broadcast);
	if (setsockopt(info->sock, SOL_SOCKET, SO_BROADCAST,
			&broadcast, optlen) < 0)
		error_handling("ft_ping: set socket opt broadcast failed\n");
	ttl = 64;
	optlen = sizeof(ttl);
	if (setsockopt(info->sock, IPPROTO_IP, IP_TTL, &ttl, optlen) < 0)
		error_handling("ft_ping: set socket opt broadcast failed\n");
}

static void	ping_recv_proc(t_info *info, struct timeval time)
{
	t_fping			recv_pkt;
	struct timeval	t;
	double			diff;
	size_t			tmp;

	diff = diff_timeval(time);
	while (diff <= 1000.0 && !g_stop)
	{
		tmp = (1000000 - (size_t)(diff * 1000)) / 2;
		t.tv_sec = tmp / 1000000;
		t.tv_usec = tmp % 1000000;
		if (setsockopt(info->sock, SOL_SOCKET, SO_RCVTIMEO,
				&t, sizeof(t)) < 0)
			error_handling("ft_ping: set socket opt rcvtimeo failed\n");
		if (recv_packet(&recv_pkt, info) == 0)
			parse_packet(recv_pkt, info);
		diff = diff_timeval(time);
	}
}

void	ping_process(t_info *info)
{
	t_ping			pkt;
	struct timeval	time;
	int				seq;

	signal(SIGINT, sigint_handler);
	raw_socket_open(info);
	printf("PING %s (%s): ", info->host, info->ip);
	printf("%ld data bytes", sizeof(pkt) - 8);
	if (info->opt == 1)
		printf(", id 0x%x = %d", info->pid, info->pid);
	printf("\n");
	seq = 0;
	while (!g_stop || seq == 0)
	{
		pkt = make_packet(seq);
		send_packet(pkt, info);
		gettimeofday(&time, NULL);
		ping_recv_proc(info, time);
		seq++;
	}
}

void	ping_finish(t_info *info)
{
	fflush(stdout);
	printf("--- %s ping statistics ---\n", info->host);
	printf("%zu packets transmitted, ", info->send_cnt);
	printf("%zu packets received, ", info->recv_cnt);
	if (info->total_recv_cnt - info->recv_cnt)
		printf("+%zu duplicates, ", info->total_recv_cnt - info->recv_cnt);
	printf("%d%% packet loss\n",
		(int)((info->send_cnt - info->recv_cnt) * 100 / info->send_cnt));
	if (info->recv_cnt)
	{
		printf("round-trip min/avg/max/stddev = ");
		printf("%.3lf/%.3lf/%.3lf/%.3lf ms\n",
			info->min, info->avg, info->max, info->stddev);
	}
}
