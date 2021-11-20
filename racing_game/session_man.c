#include <stdio.h>
#include <sys/types.h>
#include "race.h"

#define MAX_DAMMAGE 10

static int soc[MAX_PLAYERS];
static int name[MAX_PLAYERS][16];
static PLAYER p[MAX_PLAYERS];
static fd_set mask;
static fd_set readOk;
static int width;

static int final;
static int num;
static int reached;
static int crashed;
static GRADE g[MAX_PLAYERS];
static void recv_data();
static void send_data();
static void ending();

void enter(int i, int fd)
{
    int len;
    static char *login_mesg = "Your name?";
    char mesg[32];

    soc[i] = fd;
    write(soc[i], login_mesg, 11);
    read(soc[i], name[i], 16);

    sprintf(mesg, "%d\n",i);
    write(soc[i], mesg, strlen(mesg) + 1);
    printf("[%d] %s\n", i, name[i]);
}

void init_session(int n, int fin, int maxfd)
{
    int i;
    static char mesg[10];

    num =n;
    final = fin;

    width = maxfd +1;
    FD_ZERO(&mask);
    FD_SET(0,&mask);

    for(i=0; i<num; i++)
        FD_SET(soc[i], &mask);

    sprintf(mesg, "%d %d\n", num, final);
    for(i=0; i<num; i++)
        write(soc[i], mesg, 10);
    reached = 0;
    crashed = 0;

}

void session_man()
{
    while(reached + crashed < num)
    {
        recv_data();
        send_data();
    }
    
    ending();
}



static void recv_data()
{
    int i;

    readOk = mask;
    select(width, &readOk, NULL, NULL, NULL);
    for(i=0; i<num; i++)
    {
        if(FD_ISSET(soc[i], (fd_set *)&readOk))
        {
            read(soc[i], &p[i], sizeof(PLAYER));
            if(p[i].damage >= MAX_DAMMAGE)
            {
                g[num-crashed -1].entrynum = i;
                strcpy(g[num-crashed-1].name, name[i]);
                crashed++;
            }

            if(p[i].stage > final)
            {
                g[reached].entrynum = i;
                strcpy(g[reached].name, name[i]);
                reached++;
            }

        }
    }
}


static void send_data()
{
    int i;

    for( i=0; i<num; i++)
    {
        if( FD_ISSET(soc[i], &readOk))
        {
           write(soc[i], p, (sizeof(PLAYER))*num);
	   p[i].mapchange=0;
        }
    }
}

static void ending()
{
    int i;
    for(i=0; i<num; i++)
        write(soc[i], g, (sizeof(GRADE)*num));
}
