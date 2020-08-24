#include "message.h"

void ncdmsg_add(NCDMSG_QUEUE * qp, char * dp, int time)
{
	int i;
	
	/*
	if(qp->messages < LW_MAX_MESSAGES)
	{
		for(i = qp->messages; i > 0; i--)
		{
			strcpy(qp->message[i].text, qp->message[i - 1].text);
			qp->message[i].time = qp->message[i - 1].time;
		}
		strcpy(qp->message[0].text, dp);
		qp->message[0].time = time;
		qp->messages++;
	} */
	if(qp->messages < NCDMSG_MAX)
	{
		strcpy(qp->message[qp->messages].text, dp);
		qp->message[qp->messages].time = time;
		qp->messages++;
	}
}

void ncdmsg_logic(NCDMSG_QUEUE * qp)
{
	int i, j;
	
	for(i = 0; i < qp->messages; i++)
	{
		if(qp->message[i].time > 0)
		{
			qp->message[i].time--;
			if(qp->message[i].time <= 0)
			{
				for(j = i + 1; j < qp->messages; j++)
				{
					strcpy(qp->message[j - 1].text, qp->message[j].text);
					qp->message[j - 1].time = qp->message[j].time;
				}
				qp->message[j].time = 0;
				qp->messages--;
			}
		}
	}
}
