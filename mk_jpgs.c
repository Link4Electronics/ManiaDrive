#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

// Raydium JPGS video generator (from a big bunch of JPEG files)
// See Raydium doc for examples.

// Compilation can be done with:
// - gcc mk_jpgs.c -lm -o mk_jpgs
// - ... or regular tools (odyncomp, Raydium SDK, ...)

char EXT[]=".jpg";
#define BUFFSIZE  4096

// Until our MinGW supports scandir and alpha sort, let's use this hack.
// (Badly) ported from Sparc64 release of MinGW ...
#ifdef WIN32
#include <sys/types.h>
#include <sys/stat.h>

/*
 * The DIRSIZ macro gives the minimum record length which will hold
 * the directory entry.  This requires the amount of space in struct dirent
 * without the d_name field, plus enough space for the name with a terminating
 * null byte (dp->d_namlen+1), rounded up to a 4 byte boundary.
 */
#undef DIRSIZ
#define DIRSIZ(dp) \
    ((sizeof (struct dirent) - (_MAX_FNAME+1)) + (((dp)->d_namlen+1 + 3) &~ 3))

#ifndef __P
#define __P(args) ()
#endif

int
scandir(dirname, namelist, select, dcomp)
	const char *dirname;
	struct dirent ***namelist;
	int (*select) __P((struct dirent *));
	int (*dcomp) __P((const void *, const void *));
{
	register struct dirent *d, *p, **names;
	register size_t nitems;
	struct stat stb;
	long arraysz;
	DIR *dirp;
	int n;

	if ((dirp = opendir(dirname)) == NULL)
		return(-1);

	n=0;
	while ((d=readdir(dirp)) !=NULL) n++;
	rewinddir(dirp);
	arraysz = (n);
	names = (struct dirent **)malloc(arraysz * sizeof(struct dirent *));
	if (names == NULL)
		return(-1);

	nitems = 0;
	while ((d = readdir(dirp)) != NULL) {
		if (select != NULL && !(*select)(d))
			continue;	/* just selected names */
		/*
		 * Make a minimum size copy of the data
		 */
		p = (struct dirent *)malloc(DIRSIZ(d));
		if (p == NULL)
			return(-1);
		p->d_ino = d->d_ino;
		p->d_reclen = d->d_reclen;
		p->d_namlen = d->d_namlen;
		strncpy(p->d_name, d->d_name,p->d_namlen + 1);
		/*
		 * Check to make sure the array has space left and
		 * realloc the maximum size.
		 */
		if (++nitems >= arraysz) {
			arraysz = stb.st_size / 12;
			names = (struct dirent **)realloc((char *)names,
				arraysz * sizeof(struct dirent *));
			if (names == NULL)
				return(-1);
		}
		names[nitems-1] = p;
	}
	closedir(dirp);
	if (nitems && dcomp != NULL)
		qsort(names, nitems, sizeof(struct dirent *), dcomp);
	*namelist = names;
	return(nitems);
}

int
alphasort(const void *d1, const void *d2)
{
	return (strcmp((*(const struct dirent *const *)d1)->d_name,
	    (*(const struct dirent *const *)d2)->d_name));
}
#endif

void copy(char *name,FILE *fpo)
{
FILE *fp;
char buf[BUFFSIZE];
size_t nmemb;

fp=fopen(name,"rb");
if(!fp)
    {
    printf("cannot open %s\n",name);
    exit(3);
    }


//while(fread(buf,BUFFSIZE,1,fp))
 while ((nmemb=fread(buf, 1, sizeof(buf), fp)) > 0)
    {
    fwrite(buf,1,nmemb,fpo);
    }

fclose(fp);
}

long filesize(char *name)
{
FILE *fp;
long ret;

fp=fopen(name,"rb");
if(!fp)
    {
    printf("cannot open %s\n",name);
    exit(3);
    }
fseek(fp,0,SEEK_END);
ret=ftell(fp);
fclose(fp);
return ret;
}

int is_jpg(const struct dirent *ent)
{
int l,m;
const char *name;

name=ent->d_name;
l=strlen(EXT);
m=strlen(name);

if(l<strlen(EXT))
    return 0;

if( name[m-l+0]==EXT[0] &&
    name[m-l+1]==EXT[1] &&
    name[m-l+2]==EXT[2] &&
    name[m-l+3]==EXT[3] )
        return 1;

return 0;
}

int main(int argc, char **argv)
{
float fps;
int sx,sy,total;
char out[512];
char head[100];
FILE *fpo;
struct dirent **namelist;
int i;
long offset;

if(argc!=5)
    {
    printf("This program will create a movie file (JPGS) for Raydium 3D Game Engine\n");
    printf("using all jpg files in the current directory.\n");
    printf("usage: %s fps sizex sizey output.jpgs\n",argv[0]);
    exit(0);
    }

fps=atof(argv[1]);
sx=atoi(argv[2]);
sy=atoi(argv[3]);
strcpy(out,argv[4]);

fpo=fopen(out,"wb");
if(!fpo)
    {
    printf("cannot open %s for writing\n",out);
    exit(1);
    }

total=scandir(".", &namelist, is_jpg, alphasort);
if(total<0)
    {
    perror("scandir");
    exit(15);
    }

printf("%i files found, creating header ...\n",total);
offset=0;
sprintf(head,"%f %i %i %i|",fps,sx,sy,total);
fprintf(fpo,"%s",head);

// header, second part
offset=0;
for(i=0;i<total;i++)
    {
    fprintf(fpo,"%li|",offset);
    offset+=filesize(namelist[i]->d_name);
    }

// body
printf("creating file ...\n");
for(i=0;i<total;i++)
    {
    copy(namelist[i]->d_name,fpo);
    }

fclose(fpo);
printf("%s created\n",out);
return 0;
}
