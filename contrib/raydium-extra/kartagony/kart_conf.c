// Kartagony - configuration defines

#define DEFAULT_SCORE 		3
#define BONUSBLINK_FACT		5
#define JUMP_FORCE		700
#define ROCKET_SPEED		70
#define WHEELS_BREAK		0

// all following vars defined in seconds :
#define INVINCIBLE_TIME		3
#define BOOST_TIME		5
#define WEIGHT_TIME		5
#define OIL_TIME		5
#define SMOKE_TIME		5
#define BONUS_TTL		20
#define BONUSROLL_TIME		4
#define BONUSBLINK_TIME		2
#define REPLACE_TIME		3

typedef struct Team
{
GLfloat color[3];
char name[16];
} Team;

Team teams[4];

#define TEAM_COL(n)  teams[n].color[0],teams[n].color[1],teams[n].color[2]
#define TEAM_COL2(n) teams[n].color[0]-0.2,teams[n].color[1]-0.2,teams[n].color[2]-0.2

void conf_init(void)
{
strcpy(teams[0].name,"Red Team");
strcpy(teams[1].name,"Green Team");
strcpy(teams[2].name,"Blue Team");
strcpy(teams[3].name,"Yellow Team");

teams[0].color[0]=0.9;
teams[0].color[1]=0.2;
teams[0].color[2]=0.2;

teams[1].color[0]=0.2;
teams[1].color[1]=0.9;
teams[1].color[2]=0.2;

teams[2].color[0]=0.2;
teams[2].color[1]=0.2;
teams[2].color[2]=0.9;

teams[3].color[0]=0.9;
teams[3].color[1]=0.9;
teams[3].color[2]=0.2;

in_game=0;
}
