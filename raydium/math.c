/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.
*/

#ifndef DONT_INCLUDE_HEADERS
#include "index.h"
#else
#include "headers/math.h"
#include "headers/shadow.h"
#endif

GLfloat raydium_math_cos(GLfloat i)
{
return( (GLfloat)cos(i*PI/180) );
}

GLfloat raydium_math_sin(GLfloat i)
{
return( (GLfloat)sin(i*PI/180) );
}

GLfloat raydium_math_cos_inv(GLfloat i)
{
return(acos(i)*180/PI);
}

GLfloat raydium_math_sin_inv(GLfloat i)
{
return(asin(i)*180/PI);
}

void raydium_math_rotate(GLfloat *p, GLfloat rx, GLfloat ry, GLfloat rz, GLfloat *res)
{
res[0]= (p[0]*raydium_math_cos(ry)+(p[2]*raydium_math_cos(rx)+p[1]*raydium_math_sin(rx))*raydium_math_sin(ry))*raydium_math_cos(rz) + (p[1]*raydium_math_cos(rx)-p[2]*raydium_math_sin(rx))*raydium_math_sin(rz);
res[1]=-(p[0]*raydium_math_cos(ry)+(p[2]*raydium_math_cos(rx)+p[1]*raydium_math_sin(rx))*raydium_math_sin(ry))*raydium_math_sin(rz) + (p[1]*raydium_math_cos(rx)-p[2]*raydium_math_sin(rx))*raydium_math_cos(rz);
res[2]= (p[2]*raydium_math_cos(rx)+ p[1]*raydium_math_sin(rx))*raydium_math_cos(ry)-p[0]*raydium_math_sin(ry);
}

void raydium_math_normalize_vector4 (GLfloat * vector){
	GLfloat length;
	length=1.0/sqrtf(vector[0]*vector[0]+vector[1]*vector[1]+vector[2]*vector[2]);
	vector[0]*=length;
	vector[1]*=length;
	vector[2]*=length;
}

// pos: GLfloat[3], m: GLfloat[16]
void raydium_math_pos_to_matrix(GLfloat *pos, GLfloat *m)
{
m[0+4*0] = 1; m[0+4*1] = 0; m[0+4*2] = 0; m[0+4*3] = pos[0];
m[1+4*0] = 0; m[1+4*1] = 1; m[1+4*2] = 0; m[1+4*3] = pos[1];
m[2+4*0] = 0; m[2+4*1] = 0; m[2+4*2] = 1; m[2+4*3] = pos[2];
m[3+4*0] = 0; m[3+4*1] = 0; m[3+4*2] = 0; m[3+4*3] = 1;
}

void raydium_math_multiply_matrix4(GLfloat *matrix1,GLfloat *matrix2,GLfloat *result ){

	unsigned int i, j, k;
	for(i=0; i<4; i++)
	{
		for(j=0; j<4; j++)
		{
            GLfloat acc = 0;
		    for (k=0; k<4; k++)
		        acc += matrix1[i+k*4] * matrix2[k+j*4];

		    result[i+j*4] = acc;
		}
	}
}

// res: GLfloat[3]
void raydium_math_pos_get_modelview(GLfloat *res)
{
GLfloat tmp[16];
glGetFloatv(GL_MODELVIEW_MATRIX,tmp);
res[0]=tmp[12];
res[1]=tmp[13];
res[2]=tmp[14];
}

/* Unfinished !
// pos == res is safe
void raydium_trigo_carth_to_sphere(GLfloat *pos, GLfloat *res)
{
GLfloat r,G,T;

r=sqrt(pos[0]*pos[0] + pos[1]*pos[1] + pos[2]*pos[2]);
G=atan(pos[1]/pos[0]);
T=acos(pos[2]/r);

//printf("%f\n",r);
r=1;

if(pos[0]<0)
    res[0]=-r*cos(G);
else
    res[0]=+r*cos(G);

//res[1]=r*sin(G);
if(pos[0]<0)
    res[0]=-r*sin(T)*cos(G);
else
    res[0]=r*sin(T)*cos(G);
res[1]=r*sin(G)*cos(G);
res[2]=r*cos(T);

if(isnan(res[0])) res[0]=0;
if(isnan(res[1])) res[1]=0;
if(isnan(res[2])) res[2]=0;

}
*/

int raydium_math_pow2_next(int value)
{
int pows[]={0,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768,65536};
int pows_count=17; // 16 + first (0)
int i;

if(value>65536 || value<0)
    {
    raydium_log("trigo: value is outside of limits of this ugly function :/");
    }

for(i=0;i<pows_count;i++)
    if(pows[i]>=value)
        return pows[i];


// should never hit this point
raydium_log("trigo: raydium_math_pow2_next: ?!!");
return -1;
}

float raydium_math_angle_from_projections(float px, float py)
{
float realangle;
//using the arccos we get the "base angle"
realangle = acos ( px );
//check quadrants
//if the Y projection is negative, the angle has to be adjusted
if ( py < 0 )
    realangle = ( float ) 2 * PI - realangle;

return realangle;
}

#ifndef RAYDIUM_NETWORK_ONLY
signed char raydium_math_point_unproject_3D(GLfloat x, GLfloat y, GLfloat z, float* resx, float* resy)
{
GLdouble sx,sy,sz;
GLdouble modelMatrix[16];
GLdouble projectionMatrix[16];
GLint   viewport[4];

raydium_camera_replace();
glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
glGetDoublev(GL_PROJECTION_MATRIX, projectionMatrix);
glGetIntegerv(GL_VIEWPORT, viewport);
gluProject(x,y,z,modelMatrix,projectionMatrix,viewport,&sx,&sy,&sz);

sx=(sx-viewport[0])/viewport[2]*100;
sy=(sy-viewport[1])/viewport[3]*100;
(*resx)=sx;
(*resy)=sy;

if(sz<=1.0f)
    return 1; // above the camera

return 0; // behind the camera
}
#endif

void raydium_math_matrix4_identity_set(GLfloat * mati){
    mati[1]=mati[2]=mati[3]=
    mati[4]=mati[6]=mati[7]=
    mati[8]=mati[9]=mati[11]=
    mati[12]=mati[13]=mati[14]=0.0f;
    mati[0]=mati[5]=mati[10]=mati[15]=1.0f;
}

// Our matrix_inverse seems broken.
// This code works, thanks to Alexander Zaprjagaev (frustum@public.tsu.ru)
int _raydium_math_MatrixInverse(const float *m,float *out) {
    float   det;
    det = m[0] * m[5] * m[10];
    det += m[4] * m[9] * m[2];
    det += m[8] * m[1] * m[6];
    det -= m[8] * m[5] * m[2];
    det -= m[4] * m[1] * m[10];
    det -= m[0] * m[9] * m[6];
    if(det * det < 1e-25) return 0;
    det = 1.0 / det;
    out[0] =    (m[5] * m[10] - m[9] * m[6]) * det;
    out[1] =  - (m[1] * m[10] - m[9] * m[2]) * det;
    out[2] =    (m[1] * m[6] -  m[5] * m[2]) * det;
    out[3] = 0.0;
    out[4] =  - (m[4] * m[10] - m[8] * m[6]) * det;
    out[5] =    (m[0] * m[10] - m[8] * m[2]) * det;
    out[6] =  - (m[0] * m[6] -  m[4] * m[2]) * det;
    out[7] = 0.0;
    out[8] =    (m[4] * m[9] -  m[8] * m[5]) * det;
    out[9] =  - (m[0] * m[9] -  m[8] * m[1]) * det;
    out[10] =   (m[0] * m[5] -  m[4] * m[1]) * det;
    out[11] = 0.0;
    out[12] = - (m[12] * out[0] + m[13] * out[4] + m[14] * out[8]);
    out[13] = - (m[12] * out[1] + m[13] * out[5] + m[14] * out[9]);
    out[14] = - (m[12] * out[2] + m[13] * out[6] + m[14] * out[10]);
    out[15] = 1.0;
    return 1;
}

void raydium_math_invert_matrix4(GLfloat * matin, GLfloat * matout){

float det;

#define a(i,j) matin[(i-1)+(j-1)*4]
#define b(i,j) matout[(i-1)+(j-1)*4]

//http://gskinner.com/RegExr/
//regexp: a([1-9])([1-9])
//replace: a($1,$2)
// wxMaxima
// a:matrix([a11,a12,a13,a14],[a21,a22,a23,a24],[a31,a32,a33,a34],[a41,a42,a43,a44]);
// ia:a^^-1;
// det=
//subst(det,...

det=((a(1,1)*a(2,2)-a(1,2)*a(2,1))*a(3,3)+(a(1,3)*a(2,1)-a(1,1)*a(2,3))*a(3,2)+(a(1,2)*a(2,3)-a(1,3)*a(2,2))*a(3,1))*a(4,4)
    +((a(1,2)*a(2,1)-a(1,1)*a(2,2))*a(3,4)+(a(1,1)*a(2,4)-a(1,4)*a(2,1))*a(3,2)+(a(1,4)*a(2,2)-a(1,2)*a(2,4))*a(3,1))*a(4,3)
    +((a(1,1)*a(2,3)-a(1,3)*a(2,1))*a(3,4)+(a(1,4)*a(2,1)-a(1,1)*a(2,4))*a(3,3)+(a(1,3)*a(2,4)-a(1,4)*a(2,3))*a(3,1))*a(4,2)
    +((a(1,3)*a(2,2)-a(1,2)*a(2,3))*a(3,4)+(a(1,2)*a(2,4)-a(1,4)*a(2,2))*a(3,3)+(a(1,4)*a(2,3)-a(1,3)*a(2,4))*a(3,2))*a(4,1);


b(1,1)= ((a(2,2)*a(3,3)-a(2,3)*a(3,2))*a(4,4)+(a(2,4)*a(3,2)-a(2,2)*a(3,4))*a(4,3)+(a(2,3)*a(3,4)-a(2,4)*a(3,3))*a(4,2))/det;
b(1,2)= -((a(1,2)*a(3,3)-a(1,3)*a(3,2))*a(4,4)+(a(1,4)*a(3,2)-a(1,2)*a(3,4))*a(4,3)+(a(1,3)*a(3,4)-a(1,4)*a(3,3))*a(4,2))/det;
b(1,3)= ((a(1,2)*a(2,3)-a(1,3)*a(2,2))*a(4,4)+(a(1,4)*a(2,2)-a(1,2)*a(2,4))*a(4,3)+(a(1,3)*a(2,4)-a(1,4)*a(2,3))*a(4,2))/det;
b(1,4)= -((a(1,2)*a(2,3)-a(1,3)*a(2,2))*a(3,4)+(a(1,4)*a(2,2)-a(1,2)*a(2,4))*a(3,3)+(a(1,3)*a(2,4)-a(1,4)*a(2,3))*a(3,2))/det;

b(2,1)= -((a(2,1)*a(3,3)-a(2,3)*a(3,1))*a(4,4)+(a(2,4)*a(3,1)-a(2,1)*a(3,4))*a(4,3)+(a(2,3)*a(3,4)-a(2,4)*a(3,3))*a(4,1))/det;
b(2,2)= ((a(1,1)*a(3,3)-a(1,3)*a(3,1))*a(4,4)+(a(1,4)*a(3,1)-a(1,1)*a(3,4))*a(4,3)+(a(1,3)*a(3,4)-a(1,4)*a(3,3))*a(4,1))/det;
b(2,3)= -((a(1,1)*a(2,3)-a(1,3)*a(2,1))*a(4,4)+(a(1,4)*a(2,1)-a(1,1)*a(2,4))*a(4,3)+(a(1,3)*a(2,4)-a(1,4)*a(2,3))*a(4,1))/det;
b(2,4)= ((a(1,1)*a(2,3)-a(1,3)*a(2,1))*a(3,4)+(a(1,4)*a(2,1)-a(1,1)*a(2,4))*a(3,3)+(a(1,3)*a(2,4)-a(1,4)*a(2,3))*a(3,1))/det;

b(3,1)= ((a(2,1)*a(3,2)-a(2,2)*a(3,1))*a(4,4)+(a(2,4)*a(3,1)-a(2,1)*a(3,4))*a(4,2)+(a(2,2)*a(3,4)-a(2,4)*a(3,2))*a(4,1))/det;
b(3,2)= -((a(1,1)*a(3,2)-a(1,2)*a(3,1))*a(4,4)+(a(1,4)*a(3,1)-a(1,1)*a(3,4))*a(4,2)+(a(1,2)*a(3,4)-a(1,4)*a(3,2))*a(4,1))/det;
b(3,3)= ((a(1,1)*a(2,2)-a(1,2)*a(2,1))*a(4,4)+(a(1,4)*a(2,1)-a(1,1)*a(2,4))*a(4,2)+(a(1,2)*a(2,4)-a(1,4)*a(2,2))*a(4,1))/det;
b(3,4)= -((a(1,1)*a(2,2)-a(1,2)*a(2,1))*a(3,4)+(a(1,4)*a(2,1)-a(1,1)*a(2,4))*a(3,2)+(a(1,2)*a(2,4)-a(1,4)*a(2,2))*a(3,1))/det;

b(4,1)= -((a(2,1)*a(3,2)-a(2,2)*a(3,1))*a(4,3)+(a(2,3)*a(3,1)-a(2,1)*a(3,3))*a(4,2)+(a(2,2)*a(3,3)-a(2,3)*a(3,2))*a(4,1))/det;
b(4,2)= ((a(1,1)*a(3,2)-a(1,2)*a(3,1))*a(4,3)+(a(1,3)*a(3,1)-a(1,1)*a(3,3))*a(4,2)+(a(1,2)*a(3,3)-a(1,3)*a(3,2))*a(4,1))/det;
b(4,3)= -((a(1,1)*a(2,2)-a(1,2)*a(2,1))*a(4,3)+(a(1,3)*a(2,1)-a(1,1)*a(2,3))*a(4,2)+(a(1,2)*a(2,3)-a(1,3)*a(2,2))*a(4,1))/det;
b(4,4)= ((a(1,1)*a(2,2)-a(1,2)*a(2,1))*a(3,3)+(a(1,3)*a(2,1)-a(1,1)*a(2,3))*a(3,2)+(a(1,2)*a(2,3)-a(1,3)*a(2,2))*a(3,1))/det;

#undef a
#undef b

}

void raydium_math_translate_matrix4_3f(GLfloat x, GLfloat y, GLfloat z,GLfloat *matrix){
	matrix[12]=matrix[0]*x+matrix[4]*y+matrix[8]*z+matrix[12];
	matrix[13]=matrix[1]*x+matrix[5]*y+matrix[9]*z+matrix[13];
	matrix[14]=matrix[2]*x+matrix[6]*y+matrix[10]*z+matrix[14];
	matrix[15]=matrix[3]*x+matrix[7]*y+matrix[11]*z+matrix[15];
}

void raydium_math_rotate_matrix4_vector_angle(GLfloat angleInRadians, GLfloat x, GLfloat y, GLfloat z,GLfloat *matrix){
	//See page 191 of Elementary Linear Algebra by Howard Anton
	GLfloat m[16], rotate[16];
	GLfloat OneMinusCosAngle, CosAngle, SinAngle;
	GLfloat A_OneMinusCosAngle, C_OneMinusCosAngle;
	CosAngle=cosf(angleInRadians);			//Some stuff for optimizing code
	OneMinusCosAngle=1.0-CosAngle;
	SinAngle=sinf(angleInRadians);
	A_OneMinusCosAngle=x*OneMinusCosAngle;
	C_OneMinusCosAngle=z*OneMinusCosAngle;
	//Make a copy
	m[0]=matrix[0];
	m[1]=matrix[1];
	m[2]=matrix[2];
	m[3]=matrix[3];
	m[4]=matrix[4];
	m[5]=matrix[5];
	m[6]=matrix[6];
	m[7]=matrix[7];
	m[8]=matrix[8];
	m[9]=matrix[9];
	m[10]=matrix[10];
	m[11]=matrix[11];
	m[12]=matrix[12];
	m[13]=matrix[13];
	m[14]=matrix[14];
	m[15]=matrix[15];

	rotate[ 0]=x*A_OneMinusCosAngle+CosAngle;
	rotate[ 1]=y*A_OneMinusCosAngle+z*SinAngle;
	rotate[ 2]=z*A_OneMinusCosAngle-y*SinAngle;
	rotate[ 3]=0.0;

	rotate[ 4]=y*A_OneMinusCosAngle-z*SinAngle;
	rotate[ 5]=y*y*OneMinusCosAngle+CosAngle;
	rotate[ 6]=y*C_OneMinusCosAngle+x*SinAngle;
	rotate[ 7]=0.0;

	rotate[ 8]=x*C_OneMinusCosAngle+y*SinAngle;
	rotate[ 9]=y*C_OneMinusCosAngle-x*SinAngle;
	rotate[10]=z*C_OneMinusCosAngle+CosAngle;
	rotate[11]=0.0;
	//The last column of rotate[] is {0 0 0 1}

	matrix[0]=m[0]*rotate[0]+
		m[4]*rotate[1]+
		m[8]*rotate[2];

	matrix[4]=m[0]*rotate[4]+
		m[4]*rotate[5]+
		m[8]*rotate[6];

	matrix[8]=m[0]*rotate[8]+
		m[4]*rotate[9]+
		m[8]*rotate[10];

	//matrix[12]=matrix[12];

	matrix[1]=m[1]*rotate[0]+
		m[5]*rotate[1]+
		m[9]*rotate[2];

	matrix[5]=m[1]*rotate[4]+
		m[5]*rotate[5]+
		m[9]*rotate[6];

	matrix[9]=m[1]*rotate[8]+
		m[5]*rotate[9]+
		m[9]*rotate[10];

	//matrix[13]=matrix[13];

	matrix[2]=m[2]*rotate[0]+
		m[6]*rotate[1]+
		m[10]*rotate[2];

	matrix[6]=m[2]*rotate[4]+
		m[6]*rotate[5]+
		m[10]*rotate[6];

	matrix[10]=m[2]*rotate[8]+
		m[6]*rotate[9]+
		m[10]*rotate[10];

	//matrix[14]=matrix[14];

	matrix[3]=m[3]*rotate[0]+
		m[7]*rotate[1]+
		m[11]*rotate[2];

	matrix[7]=m[3]*rotate[4]+
		m[7]*rotate[5]+
		m[11]*rotate[6];

	matrix[11]=m[3]*rotate[8]+
		m[7]*rotate[9]+
		m[11]*rotate[10];

	//matrix[15]=matrix[15];
}

void raydium_math_crossproduct (GLfloat *pvector1, GLfloat *pvector2,GLfloat *normal){
	normal[0]=(pvector1[1]*pvector2[2])-(pvector1[2]*pvector2[1]);
	normal[1]=(pvector1[2]*pvector2[0])-(pvector1[0]*pvector2[2]);
	normal[2]=(pvector1[0]*pvector2[1])-(pvector1[1]*pvector2[0]);
}

void raydium_math_multiply_matrix4_vectors_array(GLfloat *matrix1,GLfloat *vect, int n_elems,GLfloat *result){
	unsigned int i, j, k;
	for(i=0; i<4; i++)
	{
		for(j=0; j<n_elems; j++)
		{
		    GLfloat acc = 0;
		    for (k=0; k<4; k++)
		        acc += matrix1[i+k*4] * vect[k+j*4];

		    result[i+j*4] = acc;
		}
	}
}

void raydium_math_quaternion_normalize(float *quat)
{
float magnitude;

magnitude = sqrt((quat[0] * quat[0]) + (quat[1] * quat[1]) + (quat[2] * quat[2]) + (quat[3] * quat[3]));
quat[0] /= magnitude;
quat[1] /= magnitude;
quat[2] /= magnitude;
quat[3] /= magnitude;
}

void raydium_math_quaternion_multiply(float *q1, float *q2, float *res)
{
float A, B, C, D, E, F, G, H;

A = (q1[3] + q1[0])*(q2[3] + q2[0]);
B = (q1[2] - q1[1])*(q2[1] - q2[2]);
C = (q1[3] - q1[0])*(q2[1] + q2[2]);
D = (q1[1] + q1[2])*(q2[3] - q2[0]);
E = (q1[0] + q1[2])*(q2[0] + q2[1]);
F = (q1[0] - q1[2])*(q2[0] - q2[1]);
G = (q1[3] + q1[1])*(q2[3] - q2[2]);
H = (q1[3] - q1[1])*(q2[3] + q2[2]);

res[3] = B + (-E - F + G + H)/2.0f;//w
res[0] = A - ( E + F + G + H)/2.0f;//x
res[1] = C + ( E - F + G - H)/2.0f;//y
res[2] = D + ( E - F - G + H)/2.0f;//z
}

#define SLERP_TO_LERP_SWITCH_THRESHOLD 0.01f
void raydium_math_quaternion_slerp(float *start, float *end, float alpha,float *result)
{
float startWeight, endWeight, difference;

difference = ((start[0] * end[0]) + (start[1] * end[1]) + (start[2] * end[2]) + (start[3] * end[3]));

if ((1.0f - fabs(difference)) > SLERP_TO_LERP_SWITCH_THRESHOLD)
    {
    float theta, oneOverSinTheta;

    theta = acos(fabs(difference));
    oneOverSinTheta = (1.0f / sin(theta));
    startWeight = (sin(theta * (1.0f - alpha)) * oneOverSinTheta);
    endWeight = (sin(theta * alpha) * oneOverSinTheta);

    if (difference < 0.0f)
        {
        startWeight = -startWeight;
        }
    }
else
    {
    startWeight = (1.0f - alpha);
    endWeight = alpha;
    }

result[0] = ((start[0] * startWeight) + (end[0] * endWeight));
result[1] = ((start[1] * startWeight) + (end[1] * endWeight));
result[2] = ((start[2] * startWeight) + (end[2] * endWeight));
result[3] = ((start[3] * startWeight) + (end[3] * endWeight));
raydium_math_quaternion_normalize(result);
}

void raydium_math_hms(double t, int *h, int *m, int *s, int *ms)
{
(*h)=t/3600;
t-=(*h)*3600;

(*m)=t/60;
t-=(*m)*60;

(*s)=t;
t-=(*s)*1;

(*ms)=t*1000;
}

void raydium_math_lookat_matrix4(GLfloat * eyePosition3D ,GLfloat *center3D, GLfloat *upVector3D,GLfloat *matrix){

	GLfloat forward[3], side[3], up[3];
	GLfloat matrix2[16], resultMatrix[16];

	forward[0]=center3D[0]-eyePosition3D[0];
	forward[1]=center3D[1]-eyePosition3D[1];
	forward[2]=center3D[2]-eyePosition3D[2];
	raydium_math_normalize_vector4(forward);

	//Side = forward x up
	raydium_math_crossproduct(forward, upVector3D,side );
	raydium_math_normalize_vector4(side);

	//Recompute up as: up = side x forward
	raydium_math_crossproduct(side, forward, up);

	matrix2[0]=side[0];
	matrix2[4]=side[1];
	matrix2[8]=side[2];
	matrix2[12]=0.0;

	matrix2[1]=up[0];
	matrix2[5]=up[1];
	matrix2[9]=up[2];
	matrix2[13]=0.0;

	matrix2[2]=-forward[0];
	matrix2[6]=-forward[1];
	matrix2[10]=-forward[2];
	matrix2[14]=0.0;

	matrix2[3]=matrix2[7]=matrix2[11]=0.0;
	matrix2[15]=1.0;

	raydium_math_multiply_matrix4(matrix, matrix2,resultMatrix);
	raydium_math_translate_matrix4_3f(-eyePosition3D[0], -eyePosition3D[1], -eyePosition3D[2],resultMatrix);

	memcpy(matrix, resultMatrix, 16*sizeof(GLfloat));
}

void raydium_math_frustum_matrix4 (GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat znear, GLfloat zfar,GLfloat *matrix){
	GLfloat matrix2[16], temp, temp2, temp3, temp4, resultMatrix[16];
	temp=2.0*znear;
	temp2=right-left;
	temp3=top-bottom;
	temp4=zfar-znear;
	matrix2[0]=temp/temp2;
	matrix2[1]=0.0;
	matrix2[2]=0.0;
	matrix2[3]=0.0;
	matrix2[4]=0.0;
	matrix2[5]=temp/temp3;
	matrix2[6]=0.0;
	matrix2[7]=0.0;
	matrix2[8]=(right+left)/temp2;
	matrix2[9]=(top+bottom)/temp3;
	matrix2[10]=(-zfar-znear)/temp4;
	matrix2[11]=-1.0;
	matrix2[12]=0.0;
	matrix2[13]=0.0;
	matrix2[14]=(-temp*zfar)/temp4;
	matrix2[15]=0.0;
	raydium_math_multiply_matrix4(matrix, matrix2,resultMatrix);
	memcpy(matrix, resultMatrix, 16*sizeof(GLfloat));
}

void raydium_math_perspective_matrix4(GLfloat fovyInDegrees, GLfloat aspectRatio, GLfloat znear, GLfloat zfar,GLfloat *matrix){
	GLfloat ymax, xmax;
	ymax=znear*tanf(fovyInDegrees*M_PI/360.0);
	xmax=ymax*aspectRatio;
	raydium_math_frustum_matrix4(-xmax, xmax, -ymax, ymax, znear, zfar, matrix);
}

void raydium_math_orthographic_matrix4(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat znear, GLfloat zfar,GLfloat *matrix){
	GLfloat matrix2[16], temp2, temp3, temp4, resultMatrix[16];
	temp2=right-left;
	temp3=top-bottom;
	temp4=zfar-znear;
	matrix2[0]=2.0/temp2;
	matrix2[1]=0.0;
	matrix2[2]=0.0;
	matrix2[3]=0.0;
	matrix2[4]=0.0;
	matrix2[5]=2.0/temp3;
	matrix2[6]=0.0;
	matrix2[7]=0.0;
	matrix2[8]=0.0;
	matrix2[9]=0.0;
	matrix2[10]=-2.0/temp4;
	matrix2[11]=0.0;
	matrix2[12]=(-right-left)/temp2;
	matrix2[13]=(-top-bottom)/temp3;
	matrix2[14]=(-zfar-znear)/temp4;
	matrix2[15]=1.0;
	raydium_math_multiply_matrix4(matrix, matrix2,resultMatrix);

	memcpy(matrix, resultMatrix, 16*sizeof(GLfloat));
}
