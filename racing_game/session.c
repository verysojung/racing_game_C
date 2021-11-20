#include <curses.h>
#include <signal.h>
#include "race.h"

static PLAYER p[MAX_PLAYERS];
static PLAYER temp;
static GRADE g[MAX_PLAYERS];

static int num;
static int final;
static int soc;
static int entry_num;
static int flag;
static int final_stage;
static PLAYER *me;
static WINDOW *win1, *win2;

static int get_my_location();
static void send_my_data();
static void del_all_locations();
static void show_all_locations();
static void recv_all_data();
static void save_stage();
static int new_stage();
char y[21][11];

static void init_data();
static void ending();

static void die();

void init_session(int s)
{
    char buf[32];
     soc = s;

     read(soc, buf, 32);
     puts(buf);
     gets(buf);
     write(soc, buf, strlen(buf)+1);
     read(soc, buf, 32);
     sscanf(buf, "%d", &entry_num);
     printf("Your entry number is %d\n", entry_num);
     read(soc, buf, 32);
     printf("msg %s",buf);
     sscanf(buf, "%d %d", &num, &final);
     printf("players are %d\n", num);
     printf("final stage is %d\n", final);

     signal(SIGINT, die);
     signal(SIGQUIT, die);
     signal(SIGTERM, die);
     signal(SIGSEGV, die);
     signal(SIGPIPE, die);

     initscr();
     noecho();
     crmode();
     nonl();
     win1 = newwin(22, 12, 1, 10);
     win2 = newwin(11, 25, 5, 40);
     box(win1, '|', '-');
     box(win2, '|', '-');

     new_stage(1);

     init_data();
     mvwprintw(win2, 2, 1, "Entry Num = %d", entry_num);
     mvwprintw(win2, 3, 2, "num stage damage");
     mvwprintw(win2, entry_num + 4, 1, "*");
     show_all_locations();
}
static void send_my_data()
{
    write(soc, &temp, sizeof(PLAYER));
}

void session()
{
    char buf[32];

    flag = 1;

    while(flag)
    {
        flag = get_my_location();
        send_my_data();
        del_all_locations();
        recv_all_data();
        show_all_locations();

    }
    ending(final_stage);

    die();

}
static int check(int a, int b)
{
    int p,q;

    p = temp.x + a;
    q = temp.y + b;

    if(p<1 || p>10 || q<1 || q>21 )
        return 0;

    if(y[q][p] == '*')
        return 0;

    return 1;
}


static int get_my_location()
{
    int c;
    int d;

    c=getchar();
    
    switch(c)
    {
        case 'a':
        case '4':
        case 'h':
            if(check(-1,0) == 1)
                temp.x--;
            else temp.damage++;
            break;
        case 's':
        case '2':
        case 'j':
            if(check(0,1) == 1)
                temp.y++;
            else temp.damage++;
            break;
        case 'w':
        case '8':
        case 'k':
            if(temp.y == 1)
            {
                temp.stage++;
                temp.y = 21;
                if(new_stage(temp.stage) == 0)
                {
                    final_stage = 0;
                    return 0;
                }
            }

            else if((d = check(0, -1)) == 1)
                temp.y--;
            else temp.damage++;
            break;

        case 'd':
        case '6':
        case '1':
            if(check(1,0) == 1)
                temp.x++;
            else temp.damage++;
            break;
	case '0':
		if(y[temp.y-1][temp.x]=='*') y[temp.y-1][temp.x] == ' ';
		else y[temp.y-1][temp.x]='*';	
		save_stage(temp.stage);
		new_stage(temp.stage);
		break;
        default:
            break;
    }

    if (temp.damage >= MAX_DAMMAGE)
    {
        final_stage = -1;
        return 0;
    }
    return 1;
}




static void del_all_locations()
{
    int i;

    for(i=0; i<num; i++)
    {
        if (p[i].stage == me->stage)
        {
            mvwprintw(win1, p[i].y, p[i].x, " ");
        }
    }

    touchwin(win1);
    wrefresh(win1);
}

static void recv_all_data()
{
    read(soc, p, sizeof(PLAYER)*num);
}

static void show_all_locations()
{
    int i;

    for(i=0; i<num; i++)
    {
        if(p[i].stage == me->stage){
            mvwprintw(win1, p[i].y, p[i].x, "%c", (char)(i+0x30));
	    if(p[i].mapchange==1) new_stage(me->stage);
	    }
    }

    touchwin(win1);
    wrefresh(win1);

    mvwprintw(win2, 9, 2, "stage = %2d/%2d", me->stage, final);
    for(i=0; i<num; i++)
        mvwprintw(win2, i+4, 2, "%2d %2d %2d", i, p[i].stage, p[i].damage);

    wrefresh(win2);
}

static void save_stage(int z){
    FILE *fp;
    int i, j;
    static char buf[16];

    if(z>final) return 0;

    sprintf(buf, "data%d",z);
    fp = fopen(buf, "w");
    for(i=1; i<=20; i++)
    {
	for(j=1; j<=10; j++)
            buf[j-1]=y[i][j] ;
	fputs(buf, fp);
	fputs(buf, fp);
 
    }
    fclose(fp);

}


static int new_stage(int z)
{
    FILE *fp;
    int i, j;
    static char buf[10];

    if(z>final) return 0;

    sprintf(buf, "data%d",z);
    fp = fopen(buf, "r");
    for(i=1; i<=20; i++)
    {
        fgets(buf, 16, fp);
        buf[10]='\0';
        wmove(win1, i, 1);
        waddstr(win1, buf);

	for(j=1; j<=10; j++)
            y[i][j] = buf[j-1];
    }
    fclose(fp); return 1;

}

static void ending(int how)
{
    int i;

    wclear(win2);
    box(win2, '|','-');
    if(how == 0)
        mvwprintw(win2, 1, 3, "Goal in, Wait");
    else
        mvwprintw(win2, 1, 3, "Crashed, Wait");

    wrefresh(win2);

    read(soc, g, (sizeof(GRADE))*num);
    
    for(i=0; i<num; i++)
    {
        mvwprintw(win2, i+3, 1, "[%d] %d: %s",i+1, g[i].entrynum, g[i].name);
    }
    wrefresh(win2);
    //move(23, 1); 
    refresh(); 
    getchar();

    
}


static void init_data()
{
    int i;

    me = p + entry_num;
    for(i=0; i<num; i++)
    {
        p[i].stage = 1;
        p[i].x = 2+i;
        p[i].y = 20;
        p[i].damage = 0;
	p[i].mapchange=0;
    }
    temp = *me;
}



static void die()
{
    endwin();
    echo();
    exit(1);
}
