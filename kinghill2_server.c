// This is test6 mini server.

#define RAYDIUM_NETWORK_ONLY
#include "raydium/index.c"


#define PARTY_TIMEOUT   (5*60)
#define REMAINING_SEND  5
#define NET_REMAINING   (RAYDIUM_NETWORK_PACKET_BASE)


char str[RAYDIUM_NETWORK_PACKET_SIZE];
//char str_send[RAYDIUM_NETWORK_PACKET_SIZE];
//int freq;

void infos(void)
{
static int steps=0;
if(steps==10)
    {
    raydium_network_internal_server_delays_dump();
    steps=0;
    }
steps++;
}



void send_time(GLfloat step)
{
char buff[RAYDIUM_NETWORK_PACKET_SIZE];
static float t=0;
static float tt=PARTY_TIMEOUT;

t+=step;
tt-=step;
if(tt<0)
    tt=0;

if(t>=REMAINING_SEND)
    {
    t=0;
    //tt=PARTY_TIMEOUT;
    raydium_log("remaining time : broadcast (%.2f)",tt);
    memcpy(buff+RAYDIUM_NETWORK_PACKET_OFFSET,&tt,sizeof(float));
    //strcpy(buff+RAYDIUM_NETWORK_PACKET_OFFSET+sizeof(float),track);
    raydium_network_broadcast(NET_REMAINING,buff);
    }


}


void quit(int sig)
{
raydium_network_internal_dump();
exit(0);
}

void broad(int type,char *buff)
{
    raydium_network_broadcast(type,buff);
//    raydium_log("broadcasting %i",type);
}


void loop(void)
{
signed char type;
int id;

if(raydium_network_read_flushed(&id, &type, str)==RAYDIUM_NETWORK_DATA_OK)
    {
    raydium_network_broadcast(type,str);
//    printf(".");
    }
raydium_timecall_callback();
}


int main(int argc, char **argv)
{
raydium_network_only_init(argc,argv);

// Ugly hack, for now...
raydium_network_netcall_add(broad,RAYDIUM_NETWORK_PACKET_ODE_EXPLOSION,1);
raydium_network_netcall_add(broad,RAYDIUM_NETWORK_PACKET_ODE_NEWELEM,1);
raydium_network_netcall_add(broad,RAYDIUM_NETWORK_PACKET_ODE_REMELEM,1);
raydium_network_netcall_add(broad,RAYDIUM_NETWORK_PACKET_ODE_NIDWHO,1);
raydium_network_netcall_add(broad,(RAYDIUM_NETWORK_PACKET_BASE+0),1);
raydium_network_netcall_add(broad,(RAYDIUM_NETWORK_PACKET_BASE+1),1);
raydium_network_netcall_add(broad,(RAYDIUM_NETWORK_PACKET_BASE+2),1);
raydium_network_netcall_add(broad,(RAYDIUM_NETWORK_PACKET_BASE+3),1);
raydium_network_netcall_add(broad,(RAYDIUM_NETWORK_PACKET_BASE+4),1);
raydium_network_netcall_add(broad,(RAYDIUM_NETWORK_PACKET_BASE+5),1);
raydium_network_netcall_add(broad,(RAYDIUM_NETWORK_PACKET_BASE+6),1);
raydium_network_netcall_add(broad,(RAYDIUM_NETWORK_PACKET_BASE+7),1);

raydium_timecall_add(infos,1);
raydium_timecall_add(send_time,-1);


do{
loop();
#ifndef WIN32
usleep(1); // MUST disable this with real servers
#else
// Now 1ms, trouble detected with Win7 64 bits, where Sleep(0) was 
// making the time stop decreasing correctly ... :/
Sleep(1);
#endif
}while(1);

}
