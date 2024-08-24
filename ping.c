/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sungyoon <sungyoon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/24 13:48:36 by sungyoon          #+#    #+#             */
/*   Updated: 2024/08/24 13:48:37 by sungyoon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ping.h"

static void	usage_error(void)
{
	dprintf(2, "ft_ping: missing host operand\n");
	dprintf(2, "Try 'ft_ping -?' for more information.\n");
	exit(1);
}

int	main(int argc, char **args)
{
	t_info	*info;
	int		i;
	int		flag;

	info = (t_info *)malloc(sizeof(t_info));
	memset(info, 0, sizeof(t_info));
	args_parsing(argc, args, info);
	i = 1;
	flag = 0;
	while (i < argc)
	{
		if (host_parse(args[i], info))
		{
			ping_process(info);
			ping_finish(info);
			flag = 1;
		}
		i++;
	}
	if (flag == 0)
		usage_error();
	exit(0);
}
