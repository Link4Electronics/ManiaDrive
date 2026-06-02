/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.
*/

#ifndef DONT_INCLUDE_HEADERS
#include "index.h"
#else
#include "headers/rayphp.h"
#endif


void raydium_php_rayphp_path_change(char *path)
{
strcpy(raydium_php_rayphp_path,path);
}

char *raydium_php_internal_rayphp_path(char *file)
{
static char ret[RAYDIUM_MAX_NAME_LEN*2];
sprintf(ret,"%s/%s",raydium_php_rayphp_path,file);
return ret;
}

int raydium_rayphp_repository_file_get(char *path)
{
int status;
int force=0;
char file[RAYDIUM_MAX_NAME_LEN];

if(strlen(path)==0)
    {
    raydium_log("RayPHP: error: empty filename");
    return 0;
    }

#ifndef RAYDIUM_NETWORK_ONLY
raydium_render_loading();
#endif

strcpy(file,path);

if(raydium_init_cli_option("repository-force",NULL)) force=1;

raydium_register_variable(file,RAYDIUM_REGISTER_STR,"filename");
raydium_register_variable(&status,RAYDIUM_REGISTER_INT,"status");
raydium_register_variable(&force,RAYDIUM_REGISTER_INT,"force");

raydium_php_exec(raydium_php_internal_rayphp_path("getfile.php"));

raydium_register_variable_unregister_last();
raydium_register_variable_unregister_last();
raydium_register_variable_unregister_last();

if(status<0) status=0;

return status;
}


// put "path" file (and dependencies, if asked)
int raydium_rayphp_repository_file_put(char *path, int depends)
{
char file[RAYDIUM_MAX_NAME_LEN];
int status;

#ifndef RAYDIUM_NETWORK_ONLY
raydium_render_loading();
#endif

strcpy(file,path);
raydium_register_variable(file,RAYDIUM_REGISTER_STR,"filename");
raydium_register_variable(&status,RAYDIUM_REGISTER_INT,"status");
raydium_register_variable(&depends,RAYDIUM_REGISTER_INT,"depends");

raydium_php_exec(raydium_php_internal_rayphp_path("putfile.php"));

raydium_register_variable_unregister_last();
raydium_register_variable_unregister_last();
raydium_register_variable_unregister_last();

if(status<0) status=0;

return status;
}

int raydium_rayphp_repository_file_list(char *filter)
{
char *res;
int maxsize=1*1024*1024;
int status=0;
char ifilter[RAYDIUM_MAX_NAME_LEN];
int count=0;


res=malloc(maxsize);

if(!res)
    {
    raydium_log("rayphp: repository listing: malloc failed");
    return 0;
    }

strcpy(ifilter,filter);
raydium_register_variable(ifilter,RAYDIUM_REGISTER_STR,"filter");
raydium_register_variable(res,RAYDIUM_REGISTER_STR,"list");
raydium_register_variable(&status,RAYDIUM_REGISTER_INT,"status");
raydium_register_variable(&maxsize,RAYDIUM_REGISTER_INT,"size");

raydium_php_exec(raydium_php_internal_rayphp_path("listrepos.php"));

raydium_register_variable_unregister_last();
raydium_register_variable_unregister_last();
raydium_register_variable_unregister_last();
raydium_register_variable_unregister_last();

if(status)
    {
    int i,len;
    int last=0;

    len=strlen(res);

    for(i=0;i<len;i++)
        {
        if(res[i]=='\n')
            {
            res[i]=0;
            if(strlen(res+last))
                {
                raydium_log("%s",res+last);
                count++;
                }
            last=i+1;
            }
        }
    }
raydium_log("%i file(s)",count);
free(res);
return status;
}

signed char raydium_rayphp_http_test(void)
{
int status=0;

raydium_register_variable(&status,RAYDIUM_REGISTER_INT,"status");
raydium_php_exec(raydium_php_internal_rayphp_path("httptest.php"));
raydium_register_variable_unregister_last();

return status;
}

signed char raydium_rayphp_zip_extract(char *file, char *to)
{
char dest[RAYDIUM_MAX_DIR_LEN];
char ifile[RAYDIUM_MAX_DIR_LEN];
int status=0;

// should test the file before ? (and download it if needed)
// probably done by the caller itself ...

strcpy(dest,to);
strcpy(ifile,file);

raydium_register_variable(&status,RAYDIUM_REGISTER_INT,"status");
raydium_register_variable(dest,RAYDIUM_REGISTER_STR,"dest");
raydium_register_variable(ifile,RAYDIUM_REGISTER_STR,"file");
raydium_php_exec(raydium_php_internal_rayphp_path("zip_extract.php"));
raydium_register_variable_unregister_last();
raydium_register_variable_unregister_last();
raydium_register_variable_unregister_last();

return status;
}

signed char raydium_rayphp_zip_add(char *zip_file, char *full_file_name,char *file_name) //TODO correct decl
{
char zfile[RAYDIUM_MAX_DIR_LEN];
char fnfile[RAYDIUM_MAX_DIR_LEN];
char nfile[RAYDIUM_MAX_DIR_LEN];
int status=0;

// should test the file before ? (and download it if needed)
// probably done by the caller itself ...

strcpy(zfile,zip_file);
strcpy(fnfile,full_file_name);
strcpy(nfile,file_name);


raydium_register_variable(&status,RAYDIUM_REGISTER_INT,"status");
raydium_register_variable(zfile,RAYDIUM_REGISTER_STR,"zip_file");
raydium_register_variable(fnfile,RAYDIUM_REGISTER_STR,"file_full_path");
raydium_register_variable(nfile,RAYDIUM_REGISTER_STR,"file_name");
raydium_php_exec(raydium_php_internal_rayphp_path("zip_add.php"));
raydium_register_variable_unregister_last();
raydium_register_variable_unregister_last();
raydium_register_variable_unregister_last();
raydium_register_variable_unregister_last();

return status;
}

signed char raydium_rayphp_repository_defaults(char *def)
{
char *head="# This file was created by the application. You can change\n\
# whatever you want here (see rayphp/ directory for informations\n\n\
# Delete this file if you want to restore defaults.\n";
FILE *fp;

if(!raydium_file_readable(raydium_file_home_path("repositories.list")))
    {
    fp=fopen(raydium_file_home_path("repositories.list"),"wt");
    if(!fp)
        {
        raydium_log("rayphp: ERROR: Unable to create default repository (download) file");
        return 0;
        }
    fprintf(fp,"%s\n%s\n",head,def);
    fclose(fp);
    raydium_log("rayphp: default repository.list created");
    }

if(!raydium_file_readable(raydium_file_home_path("repositories.upload")))
    {
    fp=fopen(raydium_file_home_path("repositories.upload"),"wt");
    if(!fp)
        {
        raydium_log("rayphp: ERROR: Unable to create default repository (upload) file");
        return 0;
        }
    fprintf(fp,"%s\n%s\n",head,def);
    fclose(fp);
    raydium_log("rayphp: default repository.upload created");
    }
return 1;
}
