/* vi: set sw=4 ts=4: */

/* cosf for uClibc
 *
 * wrapper for cos(x)
 */

#include "math.h"

#ifdef __STDC__
	float cosf(float x)		/* wrapper cos */
#else
	float cosf(x)			/* wrapper cos */
	float x;
#endif
{
	return (float) cos( (double)x );
}

/* sinf for uClibc
 *
 * wrapper for sin(x)
 */

#include "math.h"

#ifdef __STDC__
	float sinf(float x)		/* wrapper sin */
#else
	float sinf(x)			/* wrapper sin */
	float x;
#endif
{
	return (float) sin( (double)x );
}
