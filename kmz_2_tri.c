/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.
*/

// Google Sketchup Kmz file converter.
// File->Export->3D Model select kmz file type.
// use kmz_2_tri.php script.
// Multi-texturing, Environnement Mapping not supported


#include "raydium/index.c"


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


enum{follow,menu}mode=menu;
int kmz=1;
int dae=1;
void gui_main(void);
GLfloat light_color[] = {1.0, 0.9, 0.8, 1.0};

int handle;
int wdae,wkmz;
int force_face=0;

char list[RAYDIUM_GUI_DATASIZE];

int filter(const struct dirent *ent)
{
const char *name;
char ext[RAYDIUM_MAX_NAME_LEN];

name=ent->d_name;

raydium_file_ext(ext,(char *)name);

if(kmz)
    if (!strcmp(ext,"kmz"))
        return 1;

if(dae)
    if (!strcmp(ext,"dae"))
        return 1;

return 0;
}

void refresh_list(char * list)
{
struct dirent **namelist;
int i,n;

    n=scandir(".",&namelist,filter,alphasort);
    if (n==0){
        strcpy(list,"no file found");
        return;
    }
    list[0]=0;
    for(i=0;i<n;i++)
    {
        strcat(list,namelist[i]->d_name);
        strcat(list,"\n");
    }
}


void loadclick(raydium_gui_Object *w)
{
int handle;
char kmzname[RAYDIUM_MAX_NAME_LEN];
char triname[RAYDIUM_MAX_NAME_LEN];
char tmp[RAYDIUM_MAX_NAME_LEN];
float scale=1.0f;
int verbose=0;


    handle=raydium_gui_window_find("Model" );
    raydium_gui_combo_read(handle, raydium_gui_widget_find("list",handle),kmzname);
    //raydium_gui_read_name("Model","edtModelname",kmzname);
    raydium_gui_read_name("Model","edtScale",tmp);
    scale=atof(tmp);
    verbose=raydium_gui_read_name("Model","edtVerbose",tmp);
    if (verbose>0) verbose=4;
    
    force_face=raydium_gui_read_name("Model","edtTwoSides",tmp);
    if (raydium_file_readable(kmzname))
        {
        raydium_register_variable(kmzname,RAYDIUM_REGISTER_STR,"file_name");
        raydium_register_variable(&scale,RAYDIUM_REGISTER_FLOAT,"force_scale");
        raydium_register_variable(&verbose,RAYDIUM_REGISTER_INT,"verbose");
        raydium_register_variable(&force_face,RAYDIUM_REGISTER_INT,"force_face");
        raydium_file_basename(triname,kmzname);
        strcpy(strrchr(triname,'.'),".tri");
        raydium_php_exec("kmz_2_tri.php");
        raydium_register_variable_unregister_last();
        raydium_register_variable_unregister_last();
        raydium_register_variable_unregister_last();
        raydium_register_variable_unregister_last();
        if (strlen(kmzname))
            {
            if (raydium_ode_element_find("test")!=-1)
                raydium_ode_element_delete_name("test",1);

            raydium_ode_object_box_add("test",0,1,RAYDIUM_ODE_AUTODETECT,0,0,RAYDIUM_ODE_STATIC,0,triname);
            mode=follow;
            raydium_gui_hide();
            }
        }
}

void gui_main(){

    raydium_gui_theme_load("theme-raydium2.gui");

    if (raydium_gui_window_isvalid(handle=raydium_gui_window_find("info")))
        raydium_gui_window_delete_name("info");
    if (raydium_gui_window_isvalid(handle=raydium_gui_window_find("Model"))){
        char str[RAYDIUM_MAX_NAME_LEN];
            kmz=raydium_gui_check_read(handle,wkmz,str);
            dae=raydium_gui_check_read(handle,wdae,str);
            raydium_gui_window_delete_name("Model");
    }
    raydium_gui_widget_sizes(0,0,16);
    handle=raydium_gui_window_create("info",15,25,70,40);
    {
    int i=0;
    char * infos[]={"Raydium Sketchup to Raydium Converter",
                    "",
                    "1) Sketchup file->export->3D model .kmz format",
                    "",
                    "2) Select name and click Load",
                    "",
                    "3) Converted .tri file and created tga textures",
                    "Are now in your writable directory",
                    "Note: You can specify an output mesh scale",
                    "",0};
    char tmp[4];
        while(infos[i])
            {
            snprintf(tmp,4,"%2.2d",i);
            raydium_gui_label_create(tmp,handle,50,90-i*10,infos[i],0,0,0);
            i++;
            }

    }
    handle=raydium_gui_window_create("Model",45,75,55,25);

    refresh_list(list);

    raydium_gui_widget_sizes(25,4,18);
    raydium_gui_combo_create("list",handle,47,75,list,0);
    raydium_gui_widget_sizes(0,0,18);
    raydium_gui_label_create("Model Name",handle,25,85,"Model Name :",0,0,0);

    raydium_gui_widget_sizes(4,4,18);
    wkmz=raydium_gui_check_create("edtkmz",handle,55,52,".kmz",kmz);
    wdae=raydium_gui_check_create("edtdae",handle,75,52,".dae",dae);

    raydium_gui_check_create("edtVerbose",handle,65,18," Verbose",0);
    
    raydium_gui_check_create("edtTwoSides",handle,10,34," Force Single Face",force_face);

    raydium_gui_widget_sizes(0,0,18);
    raydium_gui_label_create("Scale",handle,28,25,"Scale :",0,0,0);
    raydium_gui_widget_sizes(8,4,18);
    raydium_gui_edit_create("edtScale",handle,42,17,"1.0");

    raydium_gui_widget_sizes(15,5,18);
    raydium_gui_button_create("btnOk",handle,10,50,"Load",loadclick);

    raydium_gui_show();
}


void display(void)
{

if(raydium_key_last==1027)
    exit(0);

if(raydium_mouse_click==2)
    {
    if (mode==menu)
        {
        raydium_gui_hide();
        mode=follow;
        }
    else
        {
        raydium_init_purgemem();
        gui_main();
        raydium_gui_show();
        mode=menu;
        }
    }
if (kmz!=raydium_gui_check_read(handle,wkmz,NULL)){
    refresh_list(list);
    kmz=raydium_gui_check_read(handle,wkmz,NULL);
    gui_main();
}
if (dae!=raydium_gui_check_read(handle,wdae,NULL)){
    refresh_list(list);
    dae=raydium_gui_check_read(handle,wdae,NULL);
    gui_main();
}

raydium_clear_frame();

if (mode==menu)
    raydium_camera_freemove(RAYDIUM_CAMERA_FREEMOVE_FIXED);
else
    raydium_camera_freemove(RAYDIUM_CAMERA_FREEMOVE_NORMAL);

raydium_ode_draw_all(RAYDIUM_ODE_DRAW_NORMAL);

if(raydium_key[GLUT_KEY_F1])
    raydium_ode_draw_all(RAYDIUM_ODE_DRAW_DEBUG);

//raydium_log("%d",raydium_gui_button_clicked());

raydium_osd_logo("logo.tga");

raydium_rendering_finish();
}



int main(int argc, char **argv)
{
    raydium_init_args(argc,argv);
    raydium_window_create(640,480,RAYDIUM_RENDERING_WINDOW,"Sketchup .kmz to Raydium .tri converter");
    raydium_texture_filter_change(RAYDIUM_TEXTURE_FILTER_TRILINEAR);
    raydium_window_view_perspective(60,0.01,2500);
    raydium_fog_disable();
    raydium_window_view_update();

    raydium_background_color_change(1,0.9,0.7,1);

    raydium_light_enable();
    raydium_light_on(0);
    raydium_light_conf_7f(0,50,150,200,1000000,1,0.9,0.7);

    raydium_gui_theme_load("theme-raydium2.gui");

    gui_main();

    raydium_callback(&display);
    return(0);
}
// EOF
