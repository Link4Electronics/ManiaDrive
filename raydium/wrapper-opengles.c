/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.
*/

#ifndef WRAPPER_OPENGLES_C
#define WRAPPER_OPENGLES_C
#ifdef OPENGLES

#include <OpenGLES/ES1/gl.h>

/*
    The iPhone's processor and graphics capability should be able to handle a
    vertex number around 5000 without a serious frame decrease.
*/
#define OPENGLES_DEFAULT_RESERVE 5000
#define OPENGLES_SIZE_OF_FLOAT 4

GLenum ogles_mode=0;
signed char ogles_started=0;

signed char ogles_texture_coord_enabled=0;

unsigned int ogles_reserved_size=0;
unsigned int ogles_vertex_index=0;
unsigned int ogles_vertices=0;

GLfloat ogles_texture_coord_current[2];

GLfloat *ogles_texture_coord_array=0;
GLfloat *ogles_vertex_array=0;

void ogles_reset(void)
{
    /*
        Reserve space for the vertices, not actually necessary, because arrays
        will be resized automagically, but reserving a size upfront will
        hopefully improve the performance.
    */
    if(ogles_reserved_size==0)
        ogles_reserve(OPENGLES_DEFAULT_RESERVE);
    
    ogles_texture_coord_enabled=0;
    ogles_vertex_index=0;
    ogles_vertices=0;
}

void ogles_reserve(unsigned int size)
{
    // The maximum limit on the iPhone should be around 6000 vertices.
    if(size>6000)
        raydium_log("WARNING: OpenGL ES reserve size too high: %i",size);
    else
        raydium_log("OpenGL ES reserve size: %i",size);
    
    ogles_texture_coord_array=(GLfloat*)realloc(ogles_texture_coord_array,2*size*OPENGLES_SIZE_OF_FLOAT);
    ogles_vertex_array=(GLfloat*)realloc(ogles_vertex_array,3*size*OPENGLES_SIZE_OF_FLOAT);
    
    ogles_reserved_size=size;
    
    ogles_reset();
}

void glLightModeli(GLenum pname, GLint param)
{
    glLightModelf(pname, param);
}

void glTexCoord2f(GLfloat s, GLfloat t)
{
    ogles_texture_coord_current[0]=s;
    ogles_texture_coord_current[1]=t;
    
    ogles_texture_coord_enabled=1;
}

void glBegin(GLenum mode)
{
    switch(mode)
    {
        case GL_QUADS:
            ogles_mode=GL_TRIANGLE_FAN;
            break;
        default:
            ogles_mode=mode;
            break;
    }
    
    ogles_reset();
    ogles_started=1;
}

GLuint glGenLists(GLsizei range)
{
    return 0;
}

void glDeleteLists(GLuint list, GLsizei range)
{
    return;
}

void glEnd(void)
{
    if(ogles_vertices>0 && ogles_started)
    {
        if(ogles_texture_coord_enabled)
        {
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glTexCoordPointer(2,GL_FLOAT,0,&ogles_texture_coord_array[0]);
        }
        else
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3,GL_FLOAT,0,&ogles_vertex_array[0]);
        glDrawArrays(ogles_mode,0,ogles_vertices);
        glDisableClientState(GL_VERTEX_ARRAY);
        ogles_started=0;
    }
}

void glPolygonMode(GLenum face, GLenum mode)
{
    return;
}

void glVertex3f(GLfloat x, GLfloat y, GLfloat z)
{
    // If we hit the limit, we will increase the reserve by 10%.
    if(ogles_vertices>=ogles_reserved_size)
        ogles_reserve(ogles_reserved_size*1.1);
    
    if(ogles_texture_coord_enabled)
        memcpy(ogles_texture_coord_array+ogles_vertices*2,ogles_texture_coord_current,2*OPENGLES_SIZE_OF_FLOAT);
    
    ogles_vertex_array[ogles_vertex_index++]=x;
    ogles_vertex_array[ogles_vertex_index++]=y;
    ogles_vertex_array[ogles_vertex_index++]=z;
    
    ogles_vertices++;
}

void glCallList(GLuint list)
{
    return;
}

void glGetTexLevelParameteriv(GLenum target, GLint level, GLenum pname, GLint *params)
{
    return;
}

void glTexGeni(GLenum coord, GLenum pname, GLint param)
{
    return;
}

void glColor4fv(const GLfloat *v)
{
    glColor4f(v[0],v[1],v[2],v[3]);
}

void glEndList(void)
{
    return;
}

void glOrtho(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar)
{
    glOrthof(left, right, bottom, top, zNear, zFar);
}

void glFogi(GLenum pname, GLint param)
{
    glFogf(pname, param);
}

void glTexGenfv(GLenum coord, GLenum pname, const GLfloat *params)
{
    return;
}

void glNewList(GLuint list, GLenum mode)
{
    return;
}

void glGetDoublev(GLenum pname, GLdouble *params)
{
    glGetFloatv(pname, params);
}

#ifdef IPHONEOS

/*
    The original call to this function raises an exception and causes the
    termination of the application on the iPhone OS.
*/
#ifdef glMultiTexCoord2fARB
#undef glMultiTexCoord2fARB
#endif
void glMultiTexCoord2fARB(GLenum target, GLfloat s, GLfloat t)
{
    glTexCoord2f(s,t);
}

#include <OpenGL/glu.h>

// Undeclared in iGLU, because of OpenGL ES.
GLint gluBuild2DMipmaps(GLenum target, GLint internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *data)
{
    glTexParameterf(target,GL_GENERATE_MIPMAP, GL_TRUE); // (GL 1.4 based)
    glTexImage2D(target, 0, internalFormat, width, height, 0, format, type, data);

    return 0;
}

// Not seems to be available in the iOS SDK ... why ?
void glLoadMatrixd(const GLdouble *m)
{
GLfloat mf[16];
int i;
for(i=0;i<16;i++) mf[i]=m[i];
glLoadMatrixf(mf);
}

#endif
#endif
#endif
