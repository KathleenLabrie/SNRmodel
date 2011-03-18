/* Function : gcpos */
/* Version : 0.1.0 */
/*   Generate galactocentric positions.
 *
 * int gcpos(GCPOSPARS *pars, double *gcd, double *pa)
 *
 * Calling gcpos
 *   status = gcpos(gcpospars,&gcd,&gcl);
 *
 * Needs:
 *   %%%libKLran%%%
 */

#include "./artdata.h"
#include <KLran.h>
#include <math.h>
#include <stdio.h>

int gcpos(GCPOSPARS *pars, double *gcd, double *gcl)
{
 int status=0;
 static int iset=0;
 static float spare;
 float deviate;
 double x,y;

 if ( !strncmp(pars->gcpos_model, "gaussian", 8) ) {
   deviate = gasdev(&pars->gcpos_seedgcd);
   spare = gasdev(&pars->gcpos_seedgcd);
   x = pars->gcpos_scalerad * (double)deviate;
   y = sqrt((1.-pars->gcpos_ellip)*pow(pars->gcpos_scalerad,2))*(double)spare;
   *gcd = sqrt( x*x + y*y );
   *gcl = 180.*atan2( y, x )/PI;

   /* rotate */
   *gcl += pars->gcpos_pa;
 }
 else {
   fprintf(stderr,ERRMSG_INPUT_ERROR,pars->gcpos_model);
   fprintf(stderr,"User models not implemented yet.\n");
   return(ERRNO_INPUT_ERROR);
 }
 
 return(status);
}
