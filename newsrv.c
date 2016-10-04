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
#include        <sys/wait.h>

#define MAXLINE 1024

#define SA struct sockaddr

#define LISTENQ 2

#define CentralUnitID 1
void
sig_chld(int signo)
{
	pid_t	pid;
	int		stat;

	while ( (pid = waitpid(-1, &stat, WNOHANG)) > 0)
		printf("child %d terminated\n", pid);
	return;
}

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

int*
SequenceNumberChoose(int sockfd){//and send
	char		buf[MAXLINE];
	int 		*sek,k;
	sek=malloc (sizeof(int)*4);
	srand( time( NULL ) );
	for (k=0;k<4;k++){
		sek[k]=rand();
		//printf("%i\n",sek[k]);
	}
	sek[4]='\0';
	Writen(sockfd, sek, sizeof(int)*4);
	return sek;
}



void
str_echo(int sockfd, int sensors,int **tab)
{
	ssize_t		n;
	char		buf[MAXLINE];
	int 		k,sensorid,*sek;
	sek=SequenceNumberChoose(sockfd);
	for (k=0;k<4;k++){
		printf("sek number:%i\n",sek[k]);
	}
	n = read(sockfd, &sensorid, sizeof(int)*1);
	printf("\nSensor send his ID : %d\n",sensorid);
	for (k=0;k<sensors;k++){
		if(tab[k][0]==sensorid){
			
			printf("sensor id matched\n");
		}
	}
		
}
int
UpadateSensorsNumber(FILE *fp){
	int sensors;
	fscanf(fp, "%d\n", &sensors);
	printf(" Sensor number is: %d\n\n",sensors);
	return sensors;
	
}

int**
UpadateSensorsList(FILE *fp,int sensors){
	int					i,j;
	int** array;
	array=malloc (sizeof(int)*sensors);
	int numberArray[sensors][5];
	for (j = 0; j < sensors; j++){
		array[j]=malloc (sizeof(int)*5);
		for (i = 0; i < 5; i++)
		{
			fscanf(fp, "%d,", &array[j][i]);
		}
	}

return array;

}	

int
main(int argc, char **argv)
{
	FILE * fp;
	int					listenfd,i,j, connfd,sensors;
	int	**				tab;
	pid_t				childpid;
	socklen_t			clilen;
	struct sockaddr_in6	cliaddr, servaddr;
	void				sig_chld(int);
	int sensorid;
#ifdef SIGCHLD
    struct sigaction new_action, old_action;

  /* Set up the structure to specify the new action. */
    new_action.sa_handler = sig_chld;
  //  new_action.sa_handler = SIG_IGN;
    sigemptyset (&new_action.sa_mask);
    new_action.sa_flags = 0;

    if( sigaction (SIGCHLD, &new_action, &old_action) < 0 ){
          fprintf(stderr,"sigaction error : %s\n", strerror(errno));
          return 1;
    }
 
#endif 
//	signal(SIGCHLD, sig_chld);
//	signal(SIGCHLD, SIG_IGN);
	
	//POBIERANIE LISTY SENSORÓW	
	
   if ( (listenfd = socket(AF_INET6, SOCK_STREAM, 0)) < 0){
          fprintf(stderr,"socket error : %s\n", strerror(errno));
          return 1;
   }
   int optval = 1;               
   if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0){
                fprintf(stderr,"SO_REUSEADDR setsockopt error : %s\n", strerror(errno));
   }

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin6_family = AF_INET6;
	servaddr.sin6_addr   = in6addr_any;
	servaddr.sin6_port   = htons(65002);	/* daytime server */

   if ( bind( listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0){
           fprintf(stderr,"bind error : %s\n", strerror(errno));
           return 1;
   }

   if ( listen(listenfd, LISTENQ) < 0){
           fprintf(stderr,"listen error : %s\n", strerror(errno));
           return 1;
   }
   
   	fp = fopen ("passwd", "r");
	if (fp == 0) {
		perror ("open");
	}
	sensors=UpadateSensorsNumber(fp);
	tab=UpadateSensorsList(fp,sensors);
	close(fp);
	for (j = 0; j < sensors; j++){
		for (i = 0; i < 5; i++)
		{
        	printf("%d tab is: %d \n\n",i,tab[j][i]);
		}
	}

	for ( ; ; ) {
		clilen = sizeof(cliaddr);
		if ( (connfd = accept(listenfd, (SA *) &cliaddr, &clilen)) < 0) {
			if (errno == EINTR)
				continue;		/* back to for() */
			else
				perror("accept error");
				exit(1);
		}

		if ( (childpid = fork()) == 0) {	/* child process */
			close(listenfd);	/* close listening socket */
			str_echo(connfd,sensors,tab);	/* process the request */
			exit(0);
		}
		close(connfd);			/* parent closes connected socket */
	}
}


