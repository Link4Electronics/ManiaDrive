#ifndef _MATH_H_RAY
#define _MATH_H_RAY
// (Linux) We had troubles with an already existing _MATH_H symbol.


#define raydium_trigo_cos(a) raydium_math_cos(a)
#define raydium_trigo_sin(a) raydium_math_sin(a)
#define raydium_trigo_cos_inv(a) raydium_math_cos_inv(a)
#define raydium_trigo_sin_inv(a) raydium_math_sin_inv(a)
#define raydium_trigo_abs(a) raydium_math_abs(a)
#define raydium_trigo_min(a,b) raydium_math_min(a,b)
#define raydium_trigo_max(a,b) raydium_math_max(a,b)
#define raydium_trigo_isfloat(a) raydium_math_isfloat(a)
#define raydium_trigo_round(a) raydium_math_round(a)
#define raydium_trigo_rotate(a,b,c,d,e) raydium_math_rotate(a,b,c,d,e)
#define raydium_trigo_pos_to_matrix(a,b) raydium_math_pos_to_matrix(a,b)
#define raydium_trigo_pos_get_modelview(a) raydium_math_pos_get_modelview(a)
#define raydium_trigo_pow2_next(a) raydium_math_pow2_next(a)

/*=
Maths
200
*/

// Little introduction to math.c
/**
This section is mostly designed for internal uses, but provides some
usefull maths functions, mostly for trigonometrical uses.

Historical note: most functions here were originally named with "trigo" prefix,
since this module was named trigo.c. Aliases are provided for compatibility
reasons, of course. (but watch out for old bindings !)
**/

__rayapi GLfloat raydium_math_cos (GLfloat i);
/**
Obvious (degrees)
**/

__rayapi GLfloat raydium_math_sin (GLfloat i);
/**
Obvious (degrees)
**/

__rayapi GLfloat raydium_math_cos_inv (GLfloat i);
/**
Obvious (degrees)
**/

__rayapi GLfloat raydium_math_sin_inv (GLfloat i);
/**
Obvious (degrees)
**/

#define raydium_math_rad2deg(a) ((a)*180/PI)
/**
Radians to degrees.
**/

#define raydium_math_deg2rad(a) ((a)*PI/180)
/**
Degrees to radians.
**/

#define raydium_math_abs(a) ( (a) < (0) ? (-(a)) : (a) )
/**
Obvious
define raydium_trigo_abs is deprecated, for compatibility only.
**/

#define raydium_math_min(a,b) ( (a) < (b) ? (a) : (b) )
/**
Obvious
define raydium_trigo_min is deprecated, for compatibility only.
**/

#define raydium_math_max(a,b) ( (a) > (b) ? (a) : (b) )
/**
Obvious
define raydium_trigo_max is deprecated, for compatibility only.
**/

#define raydium_math_isfloat(a) ( (!isnan(a) && !isinf(a)) ? 1 : 0)
/**
Test two cases : "Not a Number" and "Infinite"
define raydium_trigo_isfloat is deprecated, for compatibility only.
**/

#define raydium_math_round(a) ((int)((a)>0?((a)+0.5):((a)-0.5)))
/**
Will obviously "round" ##a## instead of the default C floor behaviour
define raydium_trigo_round is deprecated, for compatibility only.
**/

__rayapi void raydium_math_rotate (GLfloat * p, GLfloat rx, GLfloat ry, GLfloat rz, GLfloat * res);
/**
Rotate p (GLfloat * 3) by (rx,ry,rx) angles (degrees).
Result is stored in res (GLfloat * 3)
**/

__rayapi void raydium_math_normalize_vector4 (GLfloat * vector);
/**
Vector normalization.
**/

__rayapi void raydium_math_pos_to_matrix (GLfloat * pos, GLfloat * m);
/**
Generates a ODE style matrix (16 Glfloat) from pos (GLfloat * 3)
**/

__rayapi void raydium_math_matrix4_identity_set(GLfloat * mati);
/**
Set 4x4 matrix ##mati## to identity.
**/

__rayapi void raydium_math_multiply_matrix4(GLfloat *matrix1,GLfloat *matrix2,GLfloat *result );
/**
Matrix 4x4 multiplication.
**/

__rayapi void raydium_math_invert_matrix4(GLfloat * matin, GLfloat * matout);
/**
Compute inverse of 4x4 Matrix
**/

__rayapi void raydium_math_translate_matrix4_3f(GLfloat x, GLfloat y, GLfloat z,GLfloat *matrix);
/**
Translate 4x4 Matrix ##matrix## by (x,y,z) vector.
**/

__rayapi void raydium_math_rotate_matrix4_vector_angle(GLfloat angleInRadians, GLfloat x, GLfloat y, GLfloat z,GLfloat *matrix);
/**
Rotate 4x4 matrix ##matrix## arround vector ##(x,y,z)## for ##angleInRadians## radians.
**/

__rayapi void raydium_math_crossproduct (GLfloat *pvector1, GLfloat *pvector2,GLfloat *normal);
/**
Compute cross (vectorial) product of ##pvector1## and ##pvector2##. Result in ##normal## vector.
**/

__rayapi void raydium_math_multiply_matrix4_vectors_array(GLfloat *matrix1,GLfloat *vect, int n_elems,GLfloat *result);
/**
Multiply ##n_elems## vector array by 4x4 matrix ##matrix1##. Store rotated and translated vectors in ##n_elems## x 4 array ##result##.
**/

__rayapi void raydium_math_pos_get_modelview (GLfloat * res);
/**
Stores the translation part of the current OpenGL MODELVIEW matrix in res (3 GLfloat)
**/

__rayapi int raydium_math_pow2_next(int value);
/**
Returns next power of two of ##value##. Ugly.
**/

__rayapi int _raydium_math_MatrixInverse(const float *m,float *out);
/**
Our matrix_inverse seems broken.
This code works, thanks to Alexander Zaprjagaev (frustum@public.tsu.ru)
This code is not native
**/

__rayapi void raydium_math_quaternion_normalize(float *quat);
/**
Normalize ##quat## quaternion. I suppose such code is already available
in ODE.
**/

__rayapi void raydium_math_quaternion_slerp(float *start, float *end, float alpha,float *result);
/**
Spherical Linear Interpolation of quaternions, from ##start## to ##end##
with alpha [0,1] as interpolation point.
**/


void raydium_math_quaternion_multiply(float *q1, float *q2, float *result);
/**
Multiply two quaternions and fill the ##result## with the resulting
quaternion.
Quite usefull for making rotations over quaternions.
Here a list of common quaternions:
 W X Y Z
 1,0,0,0 	 				Identity quaternion, no rotation
 0,1,0,0 					180' turn around X axis
 0,0,1,0 					180' turn around Y axis
 0,0,0,1 					180' turn around Z axis
 sqrt(0.5),sqrt(0.5),0,0	90' rotation around X axis
 sqrt(0.5),0,sqrt(0.5),0	90' rotation around Y axis
 sqrt(0.5),0,0,sqrt(0.5)	90' rotation around Z axis
 sqrt(0.5),-sqrt(0.5),0,0	-90' rotation around X axis
 sqrt(0.5),0,-sqrt(0.5),0	-90' rotation around Y axis
 sqrt(0.5),0,0,-sqrt(0.5)	-90' rotation around Z axis
TODO: Those could be added like defines
**/


__rayapi float raydium_math_angle_from_projections(float px, float py);
/**
This function will return the real angle (in radians) for a pair of X
and Y projections of a vector. The vector has to be normalized, this is
, with the values betwen -1 and 1;
The returned angle will be in the range [0-2PI].
**/

#ifndef RAYDIUM_NETWORK_ONLY
__rayapi signed char raydium_math_point_unproject_3D(GLfloat x, GLfloat y, GLfloat z, float* resx, float* resy);
/**
Return the (x,y) screen coordinates for a 3D point viewed from the
current camera.
The ##resx## and ##resy## results are in the [0..100] interval.
This function will return ##0## if the point is behind the camera (and then,
probably not interesting), and ##1## if the point is in the POV of the
camera ("in front of" is more exact).

warning 1: the camera must be placed before calling this function.
warning 2: this function will replace the camera by itself.
**/
#endif

__rayapi void raydium_math_hms(double t, int *h, int *m, int *s, int *ms);
/**
Converts a duration (in seconds) to H:M:S:MS format.
**/

#endif

__rayapi void raydium_math_lookat_matrix4(GLfloat * eyePosition3D ,GLfloat *center3D, GLfloat *upVector3D,GLfloat *matrix);
/**
Build 4x4 Matrix corresponding to View look at matrix. This is a replacement for glut lookat function wich return matrix content to be used by a shader.
**/

__rayapi void raydium_math_frustum_matrix4 (GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat znear, GLfloat zfar,GLfloat *matrix);
/**
Build 4x4 Frustum Projection Matrix. Matrix returned is intended to be used in a shader.
**/

__rayapi void raydium_math_perspective_matrix4(GLfloat fovyInDegrees, GLfloat aspectRatio, GLfloat znear, GLfloat zfar,GLfloat *matrix);
/**
Build 4x4 Perspective Matrix for ##fovyInDegrees## in degree.
**/

__rayapi void raydium_math_orthographic_matrix4(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat znear, GLfloat zfar,GLfloat *matrix);
/**
Build 4x4 Orthographic Perspective Matrix.
**/
