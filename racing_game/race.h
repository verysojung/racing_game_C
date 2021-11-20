#include <sys/types.h>
#include <sys/time.h>
#define MAX_PLAYERS 5
#define MAX_STAGES 10
#define MAX_DAMMAGE 10
#define PORT (u_short)10003

typedef struct
{
    int stage;
    int x, y;
    int damage;
    int mapchage;
}PLAYER;

typedef struct
{
    int entrynum;
    char name[8];
}GRADE;
