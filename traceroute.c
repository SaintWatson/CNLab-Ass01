#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<errno.h>
#include<netinet/in.h>
#include<netinet/ip.h>
#include<netinet/ip_icmp.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<sys/time.h>

typedef struct sockaddr     SA;
typedef struct sockaddr_in  SAin;
typedef struct icmp         icmp;
typedef struct icmphdr      icmphdr;
typedef struct timeval      timeval;
typedef unsigned short      US;
#define BUF_SIZE 4096
#define ZERO(a) do{memset(&a, 0,sizeof(a));}while(0)

char *DNSLookup(char *host){    

    struct addrinfo hints, *info;

    // configure addrinfo
    ZERO(hints);
    hints.ai_family = AF_UNSPEC; 
    hints.ai_socktype = SOCK_STREAM;

    if(getaddrinfo( host , NULL, &hints , &info) < 0){
        fprintf(stderr, "Error: getaddrinfo failed(%d)\n", errno);
        exit(0) ;
    }

    SAin *addr = (SAin *) info->ai_addr;
    char *str_addr = inet_ntoa(addr->sin_addr);  
    freeaddrinfo(info); 

    return str_addr;
}
US checksum(US buffer[BUF_SIZE], int len){
    unsigned int sum = 0;
    for (int i=0 ; i<len ; i++)
        sum +=  (unsigned int) buffer[i];
    sum = sum/(1<<16) + sum%(1<<16);
    return ~sum;
}

int main(int argc, char *argv[]){

    if(argc != 2){
        printf("Usage: ./trace <IP addr>\n");
        exit(1);
    }
    
    char *ip = DNSLookup(argv[1]);
    if(ip == NULL){
        printf("traceroute: unknown host %s\n", argv[1]);
        exit(1);
    }
    
    int icmpfd;
    if((icmpfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0){
        printf("Can not open socket with error number %d\n", errno);
        exit(1);
    }
    
    SAin sendAddr;
    ZERO(sendAddr);
    sendAddr.sin_family = AF_INET;
    sendAddr.sin_port = htons(7);
    inet_pton(AF_INET, ip, &sendAddr.sin_addr);
    

    // [TODO]: Set timeout
    timeval timeout = {3,0}; // custumize 3 sec.
    setsockopt(icmpfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));

    int finish = 0; // if the packet reaches the destination
    int maxHop = 64; // maximum hops
    icmp sendICMP; 
    timeval begin, end; // used to record RTT
    int seq = 0; // increasing sequence number for icmp packet
    int count = 3; // sending count for each ttl
    int ret; // return value of some system call
    char hostname[128];
    char srcIP[32];
    printf("traceroute to %s (%s), %d hops max\n", argv[1], ip, maxHop);

    for(int h = 1; h < maxHop; h++){
        
        float interval[4] = {};

        
        for(int c = 0; c < count; c++){
            
            if(strcmp(ip,"127.0.0.1")==0){
                finish = 1;
            }
            // [TODO] : Set TTL 
            setsockopt(icmpfd, IPPROTO_IP, IP_TTL, &h, sizeof(h));

            // [TODO] : Set ICMP Header
            ZERO(sendICMP);
            sendICMP.icmp_type = ICMP_ECHO;
            sendICMP.icmp_code = 0;
            sendICMP.icmp_id = seq++;
            sendICMP.icmp_seq = h;
            sendICMP.icmp_cksum = 0;

            // [TODO] : Checksum
            sendICMP.icmp_cksum = checksum((US*)&sendICMP, 4);

            // [TODO] : Send the icmp packet to destination
            gettimeofday(&begin, NULL);
            ret = sendto(icmpfd, &sendICMP, sizeof(sendICMP), 0, (SA*)&sendAddr, sizeof(sendAddr));
            if(ret < 0){
                printf("Error: sendto() is failed with errno:%d\n", errno);
                exit(1);
            }

            // Recive ICMP reply, need to check the identifier and sequence number
            icmp *recvICMP = (icmp*) malloc(sizeof(icmp));
            SAin recvAddr;  
            u_int8_t icmpType;
            unsigned int recvLength = sizeof(recvAddr);
            char recvBuf[1500];
            int isTimeout = 0;
            
            ZERO(recvBuf);
            ZERO(recvAddr);

            // [TODO] : Get source hostname and ip address 
            ret = recvfrom(icmpfd, recvBuf, sizeof(recvBuf), 0, (SA*)&recvAddr, &recvLength);
            gettimeofday(&end, NULL);

            if(ret < 0){
                if(errno == 11){
                    isTimeout = 1;
                }
                else{
                    printf("Error: recvfrom() is failed with errno:%d\n", errno);
                    exit(1);
                }
            }

            recvICMP = (icmp*) (recvBuf+20);
            icmpType = recvICMP->icmp_type;
            // printf("(%d) type: %d code: %d\n",c, recvICMP->icmp_type, recvICMP->icmp_code);

            if(isTimeout == 1){
                interval[c] = -1;
            } 
            else{
                interval[c] = 0;
                interval[c] += (float) (end.tv_usec - begin.tv_usec) / 1000;
                interval[c] += (float) (end.tv_sec - begin.tv_sec) * 1000;
            }


            // Get source hostname and ip address 
            ZERO(hostname[c]);
            getnameinfo((struct sockaddr *)&recvAddr, sizeof(recvAddr), hostname, sizeof(hostname), NULL, 0, 0); 
            if(icmpType == 0)
                finish = 1;
            
        }
        // [TODO] : Print the result
        char timecost[3][10];
        for(int i=0;i<3;i++){
            if(interval[i] == -1)
                sprintf(timecost[i], "Timeout.");
            else
                sprintf(timecost[i], "%.3fms", interval[i]);
        }

        char output_header[30];
        strcpy(output_header, hostname);
        if(strlen(output_header)==0)
            strcpy(output_header, srcIP);
        if(strlen(output_header)==0)
            strcpy(output_header, "*");


        printf("%d %-30s %-8s  %-8s  %-8s\n", h, output_header, timecost[0], timecost[1], timecost[2]);
        

        if(finish)
            break;
        
    }
    close(icmpfd);
    return 0;
}