/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.
*/

#ifndef DONT_INCLUDE_HEADERS
#include "index.h"
#else
#include "headers/cli.h"

#endif

char *raydium_version(void);

int raydium_init_cli_option(char *option, char *value)
{
int i;
int found;
char full_option[RAYDIUM_MAX_NAME_LEN];
char head[3];

strcpy(full_option  ,"--");
strcpy(full_option+2,option);

found=0;

for(i=1;i<raydium_init_argc;i++)
    if(!strcasecmp(full_option,raydium_init_argv[i]))
        found=i;

if(!found) return 0;    // option not found

if((found+1) >= raydium_init_argc)
    {
    if(value) strcpy(value,"");
    return 1;   // found, no value (last option of cli)
    }

strncpy(head,raydium_init_argv[found+1],3);
head[2]=0;

if(!strcasecmp(head,"--"))
    {
    if(value) strcpy(value,"");
    return 1;   // found, no value (value starts with -- : is a option)
    }

if(value) strcpy(value,raydium_init_argv[found+1]);
return 1;
}


int raydium_init_cli_option_default(char *option, char *value, char *default_value)
{
if(!raydium_init_cli_option(option,value))
    strcpy(value,default_value);
return 1;
}

void raydium_init_internal_homedir_find(char *app_name)
{
FILE *fp;
char *str;
char *str2;
#ifdef WIN32
char *empty="";
#endif

raydium_homedir[0]=0;
#ifndef WIN32
str=getenv("HOME");
str2="";
if(!str) // strange session :/
    {
    raydium_log("ERROR ! Unable to find HOME variable !");
    exit(100);
    }
#else
str=getenv("HOMEDRIVE");
str2=getenv("HOMEPATH");
if(!str || !str2)
    {
    str=empty;
    str2=getenv("windir");
        if(!str2)
            {
            raydium_log("ERROR ! Unable to find HOMEDRIVE/HOMEPATH/windir variables !");
            exit(100);
            }
    }
#endif

// right, 'str' is now the absolute home dir of user, let's build Raydium's one
// if not already given by user
if(!raydium_init_cli_option("home",raydium_homedir))
    {
    sprintf(raydium_homedir,"%s%s/.%s",str,str2,app_name);
    }

// is writable ?
fp=fopen(raydium_file_home_path("flag"),"wt");
if(!fp)
    {
    if(mkdir(raydium_homedir,S_IRUSR|S_IWUSR|S_IRWXU)<0)
        {
        raydium_log("ERROR ! Unable to create home dir: '%s'",raydium_homedir);
        exit(101);
        }
    }
else
    {
    fclose(fp);
    unlink(raydium_file_home_path("flag"));
    }

raydium_log("using '%s' as home dir",raydium_homedir);
}


#ifndef RAYDLL
void raydium_init_args_name(int argc, char **argv, char *app_name)
#else
void raydium_init_args_name_hack(int argc, char **argv, char *app_name)
#endif
{
int i;
char logfile[RAYDIUM_MAX_NAME_LEN];
char logmode[RAYDIUM_MAX_NAME_LEN];
FILE *fp;

raydium_init_argc=argc;
raydium_init_argv=malloc(RAYDIUM_MAX_CLI_ARGS*sizeof(char *)); // can't use argc, since raydium.cli may add new argv's

for(i=0;i<argc;i++)
    {
    raydium_init_argv[i]=malloc(strlen(argv[i])+1);
    strcpy(raydium_init_argv[i],argv[i]);
    }

fp=fopen("raydium.cli","rt");
if(fp)
    {
    char line[RAYDIUM_MAX_DIR_LEN];
    char next[RAYDIUM_MAX_DIR_LEN];
    char arg[RAYDIUM_MAX_NAME_LEN];

    while(fgets(line,RAYDIUM_MAX_DIR_LEN,fp))
        {
        raydium_parser_trim(line);
        if(!strlen(line))
            break;
        while(raydium_parser_cut(line,arg,next,' '))
            {
            raydium_init_argv[raydium_init_argc]=malloc(strlen(arg)+1);
            strcpy(raydium_init_argv[raydium_init_argc],arg);
            raydium_init_argc++;
            strcpy(line,next);
            }
        raydium_init_argv[raydium_init_argc]=malloc(strlen(line)+1);
        strcpy(raydium_init_argv[raydium_init_argc],line);
        raydium_init_argc++;
        }
    fclose(fp);
    }

/*
for(i=0;i<raydium_init_argc;i++)
    printf("[%s] ",raydium_init_argv[i]);
exit(1);
*/

raydium_log("Raydium 3D Game Engine");

if (raydium_init_cli_option("help",NULL))
    {
    fprintf(stdout,"%s\n","----------------------");
    fprintf(stdout,"%s\n","Command line switches:");

    fprintf(stdout,"%s\n","");

    fprintf(stdout,"  %s\n","--consolefont file       set console font name");
    fprintf(stdout,"  %s\n","--consoletexture file    set console background texture");

    fprintf(stdout,"%s\n","");

    fprintf(stdout,"  %s\n","--filter name            texture filter (none,bilinear,trilinear,aniso)");
    fprintf(stdout,"  %s\n","--compress               enable texture compression (if available)");
    fprintf(stdout,"  %s\n","--max-aniso int          set anisotropic filter maximum level");
    fprintf(stdout,"  %s\n","--npot                   enable non-power-of-two textures (if available)");

    fprintf(stdout,"%s\n","");

    fprintf(stdout,"  %s\n","--help                   this page");

    fprintf(stdout,"%s\n","");

    fprintf(stdout,"  %s\n","--joydev name            define joystick device (Linux)");
    fprintf(stdout,"  %s\n","--evdev name             define joystick forefeedback device (Linux)");
    fprintf(stdout,"  %s\n","--joy-emul name          force joystick emulation (mouse,keyboard)");

    fprintf(stdout,"%s\n","");


    fprintf(stdout,"  %s\n","--name name              network player name");
    fprintf(stdout,"  %s\n","--ode-rate int           ODE physics network refresh rate");
    fprintf(stdout,"  %s\n","--capture-rate int       default ODE replays capture rate");

    fprintf(stdout,"  %s\n","");

    fprintf(stdout,"  %s\n","--repository-disable     disable R3S repositories access");
    fprintf(stdout,"  %s\n","--repository-force       force downloading from R3S for every file");
    fprintf(stdout,"  %s\n","--repository-refresh     check for newer files from R3S repositories");

    fprintf(stdout,"%s\n","");

    fprintf(stdout,"  %s\n","--video-device name      input v4l device (Linux)");
    fprintf(stdout,"  %s\n","--video-size wwxhh       video size format (ex: 320x240)");

    fprintf(stdout,"%s\n","");

    fprintf(stdout,"  %s\n","--fullscreen             force fullscreen render");
    fprintf(stdout,"  %s\n","--window                 force window rendering");
    fprintf(stdout,"  %s\n","--window-fixed           fixed size window rendering (not resizeable)");
    fprintf(stdout,"  %s\n","--xinerama-fullscreen    use all Xinerama screens for fullscreen (Linux)");
    fprintf(stdout,"  %s\n","--xinerama-screen int    use a specific Xinerama screen (Linux)");

    fprintf(stdout,"%s\n","");

    fprintf(stdout,"  %s\n","--autoexec file.php      run PHP script right after engine init");
    fprintf(stdout,"  %s\n","--autoexec2 file.php     run PHP script before first frame");
    fprintf(stdout,"  %s\n","--home dir               set raydium home directory");
    fprintf(stdout,"  %s\n","--write-path dir         writing directory");
    fprintf(stdout,"  %s\n","--path dir               add dir to path search");
    fprintf(stdout,"  %s\n","--rayphp dir             php script directory");
    fprintf(stdout,"  %s\n","--package file           register package file");
    fprintf(stdout,"  %s\n","--files                  list all opened files (at exit)");
    fprintf(stdout,"  %s\n","--regs                   dump php registrations (at exit)");
    fprintf(stdout,"  %s\n","--history file           console history filename");
    fprintf(stdout,"  %s\n","--logfile file           generate a logfile");
    fprintf(stdout,"  %s\n","--log-time               add time to logs (seconds since launch)");
    fprintf(stdout,"  %s\n","--log-mode mode          standard log mode (stdout, stderr, none)");

    fprintf(stdout,"%s\n","");

    fprintf(stdout,"%s\n","Unknown switches will be ignored.");
    fprintf(stdout,"%s\n","You may also check application specific command line switches.");
    exit(0);
    }

if(raydium_init_cli_option("log-time",NULL))
    {
    raydium_file_log_time=1;
    raydium_timecall_clocks_per_sec=0; // until real init (we need it in log system)
    }
else
    raydium_file_log_time=0;

raydium_file_log_mode=RAYDIUM_LOG_MODE_STDOUT;
if(raydium_init_cli_option("log-mode",logmode))
    {
    if(!strcmp("stderr",logmode))
        raydium_file_log_mode=RAYDIUM_LOG_MODE_STERR;
    if(!strcmp("none",logmode))
        raydium_file_log_mode=RAYDIUM_LOG_MODE_NONE;
    }

if(raydium_init_cli_option("logfile",logfile))
    {
    raydium_log_file=fopen(logfile,"wt");
    if(!raydium_log_file) raydium_log("init: Warning: cannot open logfile (%s) with rw mode",logfile);
    }
else raydium_log_file=NULL;
// Write a log file on the iPhone OS by default, because the user is not able to see the output neither to specify program parameters.
#ifdef IPHONEOS
if(!raydium_log_file)
    raydium_log_file=fopen("/User/Raydium.log","wt");
#endif

raydium_log("version %s",raydium_version());
raydium_log("command line args: OK");

#ifndef RAYDIUM_NETWORK_ONLY
raydium_file_dirname(raydium_init_wd,raydium_init_argv[0]);
if(chdir(raydium_init_wd)!=-1)
    raydium_log("chdir to '%s': OK",raydium_init_wd);
else
    perror("chdir");
#endif

// Find user's home directory
raydium_init_internal_homedir_find(app_name);
raydium_php_rayphp_path[0]=0; // see raydium_php_init() / raydium_php_rayphp_path_change()
raydium_atexit_init();
}

#ifndef RAYDLL
void raydium_init_args (int argc, char **argv)
{
raydium_init_args_name(argc,argv,RAYDIUM_APP_SHORTNAME);
}
#else
void raydium_init_args_hack (int argc, char **argv)
{
raydium_init_args_name_hack(argc,argv,RAYDIUM_APP_SHORTNAME);
}
#endif
