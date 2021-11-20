#include "race.h"

main()
{
    int soc;
    char hostname[16];

    puts("input server's hostname:"); gets(hostname);

    if( (soc = setup_client(hostname, PORT)) == -1)
        exit(1);

    init_session(soc);

    session();
}