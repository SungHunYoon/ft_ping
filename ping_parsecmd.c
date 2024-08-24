/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping_parsecmd.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sungyoon <sungyoon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/24 13:48:19 by sungyoon          #+#    #+#             */
/*   Updated: 2024/08/24 13:49:13 by sungyoon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ping.h"

static void	help_message(void)
{
	printf("Usage: ft_ping [OPTION...] HOST ...\n");
	printf("Send ICMP ECHO_REQUEST packets to network hosts.\n\n");
	printf(" Options valid\n\n");
	printf("  -v, \tverbose output\n");
	printf("  -?, \tgive this help list\n\n");
	exit(0);
}

static void	invalid_option(char c)
{
	printf("ft_ping: invalid option -- '%c'\n", c);
	printf("Try 'ft_ping -?' for more information.\n");
	exit(1);
}

static void	option_parse(int argc, char **args, t_info *info)
{
	int		i;
	char	*c;

	i = 1;
	while (i < argc)
	{
		if (strlen(args[i]) >= 2 && args[i][0] == '-')
		{
			c = args[i] + 1;
			while (*c)
			{
				if (*c == 'v')
					info->opt = 1;
				else if (*c == '?')
					help_message();
				else
					invalid_option(*c);
				c++;
			}
		}
		i++;
	}
}

int	host_parse(char *str, t_info *info)
{
	if (strlen(str) >= 2 && str[0] == '-')
		return (0);
	free(info->host);
	free(info->ip);
	info->host = strdup(str);
	info->ip = strdup(domain_to_ip(&info->host));
	if (!info->host || !info->ip)
		error_handling("ft_ping: malloc failed\n");
	info->prev_seq = -1;
	info->send_cnt = 0;
	info->recv_cnt = 0;
	info->total_recv_cnt = 0;
	info->min = -1;
	info->max = 0;
	info->avg = 0;
	info->m2 = 0;
	info->stddev = 0;
	return (1);
}

void	args_parsing(int argc, char **args, t_info *info)
{
	if (getuid() != 0)
		error_handling("ft_ping: please check root privilege.\n");
	option_parse(argc, args, info);
	info->pid = getpid();
}
