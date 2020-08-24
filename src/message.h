#ifndef MESSAGE_H
#define MESSAGE_H

#define NCDMSG_MAX 64

typedef struct
{
	
	char text[1024];
	int time;
	
} NCDMSG_MESSAGE;

typedef struct
{

	NCDMSG_MESSAGE message[NCDMSG_MAX];
	int messages;
	
} NCDMSG_QUEUE;

void ncdmsg_add(NCDMSG_QUEUE * qp, char * dp, int time);
void ncdmsg_logic(NCDMSG_QUEUE * qp);

#endif
