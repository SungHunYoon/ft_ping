#include "ping.h"

void	help_message(void)
{
	printf("Usage: ft_ping [OPTION...] HOST ...\n");
	printf("Send ICMP ECHO_REQUEST packets to network hosts.\n\n");
	printf(" Options valid\n\n");
	printf("  -v, \tverbose output\n");
	printf("  -?, \tgive this help list\n");
	exit(0);
}

void	invalid_option(char c)
{
	printf("ft_ping: invalid option -- '%c'\n", c);
	printf("Try 'ft_ping -?' for more information.\n");
	exit(1);
}

void	args_parsing(int argc, char **args, t_info *info)
{
	(void)args;
	if (argc == 1)
	{
		dprintf(2, "ft_ping: missing host operand\n");
		dprintf(2, "Try 'ft_ping -?' for more information.\n");
		exit(1);
	}
	strcpy(info->ip, "8.8.8.8");
	info->pid = getpid();
	if (info->pid != 0)
	{
		dprintf(2, "Please check root privilege.\n");
		exit(1);
	}
}