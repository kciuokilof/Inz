#include        <sys/types.h>   /* basic system data types */
#include        <sys/socket.h>  /* basic socket definitions */
#if TIME_WITH_SYS_TIME
#include        <sys/time.h>    /* timeval{} for select() */
#include        <time.h>                /* timespec{} for pselect() */
#else
#if HAVE_SYS_TIME_H
#include        <sys/time.h>    /* includes <time.h> unsafely */
#else
#include        <time.h>                /* old system? */
#endif
#endif
#include        <netinet/in.h>  /* sockaddr_in{} and other Internet defns */
#include        <arpa/inet.h>   /* inet(3) functions */
#include        <errno.h>
#include        <fcntl.h>               /* for nonblocking */
#include        <netdb.h>
#include        <signal.h>
#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>
#include	<netinet/tcp.h>		/* for TCP_MAXSEG */

#define SA      struct sockaddr
#define MAXLINE 2048
#define	SENSORID 1
#define enTTL 300
#define EnUnitSize 5
writen(int fd, const void *vptr, size_t n)
{
	size_t		nleft;
	ssize_t		nwritten;
	const char	*ptr;

	ptr = vptr;
	nleft = n;
	while (nleft > 0) {
		if ( (nwritten = write(fd, ptr, nleft)) <= 0) {
			if (nwritten < 0 && errno == EINTR)
				nwritten = 0;		/* and call write() again */
			else
				return(-1);			/* error */
		}

		nleft -= nwritten;
		ptr   += nwritten;
	}
	return(n);
}

void
Writen(int fd, void *ptr, size_t nbytes)
{
	if (writen(fd, ptr, nbytes) != nbytes)
		perror("writen error");
}

SendingTemerature(sockfd,sensorid){
	int			sendint[EnUnitSize],recvint[EnUnitSize],temperature,ttl,initnum[4];
	int			n;
	FILE		*fp;
	initnum[4]='\0';
	fp = fopen ("ininum.txt","r");
	fscanf (fp, "%d", &ttl);
	for(n=0;n<4;n++){
		fscanf (fp, ",%d", &initnum[n]);
		printf ("%d init number: :%d\n",n,initnum[n]);
	}
	fclose(fp);
	
	srand (time(NULL));
	sendint[10]='\0';
	for(n=0; n<EnUnitSize;n++){
		sendint[n]=rand();
		printf("zapycham bufor : %d\n",sendint[n]);
		
		}
		temperature=69;
		
		sendint[0]=sensorid;
		
		sendint[1]=temperature;
	Writen(sockfd, sendint, sizeof(int)*EnUnitSize );
	//Początek wymiany danych
	while ( (n = read(sockfd, recvint, sizeof(int)*EnUnitSize)) > 0) {
		recvint[n] = '\0';	/* null terminate */
		printf("\nConnection with central unit %d\ninitialization number: %d %d %d %d\n", recvint[0],recvint[1],recvint[2],recvint[3],recvint[4]);
		if(n==sizeof(int)*EnUnitSize)
			break;
	}
	
	
	
	}
int InitCommunication(sockfd,sensorid){

	int			sendint[10],recvint[EnUnitSize];
	ssize_t		n;
	FILE		*fp;
	char		sendline[70];
	srand (time(NULL));
	for(n=0; n<10;n++){
		sendint[n]=rand();
		printf("zapycham bufor : %d\n",sendint[n]);
		
		}
		sendint[0]=sensorid;
		sendint[10]='\0';
	Writen(sockfd, sendint, sizeof(int)*10 );
	//Początek wymiany danych
	while ( (n = read(sockfd, recvint, sizeof(int)*EnUnitSize)) > 0) {
		recvint[n] = '\0';	/* null terminate */
		printf("\nConnection with central unit %d\ninitialization number: %d %d %d %d\n", recvint[0],recvint[1],recvint[2],recvint[3],recvint[4]);
		if(n==sizeof(int)*EnUnitSize)
			break;
	}
	fp = fopen ("ininum.txt","w");
  		if (fp!=NULL){
  			snprintf(sendline, sizeof(sendline),"%d,%d,%d,%d,%d",enTTL, recvint[1], recvint[2], recvint[3], recvint[4]);
    		fputs (sendline,fp);
    		fclose (fp);
  		}
  		else{
  			perror("openfile error"); 		
  			return 0;
  		}


}



int
main(int argc, char **argv)
{
	int					a,sensorid,key[4],ininumbers[4], sockfd, rcvbuf, mss,recvint[EnUnitSize];
	socklen_t			len;
	struct sockaddr_in6	servaddr;
	char				recvline[MAXLINE + 1],sendline[MAXLINE + 1];
	int err,n;
	struct timeval start, stop;
	FILE * fp;
	if (argc != 3){
		fprintf(stderr, "ERROR: usage: %s <IPv6 address> <0-1>\n", argv[0]);
		return 1;
	}

	fp = fopen ("sensor.conf","r");
	fscanf (fp, "%d,", &sensorid);
	for(n=0;n<4;n++){
		fscanf (fp, "%d,", &key[n]);
		printf ("%d klucz :%d\n",n,key[n]);
	}
	fclose(fp);
	key[4]=0;


	if ( (sockfd = socket(AF_INET6, SOCK_STREAM, 0)) < 0){
                fprintf(stderr,"socket error : %s\n", strerror(errno));
                return 1;
    }

	len = sizeof(rcvbuf);
	if( getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &rcvbuf, &len) == -1 )
	{	
		fprintf(stderr,"getsockopt error : %s\n", strerror(errno));
		return 2;
	}
	len = sizeof(mss);
	if( getsockopt(sockfd, IPPROTO_TCP, TCP_MAXSEG, &mss, &len) == -1 ){
		fprintf(stderr,"getsockopt error : %s\n", strerror(errno));
		return 3;
	}
	printf("defaults: SO_RCVBUF = %d, MSS = %d\n", rcvbuf, mss);

#ifdef OPTION_SET
#define RCVBUF 9973   /* a prime number */
	rcvbuf = RCVBUF;		
	if( setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &rcvbuf, sizeof(rcvbuf)) == -1){
		fprintf(stderr,"setsockopt error : %s\n", strerror(errno));
		return 1;
	}
	len = sizeof(rcvbuf);
	if( getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &rcvbuf, &len) == -1){
		fprintf(stderr,"getsockopt error : %s\n", strerror(errno));
		return 4;
	}
	printf("SO_RCVBUF = %d (after setting it to %d)\n", rcvbuf, RCVBUF);

#define MSS 1300
	mss=MSS;
	if( setsockopt(sockfd, IPPROTO_TCP, TCP_MAXSEG, &mss, sizeof(mss)) == -1){
		fprintf(stderr,"setsockopt error : %s\n", strerror(errno));
		return 1;
	}
	len = sizeof(mss);
	if( getsockopt(sockfd, IPPROTO_TCP, TCP_MAXSEG, &mss, &len) == -1 ){
		fprintf(stderr,"getsockopt error : %s\n", strerror(errno));
		return 3;
	}
	printf("MSS: = %d (after setting it to %d)\n", mss, MSS);
	
#endif //OPTION_SET

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin6_family = AF_INET6;
	servaddr.sin6_port = htons(65002);		/* daytime server */
	int er;
	if(  (er=inet_pton(AF_INET6, argv[1], &servaddr.sin6_addr)) == -1 ){
		fprintf(stderr,"inet_pton error : %s\n", strerror(errno));
		return 1;
	}
	else if(er==0)
	{ printf("Addres error \n");
		return 1;
	}
	

	if( connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) == -1){
		fprintf(stderr,"connect: %s\n", strerror(errno));
		return 1;
	}
	
	//snprintf(sendline, sizeof(sendline),"Wiadomosc");
	a= atoi(argv[2]);
	
	if (a==1 )
	InitCommunication(sockfd,sensorid);
	else
	SendingTemerature(sockfd,sensorid);
	
	fflush(stdout);

	exit(0);
}
