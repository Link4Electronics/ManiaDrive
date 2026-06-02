<?php
// This file is a binding generation demo, using SWIG:
// http://www.swig.org/
// You may generate bindings for many languages, such as
// Perl, Python, Tcl/Tk, Java, (Lua is on the road ...)

// To run this script, use PHP command line interface (CLI), it's provided by
// Raydium building scripts (ex: php/sapi/cli/php gen_bindings.php)

// Warning, this is an experimental and mostly unsupported feature, only
// usable with Python and Java, for now.
// Feedback's welcome.
// See output in raydium/swig directory and use compile wrapper using provided
// instruction, with a previously "./configured" Raydium engine.

// Generating bindings under win32 can be a bit complex ... Check Raydium
// website, you may find some "ready-to-use" binaries. Contact us on the
// forum otherwise.

// raycomp: standard compilation flags for Raydium. See ocomp.sh.
$raycomp="-L/usr/X11R6/lib/ -lXinerama -lGL -lGLU -lm -lopenal -lalut -ljpeg -Iraydium/ode/include/ raydium/ode/ode/src/libode.a -lvorbis -lvorbisfile -logg -Iraydium/php/ -Iraydium/php/include -Iraydium/php/main/ -Iraydium/php/Zend -Iraydium/php/TSRM raydium/php/libs/libphp5.a -lresolv -lcrypt -lz -lcurl -lxml2 -lGLEW";
$nobindings="//!NOBINDINGS";



//////////// functions

// This function will generate "i" file for SWIG.
// this file is needed for every language.
function generate_interface_file()
{
global $nobindings;

if(chdir("raydium/headers")===false)
    die("not in Raydium's root dir\n");

if(!file_exists("../swig") && mkdir("../swig")===false)
    die("cannot create raydium/swig subdirectory\n");

$fpi=fopen("../swig/raydium.i","wt");
if($fpi===false)
    die("cannot create interface file");
fwrite($fpi,"%module raydium\n%{\n#include \"../index.c\"\n%}\n");
//fwrite($fpi,"%module raydium\n%{\n#define NO_PHP_SUPPORT\n#include \"../index.c\"\n%}\n");
// type mappings (may not do it like this, but eh ...)
fwrite($fpi,"#define __rayapi\n");
fwrite($fpi,"%typedef unsigned int GLuint;\n");
fwrite($fpi,"%typedef unsigned int GLuint;\n");
fwrite($fpi,"%typedef unsigned int ALuint;\n");
fwrite($fpi,"%typedef unsigned int GLsizei;\n");
fwrite($fpi,"%typedef int GLint;\n");
fwrite($fpi,"%typedef int ALint;\n");
fwrite($fpi,"%typedef float GLfloat;\n");
fwrite($fpi,"%typedef double GLdouble;\n");
fwrite($fpi,"%typedef float ALfloat;\n");
fwrite($fpi,"%typedef float dReal;\n");
fwrite($fpi,"%module raydium\n%{\n#include \"../common.h\"\n%}\n\n%include \"../common.h\"\n");


$h=opendir(".");
while(false !== ($file = readdir($h)))
    if(substr($file,-2,2)==".h")
    {
    $fp=fopen($file,"rt");
    $l=fgets($fp);
    fclose($fp);
    if(substr($l,0,strlen($nobindings))==$nobindings)
        {
        echo "ignoring $file (asked by header)\n";
        continue;
        }

    $hd="../headers/$file";
    fwrite($fpi,"%module raydium\n%{\n#include \"$hd\"\n%}\n\n%include \"$hd\"\n");
    }

closedir($h);


chdir("..");
$h=opendir(".");
while(false !== ($file = readdir($h)))
    if(substr($file,-2,2)==".h" && $file!="common.h")
    {
    $fp=fopen($file,"rt");
    $l=fgets($fp);
    fclose($fp);
    if(substr($l,0,strlen($nobindings))==$nobindings)
        {
        echo "ignoring $file (asked by header)\n";
        continue;
        }

    $hd="../$file";
    fwrite($fpi,"%module raydium\n%{\n#include \"$hd\"\n%}\n\n%include \"$hd\"\n");
    }

closedir($h);


fclose($fpi);
chdir("..");
echo "\n";
}



// Will use interface file to generate Perl5 wrappers, using SWIG
function swig_wrappers_perl()
{
global $raycomp;
if(chdir("raydium/swig")===false)
    die("not in Raydium's root dir, cannot find swig dir\n");

passthru("swig -perl5 raydium.i",$ret);
echo "\n";
if($ret!=0)
    die("");


chdir("../..");
echo "Compile example (see ocomp.sh for up to date gcc args) for Perl5:\n";
echo "gcc -g -Wall -O2 -shared raydium/swig/raydium_wrap.c -o raydium/swig/raydium.so -I/usr/lib/perl5/5.8.3/i386-linux-thread-multi/CORE/ -D_REENTRANT -D_GNU_SOURCE -DTHREADS_HAVE_PIDS -fno-strict-aliasing -I/usr/local/include -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -I/usr/include/gdbm $raycomp";
echo "\n";
}

// Will use interface file to generate Python wrappers, using SWIG
function swig_wrappers_python()
{
global $raycomp;
$argvmap=<<<EOHC

%include exception.i

// Type mapping for grabbing a FILE * from Python
%typemap(python,in) FILE * {
  if (!PyFile_Check(\$input)) {
        PyErr_SetString(PyExc_TypeError, "Need a file!");
        return NULL;
        }
    \$1 = PyFile_AsFile(\$input);
}

// Grab a Python function object as a Python object.
%typemap(python,in) PyObject *pyfunc {
  if (!PyCallable_Check(\$input)) {
        PyErr_SetString(PyExc_TypeError, "Need a callable object!");
        return NULL;
        }
    \$1 = \$input;
}

%typemap(in) (int argc, char **argv) {
  int i;
  if (!PyList_Check(\$input)) {
    SWIG_exception(SWIG_ValueError, "Expecting a list");
  }
  $1 = PyList_Size(\$input);
  if (\$1 == 0) {
    SWIG_exception(SWIG_ValueError, "List must contain at least 1 element");
  }
  \$2 = (char **) malloc((\$1+1)*sizeof(char *));
  for (i = 0; i < \$1; i++) {
    PyObject *s = PyList_GetItem(\$input,i);
    if (!PyString_Check(s)) {
      free(\$2);
      SWIG_exception(SWIG_ValueError, "List items must be strings");
    }
    \$2[i] = PyString_AsString(s);
  }
  \$2[i] = 0;
}
EOHC;

if(chdir("raydium/swig")===false)
    die("not in Raydium's root dir, cannot find swig dir\n");

$fp=fopen("raydium.i","rt");
$contents = '';
while (!feof($fp))
    {
    $contents .= fread($fp, 8192);
    }
fclose($fp);

$fp=fopen("raydium.i","wt");
fwrite($fp,$argvmap."\n".$contents);
fclose($fp);

$contents=<<<EOHD
%{
PyObject *MainLoopPython;

static void MainLoopPythonCallback(void)
{
PyObject *result;
PyObject *arglist;

arglist = Py_BuildValue("()");

result=PyEval_CallObject(MainLoopPython,arglist);
Py_DECREF(arglist);
if (result == Py_None)
    {
    return;
    }

if (result)
    {
    Py_DECREF(result);
    }
return;
}

%}


%module raydium
%{
void SetMainLoopPython(PyObject *pyfunc)
{
MainLoopPython = pyfunc;
Py_INCREF(MainLoopPython);
raydium_callback(MainLoopPythonCallback);
}

%}

void SetMainLoopPython(PyObject *pyfunc);

void exit (int status);

EOHD;

$fp=fopen("raydium.i","at");
fwrite($fp,$contents);
fclose($fp);

passthru("swig -python raydium.i",$ret);
echo "\n";
if($ret!=0)
    die("");


chdir("../..");
echo "Compile example (see ocomp.sh for up to date gcc args) for Python:\n";
echo "gcc -g -Wall -O2 -shared raydium/swig/raydium_wrap.c -o raydium/swig/_raydium.so -I/usr/include/python2.6/ $raycomp";
echo "\n\n";
echo "You may need to adapt python devel lib path\n";
}


function swig_wrappers_java()
{
global $raycomp;
$maps=<<<EOS

%javaconst(1);

%typemap(jni) signed char, const signed char &          "jint"
%typemap(jtype) signed char, const signed char &        "int"
%typemap(jstype) signed char, const signed char &       "int"

%typemap(in) (int argc, char **argv) {
// This typemap create a dummy first arg, since Java does not provide
// the usual C argv[0]
    int i = 0;
    \$1 = (*jenv)->GetArrayLength(jenv, \$input);
    \$2 = (char **) malloc((\$1+2)*sizeof(char *));

    \$2[0] = malloc((strlen("(dummy)")+1)*sizeof(const char *));
    strcpy(\$2[0],"(dummy)");
    /* make a copy of each string */
    for (i = 0; i<\$1; i++) {
        jstring j_string = (jstring)(*jenv)->GetObjectArrayElement(jenv, \$input, i);
        const char * c_string = (*jenv)->GetStringUTFChars(jenv, j_string, 0);
        \$2[i+1] = malloc(strlen((c_string)+1)*sizeof(const char *));
        strcpy(\$2[i+1], c_string);
        (*jenv)->ReleaseStringUTFChars(jenv, j_string, c_string);
        (*jenv)->DeleteLocalRef(jenv, j_string);
    }
    \$2[i+1] = 0;
    \$1++;
}

%typemap(freearg) (int argc, char **argv) {
    int i;
    for (i=0; i<\$1-1; i++)
      free(\$2[i]);
    free(\$2);
}

%typemap(jni) (int argc, char **argv) "jobjectArray"
%typemap(jtype) (int argc, char **argv) "String[]"
%typemap(jstype) (int argc, char **argv) "String[]"
%typemap(javain) (int argc, char **argv) "\$javainput"

EOS;

if(chdir("raydium/swig")===false)
    die("not in Raydium's root dir, cannot find swig dir\n");


@mkdir("org",0755);
@mkdir("org/raydium",0755);

$fp=fopen("raydium.i","rt");
$contents = '';
while (!feof($fp))
    {
    $contents .= fread($fp, 8192);
    }
fclose($fp);

$fp=fopen("org/raydium/raydium.i","wt");
fwrite($fp,$maps."\n".$contents);
fclose($fp);


passthru("swig -java -package org.raydium org/raydium/raydium.i",$ret);
echo "\n";
if($ret!=0)
    die("");


chdir("../..");
echo "--------------------------------------------------\n";
echo "You must now compile the native Raydium lib.\n";
echo "Compile example (see ocomp.sh for up to date gcc args) for Java:\n";
echo "gcc -g -Wall -O2 -shared raydium/swig/org/raydium/raydium_wrap.c -o raydium/swig/libraydium.so -I/opt/java_ee_sdk-5_02/jdk/include/ -I/opt/java_ee_sdk-5_02/jdk/include/linux $raycomp";
echo "\n\n";
echo "You must probably change the two endings Java include paths (-I...) in this command line.\n";
echo "\n";
echo "You must also compile Java source code:\n";
echo "# cd raydium/swig\n";
echo "# javac org/raydium/*.java\n";
echo "# jar cf raydium.jar org\n";
echo "\n";
echo "To test all this:\n";
echo "# javac Test.java\n";
echo "# export LD_LIBRARY_PATH=.\n";
echo "# java Test\n";
echo "\n";
}

///////////// "main"

$arg1=$_SERVER['argv'][1];
if($arg1=="")
    die("Raydium bindings generator:\nUsage: gen_bindings.php [--java|--python]\n");

switch($arg1)
    {
    case "--python":
        generate_interface_file();
        swig_wrappers_python();
    break;

    case "--java":
        generate_interface_file();
        swig_wrappers_java();
    break;

    default:
        echo "error: invalid argument '$arg1'\n";
    }
