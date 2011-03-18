/* Function Package : artdata.c */
/* Version: 0.1.0 */
/*   Memory allocation for the artdata package.
 *
 * Needs:
 *   %%%libKLutil%%%
 */

#include "./artdata.h"
#include <myutil.h>	/* for rterror() */
#include <stdlib.h>

ASNR *asnrvector(long int nh)
/* allocate  a ASNR vector with subscript range v[0..nh-1] */
{
 ASNR *v;
 v = (ASNR *)malloc( (size_t) (nh*sizeof(ASNR)) );
 if (!v) rterror("allocation failure in asnrvector()");
 return (v);
}

void free_asnrvector(ASNR *v)
/* free a ASNR vector allocated with asnrvector() */
{
 free(v);
}
