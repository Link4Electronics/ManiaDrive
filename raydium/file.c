/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.
*/

#ifndef DONT_INCLUDE_HEADERS
#include "index.h"
#else
#include "headers/file.h"
#include "headers/rayphp.h"
#endif

// proto
void raydium_path_resolv(char *in, char *out, char mode);


// far better than glibc's 'dirname' (and portable)
void raydium_file_dirname(char *dest,char *from)
{
char *c;
int n;

c=strrchr(from,'/'); // Unix
if(!c)
      c=strrchr(from,'\\'); // win32

if(!c)
    {
    strcpy(dest,"./");
    return;
    }
n=c-from;
memcpy(dest,from,n+1);
dest[n+1]=0;
}

void raydium_file_basename(char *dest,char *from)
{
char *c;
int n;

c=strrchr(from,'/'); // Unix
if(!c)
    c=strrchr(from,'\\'); // win32

if(!c)
    {
    strcpy(dest,from);
    return;
    }
n=(c-from+1);

if(n==(int)strlen(from))
    {
    dest[0]=0;
    return;
    }

strcpy(dest,from+n);
}

void raydium_file_ext(char *dest, char *from)
{
char name[RAYDIUM_MAX_DIR_LEN];
char *c;

dest[0]=0;

raydium_file_basename(name,from);
if( (c=strrchr(name,'.')) )
    if(c[1]!=0)
        strcpy(dest,c+1);
}

signed char raydium_file_isdir(char *path)
{
DIR * rphp;
rphp=opendir(path);

if (rphp)
	{
	closedir(rphp);
	return 1;
	}
return 0;
}

signed char raydium_file_directory_writable(char *path)
{
char file[RAYDIUM_MAX_NAME_LEN];
FILE *fp;

sprintf(file,"%s/RAYDIUM-WRITE-TEST.delme",path);
fp=fopen(file,"wb");
if(!fp)
    return 0;

fclose(fp);
unlink(file);
return 1;
}

signed char raydium_file_readable(char *filename)
{
FILE *fp;
fp=fopen(filename,"r");
if(!fp)
    return 0;
fclose(fp);
return 1;
}

void raydium_file_log_fopen_display(void)
{
int i;
char tmp[RAYDIUM_MAX_DIR_LEN];

raydium_log("List of all opened files:");
raydium_log("-------------------------");

for(i=0;i<raydium_file_log_fopen_index;i++)
    {
    if(raydium_file_log_fopen_status[i]==RAYDIUM_FILE_FOUND)
    	{
    	raydium_path_resolv(raydium_file_log_fopen[i],tmp,'r');
	if(strcmp(raydium_file_log_fopen[i],tmp))
	    raydium_log("> %s (%s)",raydium_file_log_fopen[i],tmp);
	else
	    raydium_log("> %s",raydium_file_log_fopen[i]);
	}
    else
    	raydium_log("> %s (**MISSING**)",raydium_file_log_fopen[i],tmp);
    }

}

void raydium_file_cache_flush(void)
{
int i,j;

i=j=0;
for(j=0;j<raydium_file_log_fopen_index;j++)
    {
    if (raydium_file_log_fopen_status[j]==RAYDIUM_FILE_FOUND)
    	{
        if (j==i)
            i++; // Nothing to copy.
        else
            {
            strcpy(raydium_file_log_fopen[i],raydium_file_log_fopen[j]);
            raydium_file_log_fopen_status[i]=raydium_file_log_fopen_status[j];
            i++;
            }
	}
    }
raydium_file_log_fopen_index=i;
}

FILE *raydium_file_fopen(char *file, char *mode)
{
FILE *fp=NULL;
int i;
char found=0;
char file2[RAYDIUM_MAX_DIR_LEN];

if(!file || !strlen(file))
    return NULL;

for(i=0;i<raydium_file_log_fopen_index;i++)
    if(!strcmp(raydium_file_log_fopen[i],file))
        {
        if(raydium_file_log_fopen_status[i]==RAYDIUM_FILE_NOT_FOUND)
	    {
	    if(strchr(mode,'w')) // a previous read failed, but now we're creating it (or trying to)
		raydium_file_log_fopen_status[i]=RAYDIUM_FILE_FOUND;
	    else
	    	return NULL;
	    }
        found=1;
        break;
        }

// use paths
raydium_path_resolv(file,file2,mode[0]);

do {
    // local mode ?
    if(strchr(mode,'l') || raydium_init_cli_option("repository-disable",NULL))
        {
	char mode2[16];
	strcpy(mode2,mode);
	raydium_parser_remove(mode2,'l'); // since win32 refuse to open a file with a unknown option ...
        fp= fopen(file2,mode2);
        break;
        }

    if(strchr(mode,'w'))
        {
        fp= fopen(file2,mode);
        break;
        }

    if( !raydium_init_cli_option("repository-refresh",NULL) &&
        !raydium_init_cli_option("repository-force",NULL) )
        {
        fp=fopen(file2,mode);
        if(fp) break;
        }
    raydium_rayphp_repository_file_get(file2);
    fp=fopen(file2,mode);
    }
while(0);

if(!found)
    {
    strcpy(raydium_file_log_fopen[raydium_file_log_fopen_index],file);
    if (fp)
        raydium_file_log_fopen_status[raydium_file_log_fopen_index]=RAYDIUM_FILE_FOUND;
    else
        {
        raydium_file_log_fopen_status[raydium_file_log_fopen_index]=RAYDIUM_FILE_NOT_FOUND;
        raydium_log("Error: Cannot Open %s file",file);
        }
    raydium_file_log_fopen_index++;
    }
return fp;
}

unsigned long raydium_file_sum_simple_mode(char *filename,char *mode)
{
unsigned long total=0;
unsigned long cpt=0;
int c;
FILE *fp;

fp=raydium_file_fopen(filename,mode);
if(!fp)
    {
    raydium_log("file simple sum: error: cannot open file '%s'",filename);
    return 0;
    }

while( (c=fgetc(fp))!=EOF )
    {
    c*=cpt;
    total+=c;
    cpt++;
    }

fclose(fp);
return total;
}

unsigned long raydium_file_sum_simple(char *filename)
{
return raydium_file_sum_simple_mode(filename,"rb");
}

char * raydium_file_home_path(char *file)
{
static char path[RAYDIUM_MAX_DIR_LEN];
sprintf(path,"%s/%s",raydium_homedir,file);
return path;
}

void raydium_file_home_path_cpy(char *file, char *dest)
{
strcpy(dest,raydium_file_home_path(file));
}

char *raydium_file_load(char *filename)
{
int len;
FILE *fp;
char *mem;

fp=raydium_file_fopen(filename,"rb");
if(!fp)
    return NULL;

fseek(fp,0,SEEK_END);
len=ftell(fp);
mem=malloc(len+1);
if(!mem)
    {
    fclose(fp);
    return NULL;
    }

mem[len]=0;
fseek(fp,0,SEEK_SET);

if(fread(mem,len,1,fp)!=1)
    {
    fclose(fp);
    free(mem);
    return NULL;
    }

fclose(fp);
return mem;
}

int raydium_file_binary_fgets(char *dest, int max, FILE *stream)
{
int c;
int i;

i=0;
do {
    c=fgetc(stream);

    if(c==EOF)
        {
        dest[i]=0;
        break;
        }

    dest[i]=c;

    if(c==0)
        break;

    i++;
    if(i>=(max-1))
        {
        dest[i]=0;
        break;
        }
    } while(1);

return i;
}

#ifdef WIN32
BOOL filetime_from_time(PFILETIME pFileTime, time_t Time)
{
    struct tm *pTM = localtime(&Time);
    SYSTEMTIME SystemTime;
    FILETIME LocalTime;

    if (pTM == NULL)
	return FALSE;

    SystemTime.wYear   = pTM->tm_year + 1900;
    SystemTime.wMonth  = pTM->tm_mon + 1;
    SystemTime.wDay    = pTM->tm_mday;
    SystemTime.wHour   = pTM->tm_hour;
    SystemTime.wMinute = pTM->tm_min;
    SystemTime.wSecond = pTM->tm_sec;
    SystemTime.wMilliseconds = 0;

    return SystemTimeToFileTime(&SystemTime, &LocalTime) &&
           LocalFileTimeToFileTime(&LocalTime, pFileTime);
}

// win32 utime() is unable to deal with directories.
// Using Perl workaround.
int raydium_file_utime(const char *filename, struct utimbuf *times)
{
    //dTHX;
    HANDLE handle;
    FILETIME ftCreate;
    FILETIME ftAccess;
    FILETIME ftWrite;
    struct utimbuf TimeBuffer;
    int rc;

    //filename = PerlDir_mapA(filename);
    rc = utime(filename, times);

    /* EACCES: path specifies directory or readonly file */
    if (rc == 0 || errno != EACCES /* || !IsWinNT() */)
	return rc;

    if (times == NULL) {
	times = &TimeBuffer;
	time(&times->actime);
	times->modtime = times->actime;
    }

    /* This will (and should) still fail on readonly files */
    handle = CreateFileA(filename, GENERIC_READ | GENERIC_WRITE,
                         FILE_SHARE_READ | FILE_SHARE_DELETE, NULL,
                         OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
    if (handle == INVALID_HANDLE_VALUE)
	return rc;

    if (GetFileTime(handle, &ftCreate, &ftAccess, &ftWrite) &&
	filetime_from_time(&ftAccess, times->actime) &&
	filetime_from_time(&ftWrite, times->modtime) &&
	SetFileTime(handle, &ftCreate, &ftAccess, &ftWrite))
    {
	rc = 0;
    }

    CloseHandle(handle);
    return rc;
}
#else
int raydium_file_utime(const char *filename, struct utimbuf *times)
{
return utime(filename,times);
}
#endif


signed char raydium_file_rm_rf(char *path)
{
DIR* dir;
struct dirent *ent;
struct stat s_stat;
char full_path[RAYDIUM_MAX_DIR_LEN];

dir=opendir(path);
if (dir)
	{
	while( (ent=readdir(dir)) )
		{
		sprintf(full_path,"%s/%s",path,ent->d_name);
		if(lstat(full_path,&s_stat)==-1)
			{
			perror("lstat");
			return 0;
			}

		if(S_ISDIR(s_stat.st_mode))
 			{
			if(strcmp(".",(ent->d_name)) && strcmp("..",(ent->d_name)))
				if(!raydium_file_rm_rf(full_path)) // error
					return 0;
			}
		else
			{
			if(unlink(full_path)==-1)
				{
				perror("unlink");
				return 0;
				}
			}
		}
	}
else
	{
	perror("opendir");
	return 0;
	}

closedir(dir);
if(rmdir(path)==-1)
	{
	perror("rmdir");
	return 0;
	}
return 1;
}
