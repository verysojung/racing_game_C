#include <stdio.h>
#include "race.h"
#include "session_man.h"

main(int argc,char * argv[])
{
    int num;
    int final;
    int w_soc;
    int maxfd;
    char hostname[30];

    if(argc != 3)
    {
        fprintf(stderr, "Usage: s players stages \n");
        exit(1);
    }
    
    if ((num = atoi(argv[1])) <= 0 || num > MAX_PLAYERS)
    {
        fprintf(stderr, "players limit = %d\n", MAX_PLAYERS);
        exit(1);
    }
   
    if((final = atoi(argv[2])) <= 0 || final > MAX_STAGES)
    {
        fprintf(stderr, "stage limit = %d\n",MAX_STAGES);
        exit(1);
    }

    if(gethostname(hostname,30) == -1)
    {
        fprintf(stderr, "cannot find hostname\n");
        exit(1);
    }


    if( (w_soc = setup_m_srv(hostname, PORT, num)) == -1)
    {
        fprintf(stderr, "cannot setup_m_srv\n");
        exit(1);
    }

    maxfd = m_accept(w_soc, num, enter);

    init_session(num, final, maxfd);

    session_man();
    fprintf(0, "츨력\n");
}
