/* vi: set sw=4 ts=4: */

#include "math.h"


/* cosf for uClibc
 *
 * wrapper for cos(x)
 */

#ifdef __STDC__
	float cosf(float x)
#else
	float cosf(x)
	float x;
#endif
{
	return (float) cos( (double)x );
}


/* sinf for uClibc
 *
 * wrapper for sin(x)
 */

#ifdef __STDC__
	float sinf(float x)
#else
	float sinf(x)
	float x;
#endif
{
	return (float) sin( (double)x );
}


/* ceilf for uClibc
 *
 * wrapper for ceil(x)
 */

#ifdef __STDC__
	float ceilf(float x)
#else
	float ceilf(x)
	float x;
#endif
{
	return (float) ceil( (double)x );
}


/* rintf for uClibc
 *
 * wrapper for rint(x)
 */

#ifdef __STDC__
	float rintf(float x)
#else
	float rintf(x)
	float x;
#endif
{
	return (float) rint( (double)x );
}

