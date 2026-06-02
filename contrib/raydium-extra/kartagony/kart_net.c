// Kartagony - network things :)

#define NET_GAME_STATE_T1		(RAYDIUM_NETWORK_PACKET_BASE+1)
#define NET_GAME_STATE_T2		(RAYDIUM_NETWORK_PACKET_BASE+2)
#define NET_GAME_STATE_T3		(RAYDIUM_NETWORK_PACKET_BASE+3)
#define NET_GAME_STATE_T4		(RAYDIUM_NETWORK_PACKET_BASE+4)
#define NET_GAME_STATE_BASE		NET_GAME_STATE_T1

#define NET_DEL_REQ			(RAYDIUM_NETWORK_PACKET_BASE+5)



typedef struct GameState
    {
    short scores[4];
    } GameState;
    
GameState game_state;


void delete_request(int elem)
{
char buff[RAYDIUM_NETWORK_PACKET_SIZE];
int nid;

nid=raydium_ode_element[elem].nid;

memcpy(buff+RAYDIUM_NETWORK_PACKET_OFFSET,&nid,sizeof(int));
raydium_network_write(NULL,raydium_network_uid,NET_DEL_REQ,buff);
}

void delete_request_callback(int type, char *buff)
{
int nid;
int elem;

memcpy(&nid,buff+RAYDIUM_NETWORK_PACKET_OFFSET,sizeof(int));

// let's look if this element is one of ours
elem=raydium_network_nid_element_find(nid);

if(elem>=0 && !raydium_ode_element[elem].distant)
    raydium_ode_element_delete(elem,1);
}

void net_init(void)
{
raydium_network_propag_add(NET_GAME_STATE_T1,&game_state.scores[0],sizeof(short));
raydium_network_propag_add(NET_GAME_STATE_T2,&game_state.scores[1],sizeof(short));
raydium_network_propag_add(NET_GAME_STATE_T3,&game_state.scores[2],sizeof(short));
raydium_network_propag_add(NET_GAME_STATE_T4,&game_state.scores[3],sizeof(short));

raydium_network_netcall_add(delete_request_callback,NET_DEL_REQ,1);
}
