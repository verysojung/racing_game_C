#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

int setup_m_srv(char*hostname, u_short port, int backlog)
{
    struct hostent *myhost;
    struct sockaddr_in me;
    int s;

    if( (myhost = gethostbyname(hostname)) == NULL)
    {
        fprintf(stderr, "bad hostname1\n");
        return -1;
    }

    bzero((char*)&me, sizeof(me));
    me.sin_family = AF_INET;
    me.sin_port = port;
    bcopy(myhost->h_addr, (char*)&me.sin_addr, myhost->h_length);

    if( (s = socket(AF_INET, SOCK_STREAM,0)) < 0 )
    {
      fprintf(stderr, "socket allocation failed.\n");
      return -1;
    }

    if (bind(s, &me, sizeof(me)) == -1)
    {
      fprintf(stderr, "cannot bind.\n");
      return -1;
    }

    fprintf(stderr, "successfully bound,now listens.\n");

    if(listen(s,backlog) == -1)
    {
        fprintf(stderr,"cannot listen.\n");
        return -1;
    }

    fprintf(stderr, "listen ok, now ready to accept.\n");
    return s;
}


int m_accept(int w_soc, int limit, void (*func)())
{
    int i;
    int fd;

    for(i=0; i<limit; i++)
    {
        if( (fd= accept(w_soc,NULL,NULL)) == -1 )
        {
            fprintf(stderr,"accpt erro\n");
            return -1;
        }
        (*func)(i,fd);
    }
    close(w_soc);
    return fd;
}
