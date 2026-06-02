/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.
*/

#ifndef DONT_INCLUDE_HEADERS
#include "index.h"
#else
#include "headers/log.h"
#endif

void raydium_console_line_add(char *format, ...);
void raydium_parser_trim_right(char *org);
unsigned long raydium_timecall_clock(void);


void raydium_log(char *format, ...)
{
char str[RAYDIUM_MAX_NAME_LEN];
va_list argptr;
int retlen;
int i,offset;
FILE *stream;

static unsigned long start;
unsigned long now;
float elapsed;

if(!start) start=raydium_timecall_clock();

now=raydium_timecall_clock();
if(raydium_timecall_clocks_per_sec)
    elapsed=(now-start)/(float)raydium_timecall_clocks_per_sec;
else
    elapsed=0;

va_start(argptr,format);
retlen = vsnprintf(str,RAYDIUM_MAX_NAME_LEN - 1, format,argptr);
va_end(argptr);

if(retlen < 0) retlen = 0;
str[retlen] = '\0';

stream=NULL;
if(raydium_file_log_mode==RAYDIUM_LOG_MODE_STDOUT)
    stream=stdout;
if(raydium_file_log_mode==RAYDIUM_LOG_MODE_STERR)
    stream=stderr;

if(raydium_file_log_time)
    {
    if(stream) fprintf(stream,"[%.6f] %s\n",elapsed,str);
    if(raydium_log_file) fprintf(raydium_log_file,"[%.6f] %s\n",elapsed,str);
    }
else
    {
    if(stream) fprintf(stream,"Raydium: %s\n",str);
    if(raydium_log_file) fprintf(raydium_log_file,"%s\n",str);
    }

raydium_parser_trim_right(str);
retlen=strlen(str);
offset=0;
for(i=0;i<retlen+1;i++)
    {
    if(str[i]=='\n' || str[i]==0)
        {
        str[i]=0;
        raydium_console_line_add("%s", str+offset);
        offset=i+1;
        }
    }
}

void raydium_log_atexit_quiet(void)
{
raydium_log_atexit_dump=0;
}
