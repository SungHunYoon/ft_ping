#include "ping.h"

int	main(int argc, char **args)
{
	t_info	*info;

	info = (t_info *)malloc(sizeof(t_info));
	memset(info, 0, sizeof(t_info));
	args_parsing(argc, args, info);
	ping_process(info);
	ping_finish(info);
	exit(0);
}
