/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sungyoon <sungyoon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/24 13:48:12 by sungyoon          #+#    #+#             */
/*   Updated: 2024/08/24 13:51:26 by sungyoon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ping.h"

void	error_handling(char *str)
{
	dprintf(2, "%s", str);
	exit(1);
}

double	diff_timeval(struct timeval time)
{
	double			ret;
	struct timeval	now;

	gettimeofday(&now, NULL);
	now.tv_sec -= time.tv_sec;
	now.tv_usec -= time.tv_usec;
	if (now.tv_usec < 0)
	{
		now.tv_usec += 1000000;
		now.tv_sec--;
	}
	ret = (now.tv_sec * 1000) + (now.tv_usec / 1000.0);
	return (ret);
}

char	*domain_to_ip(char **domain)
{
	char			*ret;
	struct hostent	*host;
	int				i;

	host = gethostbyname(*domain);
	if (!host)
		error_handling("ft_ping: unknown host\n");
	ret = NULL;
	free(*domain);
	*domain = strdup(host->h_name);
	i = 0;
	while (host->h_addr_list[i])
	{
		ret = inet_ntoa(*(struct in_addr *)host->h_addr_list[i]);
		break ;
	}
	return (ret);
}

char	*ip_to_domain(struct in_addr addr)
{
	struct hostent	*host;

	host = gethostbyaddr(&addr, sizeof(addr), AF_INET);
	if (!host)
		return (NULL);
	return (host->h_name);
}
