#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

int main(int argc, char **args) {
	(void)argc;
	(void)args;
	int					rawSocket;
	struct sockaddr_in	destAddr;

	rawSocket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if(rawSocket < 0)
	{
		dprintf(2, "%s\n", "Create socket failed");
		return 1;
	}
	destAddr.sin_family = AF_INET;
	
	sendto();
	return 0;
}