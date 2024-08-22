#include "ping.h"

void	help_message(void)
{
	printf("Usage: ft_ping [OPTION...] HOST ...\n");
	printf("Send ICMP ECHO_REQUEST packets to network hosts.\n\n");
	printf(" Options valid\n\n");
	printf("  -v, \tverbose output\n");
	printf("  -?, \tgive this help list\n\n");
	exit(0);
}

void	invalid_option(char c)
{
	printf("ft_ping: invalid option -- '%c'\n", c);
	printf("Try 'ft_ping -?' for more information.\n");
	exit(1);
}

void	option_parse(int argc, char **args, t_info *info)
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
		else if (info->host == NULL)
			info->host = strdup(args[i]);
		i++;
	}
}

void	args_parsing(int argc, char **args, t_info *info)
{
	option_parse(argc, args, info);
	if (!info->host)
	{
		dprintf(2, "ft_ping: missing host operand\n");
		dprintf(2, "Try 'ft_ping -?' for more information.\n");
		exit(1);
	}
	info->ip = strdup(domain_to_ip(info->host));
	if (!info->ip)
		error_handling("ft_ping: malloc failed\n");
	info->pid = getpid();
	if (getuid() != 0)
		error_handling("ft_ping: please check root privilege.\n");
}
