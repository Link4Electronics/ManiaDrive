/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.
*/

#ifndef DONT_INCLUDE_HEADERS
#include "index.h"
#else
#include "headers/lensflare.h"
#endif

void raydium_lensflare_init(void)
{
int lf;

raydium_lensflare_disable();

for(lf=0;lf<RAYDIUM_MAX_LENSFLARES;lf++)
    raydium_lensflare_reset(lf);

raydium_log("Lens flare: OK");
}

void raydium_lensflare_enable(void)
{
raydium_lensflare_enabled_tag=1;
}

void raydium_lensflare_disable(void)
{
raydium_lensflare_enabled_tag=0;
}

signed char raydium_lensflare_isvalid(int lf)
{
if(lf<0||lf>RAYDIUM_MAX_LENSFLARES-1)
    return(0);

return(1);
}

void raydium_lensflare_reset(int lf)
{
int tex,fx;
float pos[3]={0,0,0};

if(!raydium_lensflare_isvalid(lf))return;

raydium_lensflare_off(lf);
raydium_lensflare_internal_created[lf]=0;
raydium_lensflare_name[lf][0]=0;

for(tex=0;tex<RAYDIUM_LENSFLARE_MAX_TEXTURES;tex++)
    raydium_lensflare_texture_id[lf][tex]=-1;

raydium_lensflare_move(lf,pos);
raydium_lensflare_appearance[lf]=0;

for(fx=0;fx<RAYDIUM_LENSFLARE_MAX_FX;fx++)
    {
    raydium_lensflare_fx_off(lf,fx);
    raydium_lensflare_fx_size_change(lf,fx,0);
    raydium_lensflare_fx_color_rgba(lf,fx,0,0,0,0);
    raydium_lensflare_fx_velocity_change(lf,fx,0);
    }
}

void raydium_lensflare_on(int lf)
{
if(!raydium_lensflare_isvalid(lf))return;

raydium_lensflare_internal_state[lf]=1;

// Use default textures when nothing was set before.
if(raydium_lensflare_texture_id[lf][0]<0)
    raydium_lensflare_texture_name(lf,NULL);
}

void raydium_lensflare_off(int lf)
{
if(!raydium_lensflare_isvalid(lf))return;

raydium_lensflare_internal_state[lf]=0;
}

void raydium_lensflare_switch(int lf)
{
if(!raydium_lensflare_isvalid(lf))return;

if(raydium_lensflare_internal_state[lf]<1)
    raydium_lensflare_on(lf);
else
    raydium_lensflare_off(lf);
}

void raydium_lensflare_texture_name(int lf, char *name)
{
char texture_name[RAYDIUM_MAX_NAME_LEN];
char custom_name[RAYDIUM_MAX_NAME_LEN];

if(!raydium_lensflare_isvalid(lf))return;

if(!name||strlen(name)==0)
    strcpy(custom_name,"");
else
    sprintf(custom_name,"_%s_",name);

// Load textures.
sprintf(texture_name,"LF%sglow.tga",custom_name);
raydium_lensflare_texture_id[lf][0]=raydium_texture_find_by_name(texture_name);

sprintf(texture_name,"LF%sstar.tga",custom_name);
raydium_lensflare_texture_id[lf][1]=raydium_texture_find_by_name(texture_name);

sprintf(texture_name,"LF%sstreak.tga",custom_name);
raydium_lensflare_texture_id[lf][2]=raydium_texture_find_by_name(texture_name);

sprintf(texture_name,"LF%shalo.tga",custom_name);
raydium_lensflare_texture_id[lf][3]=raydium_texture_find_by_name(texture_name);

sprintf(texture_name,"LF%sray.tga",custom_name);
raydium_lensflare_texture_id[lf][4]=raydium_texture_find_by_name(texture_name);

sprintf(texture_name,"LF%sanam.tga",custom_name);
raydium_lensflare_texture_id[lf][5]=raydium_texture_find_by_name(texture_name);

// Set textures to be HDR able.
if(raydium_hdr_state)
    {
    int fx;

    for(fx=0;fx<RAYDIUM_LENSFLARE_MAX_TEXTURES;fx++)
        raydium_hdr_texture(raydium_lensflare_texture_id[lf][fx],1);
    }
}

void raydium_lensflare_move(int lf, float *pos)
{
if(!raydium_lensflare_isvalid(lf))return;

raydium_lensflare_move_3f(lf,pos[0],pos[1],pos[2]);
}

void raydium_lensflare_move_3f(int lf, float x, float y, float z)
{
if(!raydium_lensflare_isvalid(lf))return;

raydium_lensflare_position[lf][0]=x;
raydium_lensflare_position[lf][1]=y;
raydium_lensflare_position[lf][2]=z;
}

void raydium_lensflare_move_relative_3f(int lf, float x, float y, float z)
{
if(!raydium_lensflare_isvalid(lf))return;

raydium_lensflare_position[lf][0]+=x;
raydium_lensflare_position[lf][1]+=y;
raydium_lensflare_position[lf][2]+=z;
}

signed char raydium_lensflare_fx_isvalid(int fx)
{
if(fx<0||fx>RAYDIUM_LENSFLARE_MAX_FX-1)
    return(0);

return(1);
}

void raydium_lensflare_fx_on(int lf, int fx)
{
if(!raydium_lensflare_fx_isvalid(fx))return;

raydium_lensflare_fx_internal_state[lf][fx]=1;
}

void raydium_lensflare_fx_off(int lf, int fx)
{
if(!raydium_lensflare_fx_isvalid(fx))return;

raydium_lensflare_fx_internal_state[lf][fx]=0;
}

void raydium_lensflare_fx_switch(int lf, int fx)
{
if(!raydium_lensflare_fx_isvalid(fx))return;

if(raydium_lensflare_fx_internal_state[lf][fx]<1)
    raydium_lensflare_fx_on(lf,fx);
else
    raydium_lensflare_fx_off(lf,fx);
}

void raydium_lensflare_fx_size_change(int lf, int fx, float s)
{
if(!raydium_lensflare_fx_isvalid(fx))return;

raydium_lensflare_fx_size[lf][fx]=s;
}

void raydium_lensflare_fx_color_change(int lf, int fx, float r, float g, float b)
{
if(!raydium_lensflare_fx_isvalid(fx))return;

raydium_lensflare_fx_color[lf][fx][0]=r;
raydium_lensflare_fx_color[lf][fx][1]=g;
raydium_lensflare_fx_color[lf][fx][2]=b;
}

void raydium_lensflare_fx_alpha_change(int lf, int fx, float a)
{
if(!raydium_lensflare_fx_isvalid(fx))return;

raydium_lensflare_fx_color[lf][fx][3]=a;
}

void raydium_lensflare_fx_color_rgba(int lf, int fx, float r, float g, float b, float a)
{
if(!raydium_lensflare_fx_isvalid(fx))return;

raydium_lensflare_fx_color_change(lf,fx,r,g,b);
raydium_lensflare_fx_alpha_change(lf,fx,a);
}

void raydium_lensflare_fx_velocity_change(int lf, int fx, float v)
{
if(!raydium_lensflare_fx_isvalid(fx))return;

raydium_lensflare_fx_velocity[lf][fx]=v;
}

signed char raydium_lensflare_internal_load(int lf, char *filename)
{
FILE *fp; // file handle
char v[RAYDIUM_MAX_NAME_LEN]; // variable name
char s[RAYDIUM_MAX_NAME_LEN]; // string value
float f[6]; // float value
int type,size;

if(!raydium_lensflare_isvalid(lf))return(0);

if((fp=raydium_file_fopen(filename,"rt")))
    {
    raydium_lensflare_reset(lf);
    raydium_lensflare_on(lf);

    while((type=raydium_parser_read(v,s,f,&size,fp))!=RAYDIUM_PARSER_TYPE_EOF)
        {
        if(type==RAYDIUM_PARSER_TYPE_STRING&&strcmp(v,"texture")==0)
            {
            raydium_lensflare_texture_name(lf,s);
            continue;
            }
        if(type==RAYDIUM_PARSER_TYPE_FLOAT&&strcmp(v,"main")==0)
            {
            raydium_lensflare_fx_on(lf,0);
            raydium_lensflare_fx_size_change(lf,0,f[0]);
            raydium_lensflare_fx_velocity_change(lf,0,f[1]);
            raydium_lensflare_fx_color_rgba(lf,0,f[2],f[3],f[4],f[5]);
            continue;
            }
        if(type==RAYDIUM_PARSER_TYPE_FLOAT&&strcmp(v,"rays")==0)
            {
            raydium_lensflare_fx_on(lf,1);
            raydium_lensflare_fx_size_change(lf,1,f[0]);
            raydium_lensflare_fx_velocity_change(lf,1,f[1]);
            raydium_lensflare_fx_color_rgba(lf,1,f[2],f[3],f[4],f[5]);
            continue;
            }
        if(type==RAYDIUM_PARSER_TYPE_FLOAT&&strcmp(v,"streak")==0)
            {
            raydium_lensflare_fx_on(lf,2);
            raydium_lensflare_fx_size_change(lf,2,f[0]);
            raydium_lensflare_fx_velocity_change(lf,2,f[1]);
            raydium_lensflare_fx_color_rgba(lf,2,f[2],f[3],f[4],f[5]);
            continue;
            }
        if(type==RAYDIUM_PARSER_TYPE_FLOAT&&strcmp(v,"halo")==0)
            {
            raydium_lensflare_fx_on(lf,3);
            raydium_lensflare_fx_size_change(lf,3,f[0]);
            raydium_lensflare_fx_velocity_change(lf,3,f[1]);
            raydium_lensflare_fx_color_rgba(lf,3,f[2],f[3],f[4],f[5]);
            continue;
            }
        if(type==RAYDIUM_PARSER_TYPE_FLOAT&&strcmp(v,"orbs")==0)
            {
            raydium_lensflare_fx_on(lf,4);
            raydium_lensflare_fx_size_change(lf,4,f[0]);
            raydium_lensflare_fx_velocity_change(lf,4,f[1]);
            raydium_lensflare_fx_color_rgba(lf,4,f[2],f[3],f[4],f[5]);
            continue;
            }
        if(type==RAYDIUM_PARSER_TYPE_FLOAT&&strcmp(v,"glow")==0)
            {
            raydium_lensflare_fx_on(lf,5);
            raydium_lensflare_fx_size_change(lf,5,f[0]);
            raydium_lensflare_fx_velocity_change(lf,5,f[1]);
            raydium_lensflare_fx_color_rgba(lf,5,f[2],f[3],f[4],f[5]);
            continue;
            }
        if(type==RAYDIUM_PARSER_TYPE_FLOAT&&strcmp(v,"star")==0)
            {
            raydium_lensflare_fx_on(lf,6);
            raydium_lensflare_fx_size_change(lf,6,f[0]);
            raydium_lensflare_fx_velocity_change(lf,6,f[1]);
            raydium_lensflare_fx_color_rgba(lf,6,f[2],f[3],f[4],f[5]);
            continue;
            }
        if(type==RAYDIUM_PARSER_TYPE_FLOAT&&strcmp(v,"anam")==0)
            {
            raydium_lensflare_fx_on(lf,7);
            raydium_lensflare_fx_size_change(lf,7,f[0]);
            raydium_lensflare_fx_velocity_change(lf,7,f[1]);
            raydium_lensflare_fx_color_rgba(lf,7,f[2],f[3],f[4],f[5]);
            continue;
            }
        }

    fclose(fp);
    }

return(1);
}

int raydium_lensflare_find(char *name)
{
int lf;

for(lf=0;lf<RAYDIUM_MAX_LENSFLARES;lf++)
    if(!strcmp(raydium_lensflare_name[lf],name))
        return(lf);

return(-1);
}

int raydium_lensflare_create(char *name, char *filename)
{
int lf,i;

if(!name || !strlen(name))
    return -1;

lf=raydium_lensflare_find(name);

if(lf<0)
    {
    // search for free slot
    for(i=0;i<RAYDIUM_MAX_LENSFLARES;i++)
        if(!raydium_lensflare_internal_created[i])
            {
            lf=i;
            break;
            }
    if(i==RAYDIUM_MAX_LENSFLARES)
        {
        raydium_log("ERROR: lensflare: cannot create, no more free slots");
        return -1;
        }
    }

if(raydium_lensflare_internal_load(lf,filename))
    {
    raydium_lensflare_internal_created[lf]=1;
    strcpy(raydium_lensflare_name[lf],name);
    }
else
    return(-1);

return(lf);
}

signed char raydium_lensflare_internal_point_is_occluded(float x, float y, float z)
{
GLint viewport[4];
GLdouble mvmatrix[16],projmatrix[16];
GLdouble winx,winy,winz;
GLdouble flarez;
GLfloat bufferz;

glGetIntegerv(GL_VIEWPORT,viewport);
glGetDoublev(GL_MODELVIEW_MATRIX,mvmatrix);
glGetDoublev(GL_PROJECTION_MATRIX,projmatrix);

gluProject(x,y,z,mvmatrix,projmatrix,viewport,&winx,&winy,&winz);
flarez=winz;

glReadPixels(winx,winy,1,1,GL_DEPTH_COMPONENT,GL_FLOAT,&bufferz);

if(bufferz<flarez)
    return(1);
else
    return(0);
}

void raydium_lensflare_fx_internal_draw(int lf, int fx, int id, int tex, float d, float cx, float cy, float vx, float vy, float g, float pt)
{
if(raydium_lensflare_fx_internal_state[lf][fx])
    {
    static float s=0; // size
    static float m=0; // mulpos
    static float t=0; // turn angle
    static float a=0; // anam
    float fxs=raydium_lensflare_fx_size[lf][fx];
    float fxv=raydium_lensflare_fx_velocity[lf][fx];
    float x1,y1,x2,y2;

    switch(id)
        {
        case 1: // FX group 0
            s=fxs*(float)(40.0*(1.0-(d/75.0)))*pt;
            m=d;
            t=fxv*(-g);
            break;
        case 2: // FX group 1
            s=fxs*(float)(60.0*(1.0-(d/75.0)))*pt;
            m=d;
            t=fxv*g*0.5;
            break;
        case 3:
            t=fxv*(-g)*0.5;
            break;
        case 4:
            t=fxv*g*0.6;
            break;
        case 5:
            t=fxv*(-g)*0.6;
            break;
        case 6: // FX group 2
            s=fxs*(float)(80.0*(1.0-(d/75.0)))*pt;
            m=d;
            t=fxv*g/5.0;
            break;
        case 7: // FX group 3
            s=fxs*(float)(2.0*cosf(g/2.0)/d)+(50.0*(1.0-(d/75.0)))*pt;
            m=d;
            t=fxv*g;
            break;
        case 8: // FX group 4
            s=fxs*(float)(4.5*(0.0+(d/75.0)))*pt;
            m=d*1.34;
            t=fxv*g;
            break;
        case 9:
            s=fxs*(float)(2.5*(0.0+(d/75.0)))*pt;
            m=d*0.4;
            t=fxv*(-g);
            break;
        case 10:
            s=fxs*(float)(9.5*(0.0+(d/75.0)))*pt;
            m=d*0.1;
            t=fxv*g;
            break;
        case 11:
            s=fxs*(float)(4.5*(0.0+(d/75.0)))*pt;
            m=-d*0.1;
            t=fxv*(-g);
            break;
        case 12:
            s=fxs*(float)(2.0*(0.0+(d/75.0)))*pt;
            m=-d*0.4;
            t=fxv*g;
            break;
        case 13:
            s=fxs*(float)(9.0*(0.0+(d/75.0)))*pt;
            m=-d*0.6;
            t=fxv*(-g);
            break;
        case 14: // FX group 5
            s=fxs*(float)(75.0*(0.0+(d/75.0)))*pt;
            m=-d;
            t=fxv*g;
            break;
        case 15:
            s=fxs*fmodf((float)(75.0*(0.0+(d/75.0))),75)*pt;
            t=fxv*(-g);
            break;
        case 16: // FX group 6
            s=fxs*(float)(10.0*(0.0+(d/75.0)))*pt;
            m=-d*1.53;
            t=fxv*360.0*(vx*vy);
            break;
        case 17:
            s=fxs*(float)(20.0*(0.0+(d/75.0)))*pt;
            t=fxv*g*(float)vx*(float)vy*2;
            break;
        case 18:
            t=fxv*(-g)*(float)vx*(float)vy*3;
            break;
        case 19: // FX group 7
            s=1.2*pt;
            m=d;
            t=0;
            a=fxs*(25.0+50.0*(0.0+(d/50.0)));
            break;
        case 20:
            s=1.1*pt;
            a=fxs*(fxv*cosf(g)+24.0+50.0*(0.0+(d/50.0)));
            break;
        default:
            return;
        }

    switch(id)
        {
        case 19:
            x1=cx+(vx*m)-s*a;
            y1=cy+(vy*m)-s;
            x2=cx+(vx*m)+s*a;
            y2=cy+(vy*m)+s;
            break;
        case 20:
            x1=cx+(vx*m)+s*a;
            y1=cy+(vy*m)+s;
            x2=cx+(vx*m)-s*a;
            y2=cy+(vy*m)-s;
            break;
        default:
            x1=cx+(vx*m)-s;
            y1=cy+(vy*m)-s;
            x2=cx+(vx*m)+s;
            y2=cy+(vy*m)+s;
            break;
        }

    raydium_osd_color_rgba(
        raydium_lensflare_fx_color[lf][fx][0],
        raydium_lensflare_fx_color[lf][fx][1],
        raydium_lensflare_fx_color[lf][fx][2],
        raydium_lensflare_fx_color[lf][fx][3]);
    raydium_osd_draw_quad(raydium_lensflare_texture_id[lf][tex],x1,y1,x2,y2,t);
    }
}

void raydium_lensflare_draw(int lf)
{
if(raydium_lensflare_enabled_tag&&raydium_lensflare_internal_state[lf])
    {
    float cx,cy;
    float vx,vy;
    float posx,posy;
    float color[4];
    float dist;
    float pt;
    static float giro=0;
    signed char appear=1;

    // Check for POV of the camera visibility on the screen and test the appearance of the lens flare effect.
    if(raydium_math_point_unproject_3D(raydium_lensflare_position[lf][0],raydium_lensflare_position[lf][1],raydium_lensflare_position[lf][2],&posx,&posy)&&!raydium_lensflare_internal_point_is_occluded(raydium_lensflare_position[lf][0],raydium_lensflare_position[lf][1],raydium_lensflare_position[lf][2])&&posx<100&&posx>0&&posy<100&&posy>0)
        {
        // Lens flare appears.
        raydium_lensflare_appearance[lf]+=raydium_frame_time*7.0;
        if(raydium_lensflare_appearance[lf]>1)raydium_lensflare_appearance[lf]=1;
        }
    else
        {
        // Lens flare disappears.
        if(raydium_lensflare_appearance[lf]>0)
            {
            raydium_lensflare_appearance[lf]-=raydium_frame_time*4.0;
            appear=0;
            }

        if(raydium_lensflare_appearance[lf]<0)raydium_lensflare_appearance[lf]=0;
        }

    // Draw the lens flare effect.
    if(raydium_lensflare_appearance[lf]>0&&raydium_lensflare_appearance[lf]<=1)
        {
        // Store OSD color.
        memcpy(color,(char*)raydium_osd_color,4*sizeof(float));

        pt=raydium_lensflare_appearance[lf];
        cx=cy=50;
        giro+=4*raydium_frame_time;

        if(appear)
            {
            // Get vector.
            vx=((float)posx-(float)cx);
            vy=((float)posy-(float)cy);

            // Normalize
            dist=sqrt((vx*vx)+(vy*vy));
            vx=vx/dist;
            vy=vy/dist;

            // Remember last 2D screen position for the disappearance effect.
            raydium_lensflare_position[lf][3]=dist;
            raydium_lensflare_position[lf][4]=vx;
            raydium_lensflare_position[lf][5]=vy;
            }
        else
            {
            // Restore last 2D screen position from the appearance effect.
            dist=raydium_lensflare_position[lf][3];
            vx=raydium_lensflare_position[lf][4];
            vy=raydium_lensflare_position[lf][5];
            }

        glBlendFunc(GL_SRC_ALPHA,GL_ONE);

        // #1 Big main glow far away.
        raydium_lensflare_fx_internal_draw(lf,0,1,0,dist,cx,cy,vx,vy,giro,pt);

        // #2 First ray rotation clockwise.
        raydium_lensflare_fx_internal_draw(lf,1,2,4,dist,cx,cy,vx,vy,giro,pt);
        // #3 Second ray rotation counter clockwise.
        raydium_lensflare_fx_internal_draw(lf,1,3,4,dist,cx,cy,vx,vy,giro,pt);
        // #4 Third ray rotation clockwise.
        raydium_lensflare_fx_internal_draw(lf,1,4,4,dist,cx,cy,vx,vy,giro,pt);
        // #5 Fourth ray rotation counter clockwise.
        raydium_lensflare_fx_internal_draw(lf,1,5,4,dist,cx,cy,vx,vy,giro,pt);

        // #6 Big streaks far away.
        raydium_lensflare_fx_internal_draw(lf,2,6,2,dist,cx,cy,vx,vy,giro,pt);

        // #7 Big halo far away.
        raydium_lensflare_fx_internal_draw(lf,3,7,3,dist,cx,cy,vx,vy,giro,pt);

        // Draw the next effects only when the lens flare appears.
        // We don't see near effects during the lens flare disappearance time,
        // so we only need to show a kind of residual effect in the retina of the eyes here.
        if(appear)
            {
            // #8 First middle distance orb.
            raydium_lensflare_fx_internal_draw(lf,4,8,3,dist,cx,cy,vx,vy,giro,pt);
            // #9 Second middle distance orb.
            raydium_lensflare_fx_internal_draw(lf,4,9,3,dist,cx,cy,vx,vy,giro,pt);
            // #10 Third middle distance orb.
            raydium_lensflare_fx_internal_draw(lf,4,10,3,dist,cx,cy,vx,vy,giro,pt);
            // #11 Fourth middle distance orb.
            raydium_lensflare_fx_internal_draw(lf,4,11,3,dist,cx,cy,vx,vy,giro,pt);
            // #12 Fifth middle distance orb.
            raydium_lensflare_fx_internal_draw(lf,4,12,3,dist,cx,cy,vx,vy,giro,pt);
            // #13 Sixth middle distance orb.
            raydium_lensflare_fx_internal_draw(lf,4,13,3,dist,cx,cy,vx,vy,giro,pt);

            // #14 Big near front glow.
            raydium_lensflare_fx_internal_draw(lf,5,14,0,dist,cx,cy,vx,vy,giro,pt);
            // #15 Big near front halo.
            raydium_lensflare_fx_internal_draw(lf,5,15,3,dist,cx,cy,vx,vy,giro,pt);

            // #16 Small near front glow.
            raydium_lensflare_fx_internal_draw(lf,6,16,0,dist,cx,cy,vx,vy,giro,pt);
            // #17 Small near front star rotation clockwise.
            raydium_lensflare_fx_internal_draw(lf,6,17,1,dist,cx,cy,vx,vy,giro,pt);
            // #18 Small near front star rotation counter clockwise.
            raydium_lensflare_fx_internal_draw(lf,6,18,1,dist,cx,cy,vx,vy,giro,pt);

            // #19 Wide anamorphic far away.
            raydium_lensflare_fx_internal_draw(lf,7,19,5,dist,cx,cy,vx,vy,giro,pt);
            // #20 Slim anamorphic far away.
            raydium_lensflare_fx_internal_draw(lf,7,20,5,dist,cx,cy,vx,vy,giro,pt);
            }

        // Restore blend function.
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

        // Restore OSD color.
        memcpy(raydium_osd_color,color,4*sizeof(float));
        }
    }
}

void raydium_lensflare_draw_all(void)
{
if(raydium_lensflare_enabled_tag)
    {
    int lf;

    for(lf=0;lf<RAYDIUM_MAX_LENSFLARES;lf++)
        raydium_lensflare_draw(lf);
    }
}
