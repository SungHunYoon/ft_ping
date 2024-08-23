#include "ping.h"

double	calculate_variance(double m2, int n)
{
	if (n < 2)
		return 0.0;
	return (m2 / (n - 1));
}

double	calculate_stddev(double m2, int n)
{
	return (sqrt(calculate_variance(m2, n)));
}

void	update_statistics(double x, t_info *info)
{
	double	delta;
	
	delta = x - info->avg;
	if (info->min < 0|| info->min > x)
		info->min = x;
	if (info->max < x)
		info->max = x;
	info->avg += delta / info->total_recv_cnt;
	info->m2 += delta * (x - info->avg);
	info->stddev = calculate_stddev(info->m2, info->total_recv_cnt);
}
