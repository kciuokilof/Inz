#include    <sys/types.h>   /* basic system data types */
#include    <sys/socket.h>  /* basic socket definitions */
#if TIME_WITH_SYS_TIME
#include        <sys/time.h>    /* timeval{} for select() */
#include    <time.h>                /* timespec{} for pselect() */
#else
#if HAVE_SYS_TIME_H
#include    <sys/time.h>    /* includes <time.h> unsafely */
#else
#include    <time.h>                /* old system? */
#endif
#endif
#include    <netinet/in.h>  /* sockaddr_in{} and other Internet defns */
#include    <arpa/inet.h>   /* inet(3) functions */
#include    <errno.h>
//#include    <fcntl.h>               /* for nonblocking */
#include    <netdb.h>
#include    <signal.h>
#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>
#include    <limits.h>		/* for OPEN_MAX */
//#include	<poll.h>
#include 	<sys/epoll.h>
#include    <unistd.h>

//#define STATS
#ifdef STATS
#include 	<pthread.h>
#endif
 
#define MAXLINE 10
#define SA struct sockaddr
#define LISTENQ 2
#define INFTIM -1
#define MAXEVENTS 200000
#define	centralunitid 10
#define EnUnitSize 5
#define enTTL 300

ssize_t						/* Write "n" bytes to a descriptor. */
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
/* end writen */

void
Writen(int fd, void *ptr, size_t nbytes)
{
	if (writen(fd, ptr, nbytes) != nbytes)
		perror("writen error");
}


	int evn_r;
	int activeconns_r;
	long s_r;

	
#ifdef STATS
void *
print_r(void *arg)
{
	while (1){
		fprintf(stderr,"\rService of %6d events in %10ld us : active connections: %6d", evn_r,s_r, activeconns_r);
		sleep(1);
	}
	return(NULL);
}
#endif


int*
SequenceNumberChoose(int sockfd){//and send
	int 		*sek,k;
	sek=malloc (sizeof(int)*EnUnitSize);
	srand( time( NULL ) );
	sek[0]=centralunitid;
	for (k=1;k<5;k++){
		sek[k]=rand();
		printf("numer :%i\n",sek[k]);
	}
	sek[EnUnitSize]='\0';
	printf("\nWybrano numer sekwencyjny\n");
	Writen(sockfd, sek, sizeof(int)*EnUnitSize);
	printf("\nWysłano numer sekwencyjny\n");
	return sek;
}

int
SensorCommunication(int currfd, int activeconns, int sensors, int **tab){
	ssize_t		n;	
	int 		k,sensorid,*sek;
printf("\nsesnor com num %d\n",sensors);
	sek=SequenceNumberChoose(currfd);
	tab[sensors][5]=enTTL;
	tab[sensors][6]=sek[1];
	tab[sensors][7]=sek[2];
	tab[sensors][8]=sek[3];
	tab[sensors][9]=sek[4];
	close(currfd);
	return activeconns;
}
int ReadingTemp(int currfd,int activeconns,int recvint[4]){
	char	sendline[70];
	FILE 	*TemperatureArray;
	time_t 	rawtime;
	struct 	tm * timeinfo;
	time(&rawtime);
	timeinfo=localtime(&rawtime);
	
	
		// We successfully read from the socket.
	TemperatureArray = fopen ("TemperatureArray", "a");
	if (TemperatureArray!=NULL){
			printf("przesłano temp:%d ",recvint[1]);
  			snprintf(sendline, sizeof(sendline),"%s-%d-%d\n", asctime(timeinfo), recvint[0], recvint[1]);
    		fputs (sendline,TemperatureArray);
    		fclose (TemperatureArray);
  		}
  		else{
  			perror("openfile error"); 		
  			return 0;
  		}
	
	
	
	return activeconns;
	}


int
UpadateSensorsNumber(FILE *fp){
	int sensors;
	fscanf(fp, "%d\n", &sensors);
	printf("\n Sensor number is: %d\n\n",sensors);
	return sensors;
	
}

int**
UpadateSensorsList(FILE *fp,int sensors){
	int					i,j;
	int** array;
	array=malloc (sizeof(int)*sensors);
	for (j = 0; j < sensors; j++){
		array[j]=malloc (sizeof(int)*10);
		printf("\nSensor nr.%d\n",j+1);
		for (i = 0; i < 5; i++)
		{
			fscanf(fp, "%d,", &array[j][i]);
			printf(" %d ",array[j][i]);
		}
	}

return array;

}	



int
main(int argc, char **argv)
{
	int debug = 0;
	int  **				tab;
	FILE *				fp;
	int					listenfd, connfd, sensors,recvint[10];
	pid_t				childpid;
	socklen_t			clilen;
	struct sockaddr_in6	cliaddr, servaddr;
	int					i,j, maxi, maxfd, n, k; 
	int					nready, s;
	char 				addr_buf[INET6_ADDRSTRLEN+1];
	struct 				timeval start, stop;
	int activeconns = 0;
    struct sockaddr_storage peer_addr;
    socklen_t peer_addr_len;
	
	printf("MAX CONNECTIONS = %d\n",MAXEVENTS-2);
	
	if( argc > 1 )
		debug = atoi(argv[1]);
	
	int epollfd, eventstriggered, currfd;
	struct epoll_event events[MAXEVENTS];
	struct epoll_event ev;
	

	fp = fopen ("passwd", "r");
	if (fp == 0) {
		perror ("open");
	}
	sensors=UpadateSensorsNumber(fp);
	tab=UpadateSensorsList(fp,sensors);
	for (j = 0; j < sensors; j++){
		for (i = 0; i < 5; i++)
		{
        	printf("%d tab is: %d \n\n",i,tab[j][i]);
		}
	}
	fclose(fp);
	
		/* Set up our epoll queue */
	if((epollfd = epoll_create(MAXEVENTS)) == -1){ 	
          fprintf(stderr,"epoll_create() error : %s\n", strerror(errno));
          return -1;
   }
	
//Create listening sockets	
   if ( (listenfd = socket(AF_INET6, SOCK_STREAM, 0)) < 0){
          fprintf(stderr,"TCP socket error : %s\n", strerror(errno));
          return -1;
   }
   int on = 1;               
   if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0){
        fprintf(stderr,"SO_REUSEADDR setsockopt error : %s\n", strerror(errno));
   }

   

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin6_family = AF_INET6;
	servaddr.sin6_addr   = in6addr_any;
	servaddr.sin6_port   = htons(65002);	/* echo server */

   if ( bind( listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0){
           fprintf(stderr,"TCP bind error : %s\n", strerror(errno));
           return 1;
   }

   

   if ( listen(listenfd, LISTENQ) < 0){
           fprintf(stderr,"listen error : %s\n", strerror(errno));
           return 1;
   }

	activeconns +=2;	
	activeconns_r = activeconns;
	int time_start=0;
	int evn;
	ev.events = EPOLLIN; //Specify which event to listen for
	ev.data.fd = listenfd; // Specify user data.

	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &ev) == -1){
           fprintf(stderr,"listen error : %s\n", strerror(errno));
           return 1;
	}
	

#ifdef STATS
	int ret;
	pthread_t	tid;
	if ( (ret=pthread_create(&tid, NULL, print_r, NULL)) != 0 ) {
	    errno=ret;
	    fprintf(stderr, "pthred_create() error : %s", strerror(ret));
	}
#endif

#define AVG 1000
	long ss[AVG];
	int count=0;
	int iter=0;
	debug=1;
	for ( ; ; ) {


	if( gettimeofday(&stop,0) != 0 ){
		fprintf(stderr,"gettimeofday error : %s\n", strerror(errno));
	}else{
		if(time_start == 1){
			long s =(long)((stop.tv_sec*1000000 + stop.tv_usec) - (start.tv_sec*1000000 + start.tv_usec));
			if(count < 100) count++;
			if(iter > 99) iter=0;
			ss[iter]=s;
			long sum;
			
			for(k=0; k < count; k++) sum+=ss[iter];	
			iter++; 
			s_r=sum/count; 	evn_r = evn; 	activeconns_r =  activeconns;
//		    if(debug) 
//				fprintf(stderr,"Service of %6d events in %10ld us : active connections: %6d",
//					evn,s, activeconns);
			time_start=0;  
		}
	}
	
    	if( (nready = epoll_wait(epollfd, events, MAXEVENTS, -1)) == -1){
           fprintf(stderr,"epoll_wait error : %s\n", strerror(errno));
           return 1;
		}
		
		if(debug)
			printf ("New events=%d, active connections: %d\n", nready, activeconns);
			
	for (i = 0; i < nready; i++) {	/* check all clients for data */
	printf("\n\n\ncheck i = %d\n",i);
	if( gettimeofday(&start,0) != 0 ){
		fprintf(stderr,"gettimeofday error : %s\n", strerror(errno));
	}else{
		time_start=1;
		evn = nready;
	}



//TCP listen socket
        if (events[i].data.fd == listenfd) { //e
		   clilen = sizeof(cliaddr);
		   if ( (connfd = accept(listenfd, (SA *) &cliaddr, &clilen)) < 0) {
				perror("accept error");
				continue;
		   }
if(debug)		   
printf ("\tnew TCP client: events=%d, sockfd = %d, on socket = %d,  activeconns = %d \n", nready, listenfd, connfd, activeconns);
		
		   bzero(addr_buf, sizeof(addr_buf));
	   	   inet_ntop(AF_INET6, (struct sockaddr  *) &cliaddr.sin6_addr,  addr_buf, sizeof(addr_buf));
		   if(debug)
			 printf("\tNew client: %s, port %d\n",	addr_buf, ntohs(cliaddr.sin6_port));

		   if ((activeconns+1) >= MAXEVENTS) { //epoll
			// Too many connections for this program
				fprintf(stderr, "accept() error - too many connections for this program\n");
				close(connfd);
				continue;
		   }
		   ev.events =  EPOLLIN  ; // available for input and non edge_triggered
		   ev.data.fd = connfd;
		   if (-1 == epoll_ctl(epollfd, EPOLL_CTL_ADD, connfd, &ev)){
				perror("epoll_ctl new connection error");
				close(connfd);
				continue;
		   }
		   activeconns ++;
		   
		   continue;
	    } //end of epoll
//end of TCP listen socket
//tcp clients
		currfd = events[i].data.fd;
		if (-1 == epoll_ctl(epollfd, EPOLL_CTL_DEL, currfd, &ev)){
				perror("epoll_ctl new connection error");
				close(currfd);
				continue;
		   }
		if(debug)		printf ("\nRead client\n: events=%d, sockfd = %d\n", nready, currfd);
		
		if((n = read(currfd, recvint, sizeof (int)*10)) == -1) {
			// Closing the descriptor will make epoll remove it from the set of descriptors which are monitored.
		perror("read error - closing connection");
		close(currfd);
		activeconns --;
		return activeconns;
	}
		// We successfully read from the socket. Check if there is data
	 if( n==0) {
		printf("\nEOF\n");
			// The socket sent EOF.
			// Closing the descriptor will make epoll remove it from the set of descriptors which are monitored.
		close (currfd);
		activeconns --;
		return activeconns;
		
	}
	recvint[10]='\0';
	for(k=0; k<10;k++){
		
		printf("Dane : %d\n",recvint[k]);
		
		}
	printf("\nSensor send his ID : %d\n",recvint[0]);
	for (k=0;k<sensors;k++){
		if(tab[k][0]==recvint[0]){
			if(tab[k][5]!=0){
				
				//Kod do obsługi sensorów z ustaionymi parametrami szyfrowania.
				printf("sensor id matched, encryption established par :%d\n",recvint[1]);
				
				ReadingTemp(currfd,activeconns,recvint);
				

			}

			else{
				//Kod do obsługi sensorów z nie ustaionymi parametrami szyfrowania.
				printf("sensor id matched\n");
				activeconns=SensorCommunication(currfd,activeconns,i,tab);
				break;
			}
		}
	}
	
	for (j = 0; j < sensors; j++){
		for (k = 0; k < 10; k++){
        	printf("%d tab is: %d \n\n",k,tab[j][k]);
		}
	}
//epoll end
}
	}
}










	
	
	
	
