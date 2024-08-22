#include "ping.h"

void	error_handling(char *str)
{
	dprintf(2, str);
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
	return ret;
}

char	*int_to_str_ip(uint32_t addr)
{
	char			*ret;
	struct in_addr	inaddr;

	inaddr.s_addr = addr;
	ret = inet_ntoa(inaddr);
	return ret;
}

char	*domain_to_ip(char *domain)
{
	char			*ret;
	struct hostent	*host;
	int				i;

	host = gethostbyname(domain);
	if (!host)
		error_handling("ft_ping: unknown host\n");
	ret = NULL;
	free(domain);
	domain = strdup(host->h_name);
	i = 0;
	while (host->h_addr_list[i])
	{
		ret = inet_ntoa(*(struct in_addr *)host->h_addr_list[i]);
		break;
	}
	return ret;
}
