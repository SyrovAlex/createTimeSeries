//Program for monitoring incoming node traffic.05/12/2020
/*
Program arguments:
- (1) name of the file where traffic monitoring results should be saved (format for saving data - see file "format_saving_data.jpeg");
- (2) monitoring duration (seconds);
- (3) IP address of the node that is being monitored.
*/
#include <sys/socket.h>
#include <features.h> /* for the glibc version number */
#if __GLIBC__ >= 2 && __GLIBC_MINOR >= 1 
#include <netpacket/packet.h>
#include <net/ethernet.h>       /* the L2 protocols */
#else
#include <asm/types.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h> /* The L2 protocols */ 
#endif
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h>
#include <net/ethernet.h>
#include <netinet/tcp.h>
#include <ctype.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <signal.h>
#include <string.h>

//Structure for the Ethernet header
struct ether_hdr{
	unsigned char ether_dest_addr[ETHER_ADDR_LEN];
	unsigned char ether_src_addr[ETHER_ADDR_LEN];
	unsigned short ether_type;
};

//Structure for the IP header
struct ip_hdr{
	unsigned char ip_version_and_header_length;
	unsigned char ip_tos;
	unsigned short ip_len;
	unsigned short ip_id;
	unsigned short ip_frag_offset;
	unsigned char ip_ttl;
	unsigned char ip_type;
	unsigned short ip_checksum;
	struct in_addr ip_src_addr;
	struct in_addr ip_dest_addr;
};

//Structure for the TCP header
struct tcp_hdr{
	unsigned short tcp_src_port;
	unsigned short tcp_dest_port;
	unsigned int tcp_seq;
	unsigned int tcp_ack;
	unsigned char reserved:4;
	unsigned char tcp_offset:4;
	unsigned char tcp_flags;
	unsigned short tcp_window;
	unsigned short tcp_checksum;
	unsigned short tcp_urgent;
};

int currentTime=0;

void catcher(int signum){
	if(signum==SIGALRM) currentTime++;
}

int main(int argc, char **argv){
	FILE *fd;
	int limitTime=atoi(argv[2]);//argument 2
	int countFrame=0;
	struct timeval timeRecv;
	fd=fopen(argv[1],"w");//argument 1
	struct in_addr addar;
	inet_aton(argv[3], &addar);//argument 3
	int sizeRecv;
	int sockRecv;
	sockRecv=socket(AF_PACKET,SOCK_RAW, htons(ETH_P_ALL));
	unsigned char packetRecv[65565];
	struct ether_hdr *headerEther;
	struct ip_hdr *headerIp;
	struct tcp_hdr *headerTcp;
	struct itimerval intervalTimer;
	intervalTimer.it_value.tv_sec=1;
	intervalTimer.it_value.tv_usec=0;
	intervalTimer.it_interval.tv_sec=1;
	intervalTimer.it_interval.tv_usec=0;
	setitimer(ITIMER_REAL,&intervalTimer,NULL);
	struct sigaction act;
	memset(&act,0,sizeof(act));
	act.sa_handler=&catcher;
	sigaction(SIGALRM,&act,NULL);
	fprintf(fd,"Count Sec mSec ID Len SrcPort DestPort Flags\n");
	while(currentTime<limitTime){
		sizeRecv = recvfrom(sockRecv, packetRecv, 65565, 0, NULL, NULL);
		gettimeofday(&timeRecv, NULL);
		headerEther=(struct ether_hdr *)packetRecv;
		if(ntohs(headerEther-> ether_type)!=0x0800) continue;
		headerIp=(struct ip_hdr *)(packetRecv+sizeof(struct ether_hdr));
		if(headerIp->ip_dest_addr.s_addr!=addar.s_addr) continue;
		if(headerIp->ip_type!=0x06) continue;
		headerTcp=(struct tcp_hdr *)(packetRecv+sizeof(struct ether_hdr)+sizeof(struct ip_hdr));
		countFrame++;
		//Output data to the monitor
		printf("Count:%d Sec:%ld mSec:%ld ID:%d Len:%d SrcPort:%d DestPort:%d Flags:%d\n",
				countFrame,//Sequence number of the frame
				timeRecv.tv_sec,//Time when the frame was received, seconds
				timeRecv.tv_usec,//Time when the frame was received, microseconds 
				ntohs(headerIp->ip_id),//Frame identifier
				ntohs(headerIp->ip_len),//IP size
				ntohs(headerTcp->tcp_src_port),//The source port of the frame
				ntohs(headerTcp->tcp_dest_port),//The destination port of the frame
				headerTcp->tcp_flags);//Frame flags

		//Saving data to a file
		fprintf(fd, "%d %ld %ld %d %d %d %d %d\n",
                                countFrame,//Sequence number of the frame
                                timeRecv.tv_sec,//Time when the frame was received, seconds
                                timeRecv.tv_usec,//Time when the frame was received, microseconds 
                                ntohs(headerIp->ip_id),//Frame identifier
                                ntohs(headerIp->ip_len),//IP size
                                ntohs(headerTcp->tcp_src_port),//The source port of the frame
                                ntohs(headerTcp->tcp_dest_port),//The destination port of the frame
                                headerTcp->tcp_flags);//Frame flags
			}

	fclose(fd);
	return 0;
}

