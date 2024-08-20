#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/icmp.h>

int main(int argc, char **args) {
	(void)argc;
	(void)args;
	int	rawSocket;

	rawSocket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if(rawSocket < 0) {
		dprintf(2,"Create socket failed\n");
		return 1;
	}

	struct sockaddr_in destAddr;
	char data[] = "test1234test";
	memset(&destAddr, 0, sizeof(destAddr));
	destAddr.sin_family = AF_INET;
	destAddr.sin_addr.s_addr = htonl(inet_addr("8.8.8.8"));
	sendto(rawSocket, data, 12, 0 , (struct sockaddr *)&destAddr, sizeof(destAddr));
	printf("send OK\n");	

	struct sockaddr_in srcAddr;
	socklen_t srcLen;
	int len;
	char buf[1024];
	if ((len = recvfrom(rawSocket, buf, 1024, 0, (struct sockaddr *)&srcAddr, &srcLen)) == -1) {
		dprintf(2, "recvfrom failed\n");
		return 1;
	}
	for (int i = 0; i < len; i++)
		printf("%02X ", buf[i]);
	printf("\n");
	return 0;
}