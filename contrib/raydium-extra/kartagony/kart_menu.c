// Kartogony - Menu Items

typedef struct MenuItem
{
char state;
char text[RAYDIUM_MAX_NAME_LEN];
char texture[RAYDIUM_MAX_NAME_LEN];
void *OnClick; //event
int  param;
float px;
float py;
float sizex;
float sizey;
float text_size;
float colors[9]; // 3 each, off, on, current
} MenuItem;

#define MAX_MENU_ITEMS 20
MenuItem menus[MAX_MENU_ITEMS];
char menu;
char menu_server_ip[RAYDIUM_MAX_NAME_LEN];
char menu_networked_game;;
int  menu_team;

void MenuItem_init(MenuItem *e)
{
e->state=0;
e->OnClick=NULL;
}

void MenuItem_colors(MenuItem *e, float off_r, float off_g, float off_b, float on_r, float on_g, float on_b)
{
float f[9];
f[0]=off_r;
f[1]=off_g;
f[2]=off_b;
f[3]=on_r;
f[4]=on_g;
f[5]=on_b;
// current color
f[6]=off_r;
f[7]=off_g;
f[8]=off_b;
memcpy(e->colors,f,sizeof(float)*9);
}

void MenuItem_init_all(void)
{
int i;
for(i=0;i<MAX_MENU_ITEMS;i++)
    MenuItem_init(&menus[i]);
}

void MenuItem_draw(void)
{
int i;
void (*f)(int);
float dec;

float mx;
float my;
float inc;

mx=(raydium_mouse_x/(raydium_window_tx+0.1))*100;
my=100-(raydium_mouse_y/(raydium_window_ty+0.1))*100;
inc=frame_time*4;

for(i=0;i<MAX_MENU_ITEMS;i++)
    if(menus[i].state)
	{
	f=menus[i].OnClick;
	
	if(mx>menus[i].px-(menus[i].sizex/2) && mx<menus[i].px+(menus[i].sizex/2) &&
	   my>menus[i].py-(menus[i].sizey/2) && my<menus[i].py+(menus[i].sizey/2) )
	   {
	   // selected
	   if(menus[i].colors[3]>menus[i].colors[0] && menus[i].colors[6]<menus[i].colors[3])
		menus[i].colors[6]+=inc;
	   if(menus[i].colors[0]>menus[i].colors[3] && menus[i].colors[6]>menus[i].colors[3])
		menus[i].colors[6]-=inc;

	   if(menus[i].colors[4]>menus[i].colors[1] && menus[i].colors[7]<menus[i].colors[4])
		menus[i].colors[7]+=inc;
	   if(menus[i].colors[1]>menus[i].colors[4] && menus[i].colors[7]>menus[i].colors[4])
		menus[i].colors[7]-=inc;

	   if(menus[i].colors[5]>menus[i].colors[2] && menus[i].colors[8]<menus[i].colors[5])
		menus[i].colors[8]+=inc;
	   if(menus[i].colors[2]>menus[i].colors[5] && menus[i].colors[8]>menus[i].colors[5])
		menus[i].colors[8]-=inc;	   
	   if(raydium_mouse_click==1 && f) 
	    { 
	    f(menus[i].param); 
	    raydium_mouse_click=0;
//	    return;
	    }
	   }
	   else
	   {
	   // not selected
	   if(menus[i].colors[3]>menus[i].colors[0] && menus[i].colors[6]>menus[i].colors[0])
		menus[i].colors[6]-=inc;
	   if(menus[i].colors[0]>menus[i].colors[3] && menus[i].colors[6]<menus[i].colors[0])
		menus[i].colors[6]+=inc;

	   if(menus[i].colors[4]>menus[i].colors[1] && menus[i].colors[7]>menus[i].colors[1])
		menus[i].colors[7]-=inc;
	   if(menus[i].colors[1]>menus[i].colors[4] && menus[i].colors[7]<menus[i].colors[1])
		menus[i].colors[7]+=inc;

	   if(menus[i].colors[5]>menus[i].colors[2] && menus[i].colors[8]>menus[i].colors[2])
		menus[i].colors[8]-=inc;
	   if(menus[i].colors[2]>menus[i].colors[5] && menus[i].colors[8]<menus[i].colors[2])
		menus[i].colors[8]+=inc;	   

	   }
	   
	raydium_osd_color_rgba(menus[i].colors[6],menus[i].colors[7],menus[i].colors[8],1);
	raydium_osd_draw_name(menus[i].texture,
			      menus[i].px-(menus[i].sizex/2),
			      menus[i].py-(menus[i].sizey/2),
			      menus[i].px+(menus[i].sizex/2),
			      menus[i].py+(menus[i].sizey/2));
	dec=strlen(menus[i].text)-2;
	dec*=(menus[i].text_size/26);
	raydium_osd_printf(menus[i].px-dec,menus[i].py,menus[i].text_size,0.5,"font2.tga","%s",menus[i].text);
	}
}



/////////////////
void menu_color(int);
void menu_ip(int);
void menu_main(int);
void menu_game(int);
void start_game(void);
///

void menu_number(int num)
{
int i;
i=strlen(menu_server_ip);

if(num==27)
    {
    if(i==0) return;
    menu_server_ip[i-1]=0;
    }
else
    if(i<15)
    {
    menu_server_ip[i]=num;
    menu_server_ip[i+1]=0;
    }
sprintf(menus[13].text,"^f%s",menu_server_ip);
}

void menu_main(int dummy)
{
menu_networked_game=0;
menu=1;

MenuItem_init_all();

// Training
menus[0].state=1;
strcpy(menus[0].text,"^fTraining");
strcpy(menus[0].texture,"aqua.tga");
menus[0].OnClick=menu_color;
menus[0].px=50;
menus[0].py=80;
menus[0].sizex=30;
menus[0].sizey=10;
menus[0].text_size=20;
MenuItem_colors(&menus[0],0.9,0.2,0.2, 0.2,0.2,0.9);

// Play
menus[1].state=1;
strcpy(menus[1].text,"^fNetwork Play");
strcpy(menus[1].texture,"aqua.tga");
menus[1].OnClick=menu_ip;
menus[1].px=50;
menus[1].py=60;
menus[1].sizex=30;
menus[1].sizey=10;
menus[1].text_size=20;
MenuItem_colors(&menus[1],0.9,0.2,0.2,0.2,0.2,0.9);

// Quit
menus[2].state=1;
strcpy(menus[2].text,"^fQuit");
strcpy(menus[2].texture,"aqua.tga");
menus[2].OnClick=exit;
menus[2].param=0;
menus[2].px=50;
menus[2].py=20;
menus[2].sizex=30;
menus[2].sizey=10;
menus[2].text_size=20;
MenuItem_colors(&menus[2],0.9,0.9,0.9,0.1,0.1,0.1);

}


void menu_color(int dummy)
{
menu=1;

MenuItem_init_all();

menus[0].state=1;
strcpy(menus[0].text,"^fRed Team");
strcpy(menus[0].texture,"aqua.tga");
menus[0].OnClick=menu_game;
menus[0].param=0;
menus[0].px=50;
menus[0].py=90;
menus[0].sizex=30;
menus[0].sizey=10;
menus[0].text_size=20;
MenuItem_colors(&menus[0], TEAM_COL2(0), TEAM_COL(0));

menus[1].state=1;
strcpy(menus[1].text,"^fGreen Team");
strcpy(menus[1].texture,"aqua.tga");
menus[1].OnClick=menu_game;
menus[1].param=1;
menus[1].px=50;
menus[1].py=70;
menus[1].sizex=30;
menus[1].sizey=10;
menus[1].text_size=20;
MenuItem_colors(&menus[1], TEAM_COL2(1), TEAM_COL(1));

menus[2].state=1;
strcpy(menus[2].text,"^fBlue Team");
strcpy(menus[2].texture,"aqua.tga");
menus[2].OnClick=menu_game;
menus[2].param=2;
menus[2].px=50;
menus[2].py=50;
menus[2].sizex=30;
menus[2].sizey=10;
menus[2].text_size=20;
MenuItem_colors(&menus[2], TEAM_COL2(2), TEAM_COL(2));

menus[3].state=1;
strcpy(menus[3].text,"^fYellow Team");
strcpy(menus[3].texture,"aqua.tga");
menus[3].OnClick=menu_game;
menus[3].param=3;
menus[3].px=50;
menus[3].py=30;
menus[3].sizex=30;
menus[3].sizey=10;
menus[3].text_size=20;
MenuItem_colors(&menus[3], TEAM_COL2(3), TEAM_COL(3));

menus[4].state=1;
strcpy(menus[4].text,"^fBACK");
strcpy(menus[4].texture,"aqua.tga");
menus[4].OnClick=menu_main;
menus[4].px=50;
menus[4].py=10;
menus[4].sizex=30;
menus[4].sizey=10;
menus[4].text_size=25;
MenuItem_colors(&menus[4],0.1,0.1,0.1, 0.9,0.9,0.9);
}

void menu_ip(int dummy)
{
FILE *fp;
menu_networked_game=1;
menu=1;


menu_server_ip[0]=0;
fp=fopen("server.last","rt");
if(fp)
    {
    fgets(menu_server_ip,128,fp);
    if(strlen(menu_server_ip) && menu_server_ip[strlen(menu_server_ip)-1]=='\n')
	menu_server_ip[strlen(menu_server_ip)-1]=0;
    fclose(fp);
    }

menu_team=dummy;

MenuItem_init_all();

menus[0].state=1;
strcpy(menus[0].text,"^fServer IP");
strcpy(menus[0].texture,"aqua.tga");
menus[0].px=50;
menus[0].py=90;
menus[0].sizex=30;
menus[0].sizey=10;
menus[0].text_size=20;
MenuItem_colors(&menus[0],0.6,0.2,0.2, 0.9,0.2,0.2);

menus[1].state=1;
strcpy(menus[1].text,"^fBACK");
strcpy(menus[1].texture,"aqua.tga");
menus[1].OnClick=menu_main;
menus[1].px=50;
menus[1].py=10;
menus[1].sizex=30;
menus[1].sizey=10;
menus[1].text_size=25;
MenuItem_colors(&menus[1],0.1,0.1,0.1, 0.2,0.9,0.2);

menus[2].state=1;
strcpy(menus[2].text,"^f7");
strcpy(menus[2].texture,"aquab.tga");
menus[2].OnClick=menu_number;
menus[2].param='7';
menus[2].px=40;
menus[2].py=75;
menus[2].sizex=10;
menus[2].sizey=10;
menus[2].text_size=20;
MenuItem_colors(&menus[2],0.9,0.2,0.2, 0.2,0.9,0.2);

menus[3].state=1;
strcpy(menus[3].text,"^f8");
strcpy(menus[3].texture,"aquab.tga");
menus[3].OnClick=menu_number;
menus[3].param='8';
menus[3].px=50;
menus[3].py=75;
menus[3].sizex=10;
menus[3].sizey=10;
menus[3].text_size=20;
MenuItem_colors(&menus[3],0.9,0.2,0.2, 0.2,0.9,0.2);

menus[4].state=1;
strcpy(menus[4].text,"^f9");
strcpy(menus[4].texture,"aquab.tga");
menus[4].OnClick=menu_number;
menus[4].param='9';
menus[4].px=60;
menus[4].py=75;
menus[4].sizex=10;
menus[4].sizey=10;
menus[4].text_size=20;
MenuItem_colors(&menus[4],0.9,0.2,0.2, 0.2,0.9,0.2);

menus[5].state=1;
strcpy(menus[5].text,"^f4");
strcpy(menus[5].texture,"aquab.tga");
menus[5].OnClick=menu_number;
menus[5].param='4';
menus[5].px=40;
menus[5].py=65;
menus[5].sizex=10;
menus[5].sizey=10;
menus[5].text_size=20;
MenuItem_colors(&menus[5],0.9,0.2,0.2, 0.2,0.9,0.2);

menus[6].state=1;
strcpy(menus[6].text,"^f5");
strcpy(menus[6].texture,"aquab.tga");
menus[6].OnClick=menu_number;
menus[6].param='5';
menus[6].px=50;
menus[6].py=65;
menus[6].sizex=10;
menus[6].sizey=10;
menus[6].text_size=20;
MenuItem_colors(&menus[6],0.9,0.2,0.2, 0.2,0.9,0.2);

menus[7].state=1;
strcpy(menus[7].text,"^f6");
strcpy(menus[7].texture,"aquab.tga");
menus[7].OnClick=menu_number;
menus[7].param='6';
menus[7].px=60;
menus[7].py=65;
menus[7].sizex=10;
menus[7].sizey=10;
menus[7].text_size=20;
MenuItem_colors(&menus[7],0.9,0.2,0.2, 0.2,0.9,0.2);

menus[8].state=1;
strcpy(menus[8].text,"^f1");
strcpy(menus[8].texture,"aquab.tga");
menus[8].OnClick=menu_number;
menus[8].param='1';
menus[8].px=40;
menus[8].py=55;
menus[8].sizex=10;
menus[8].sizey=10;
menus[8].text_size=20;
MenuItem_colors(&menus[8],0.9,0.2,0.2, 0.2,0.9,0.2);

menus[9].state=1;
strcpy(menus[9].text,"^f2");
strcpy(menus[9].texture,"aquab.tga");
menus[9].OnClick=menu_number;
menus[9].param='2';
menus[9].px=50;
menus[9].py=55;
menus[9].sizex=10;
menus[9].sizey=10;
menus[9].text_size=20;
MenuItem_colors(&menus[9],0.9,0.2,0.2, 0.2,0.9,0.2);

menus[10].state=1;
strcpy(menus[10].text,"^f3");
strcpy(menus[10].texture,"aquab.tga");
menus[10].OnClick=menu_number;
menus[10].param='3';
menus[10].px=60;
menus[10].py=55;
menus[10].sizex=10;
menus[10].sizey=10;
menus[10].text_size=20;
MenuItem_colors(&menus[10],0.9,0.2,0.2, 0.2,0.9,0.2);

menus[11].state=1;
strcpy(menus[11].text,"^f0");
strcpy(menus[11].texture,"aquab.tga");
menus[11].OnClick=menu_number;
menus[11].param='0';
menus[11].px=40;
menus[11].py=45;
menus[11].sizex=10;
menus[11].sizey=10;
menus[11].text_size=20;
MenuItem_colors(&menus[11],0.9,0.2,0.2, 0.2,0.9,0.2);


menus[12].state=1;
strcpy(menus[12].text,"^f.");
strcpy(menus[12].texture,"aquab.tga");
menus[12].OnClick=menu_number;
menus[12].param='.';
menus[12].px=60;
menus[12].py=45;
menus[12].sizex=10;
menus[12].sizey=10;
menus[12].text_size=20;
MenuItem_colors(&menus[12],0.9,0.2,0.2, 0.2,0.9,0.2);

// Displays IP address
menus[13].state=1;
sprintf(menus[13].text,"^f%s",menu_server_ip);
strcpy(menus[13].texture,"aqua.tga");
menus[13].px=30;
menus[13].py=30;
menus[13].sizex=40;
menus[13].sizey=10;
menus[13].text_size=20;
MenuItem_colors(&menus[13],0.9,0.2,0.2, 0.9,0.2,0.2);

menus[14].state=1;
strcpy(menus[14].text,"^fdel");
strcpy(menus[14].texture,"aquab.tga");
menus[14].OnClick=menu_number;
menus[14].param=27;
menus[14].px=50;
menus[14].py=45;
menus[14].sizex=10;
menus[14].sizey=10;
menus[14].text_size=20;
MenuItem_colors(&menus[14],0.9,0.2,0.2, 0.2,0.9,0.2);

menus[15].state=1;
strcpy(menus[15].text,"^fConnect");
strcpy(menus[15].texture,"aqua.tga");
menus[15].OnClick=menu_color;
menus[15].param=dummy;
menus[15].px=70;
menus[15].py=30;
menus[15].sizex=30;
menus[15].sizey=10;
menus[15].text_size=20;
MenuItem_colors(&menus[15],0.9,0.2,0.2, 0.2,0.2,0.9);

}

void menu_game(int team)
{
menu_team=team;
menu=0;
start_game();
}
