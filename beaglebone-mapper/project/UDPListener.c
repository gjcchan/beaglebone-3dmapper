#include "routine.h"
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>


/*
	All you need to do is to send a UDP message of "go" and it will return a full JSON object of all coords when done
*/

	
#define	PORT		12345		/* default protocol port number */
#define BUFF_SIZE	1000000			/*default buffer size */
/*
*	Code listens to the UDP port of 12345 to get queries from node server
*/
static struct	sockaddr_in sad; /* structure to hold server's address	*/
static struct	sockaddr_in cad2;/* structure to hold client's address	UDP ONLY*/
static int		sd3;	 	 	 /* socket descriptors			*/
static int   	udpRecv;
static int 		running = 0;
static socklen_t alen2;	 		/* length of address			*/

static pthread_t listener;		/* listener thread */
static char 	udpBuf[BUFF_SIZE];
static char		replyBuffer[BUFF_SIZE];
static char* 	cmd;			/* used for parsing input */

static void* UDP_listen(void* data); 

void UDP_ListenerLaunchThread(void) 
{
	//open up listener socket
	memset((char *)&sad,0,sizeof(sad)); /* clear sockaddr structure	*/
	sad.sin_family = AF_INET;	  		/* set family to Internet	*/
	sad.sin_addr.s_addr = INADDR_ANY; 	/* set the local IP address	*/
	sad.sin_port = htons((unsigned short)PORT);
	sd3 = socket(PF_INET, SOCK_DGRAM, 0);
	if(sd3 < 0) {
		fprintf(stderr, "socket creation failed\n");
		exit(1);		
	}
	if (bind(sd3, (struct sockaddr *)&sad, sizeof(sad)) < 0) {
		fprintf(stderr,"bind failed\n");
		exit(1);
	}
	//create thread
	pthread_create(&listener, NULL, UDP_listen, NULL);
}
void UDP_close(void) 
{
	running = 0;
	pthread_kill(listener, 1);
	close(sd3);

}

static void* UDP_listen(void* data) 
{
	running = 1;
	fprintf(stderr, "udp up");
	while(running == 1) 
	{
		// clear buffer first HERE
		alen2 = sizeof(cad2);
		udpRecv = recvfrom(sd3,udpBuf,BUFF_SIZE,0,(struct sockaddr *)&cad2,&alen2);
		if(udpRecv < 0) {
			fprintf(stderr, "udp packet receieve failed\n");
		} 
		else {
			cmd = strtok(udpBuf," \n");
			if(strcmp(cmd,"go") == 0) {
				routine_run();	
				char* sendTo = routine_json();
				sendto(sd3,sendTo,
					strlen(sendTo),
					0,
					(struct sockaddr *)&cad2,sizeof(cad2));	
			}
			else if(strcmp(cmd,"set") == 0) {
				int newDeg = atoi(strtok(NULL," \n"));
				routine_change_deg(newDeg);
				char* sendTo = "recv";
				sendto(sd3,sendTo,
					strlen(sendTo),
					0,
					(struct sockaddr *)&cad2,sizeof(cad2));	
			}
			else if(strcmp(cmd,"takeSnapshot") == 0) {
				routine_run();	
				char* sendTo = routine_json();
				sendto(sd3,sendTo,
					strlen(sendTo),
					0,
					(struct sockaddr *)&cad2,sizeof(cad2));	
			}
			else if(strcmp(cmd,"check") == 0) {
				sendto(sd3,"ack",
					strlen("ack"),
					0,
					(struct sockaddr *)&cad2,sizeof(cad2));	
			}
			else if (strcmp(cmd, "updateDimension") == 0){
				double* returnValues = routine_getDimensionValues();
				replyBuffer[0] = 0;
				sprintf(replyBuffer, "%lf %lf %lf", returnValues[0], returnValues[1], returnValues[2]);
				sendto(sd3, replyBuffer,
					strlen(replyBuffer),
					0,
					(struct sockaddr *)&cad2,sizeof(cad2));
			}
			else if (strcmp(cmd, "updateTemperature") == 0){
				double tempValue = routine_getTemperatureValue();
				replyBuffer[0] = 0;
				sprintf(replyBuffer, "%lf", tempValue);
				sendto(sd3, replyBuffer,
					strlen(replyBuffer),
					0,
					(struct sockaddr *)&cad2,sizeof(cad2));
			}
			else {
				//bad input response here
				replyBuffer[0] = 0;
				sprintf(replyBuffer, "bad command: %s", cmd);
				sendto(sd3,"bad command:",
					strlen(replyBuffer),
					0,
					(struct sockaddr *)&cad2,sizeof(cad2));					
			}
			//sends a endline to force flush buffer
			sendto(sd3,"\n",1,0,(struct sockaddr *)&cad2,sizeof(cad2));	
		}

		memset(udpBuf, '\0', sizeof(char)*BUFF_SIZE);

	}
	return NULL;
}
