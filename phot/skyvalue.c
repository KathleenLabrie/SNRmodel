/* Function: skyvalue */
/* Version: 0.1.0 */
/*   Calculate skyvalue with iterative sigma clipping on a vector of pixels.
 *   Currently supported : - mean
 *			   - median
 *
 * Usage: serr=skyvalue(skypix,npix,fitskypars,&skyvalue,&stddev,&new_npix)
 * 
 * Needs:
 *   %%%libKLstats%%%
 */

#include "./phot.h"
#include "./KLstats.h"
#include <stdio.h>
#include <KLutil.h>
#include <math.h>	/* for gcc4.0, fabs */

int skyvalue(double *inputvec, int npix, FITSKYPARS pars, 
	      double *sky, double *stddev, int *npix1, int FLAGS)
{
 int serr=0,i,niter,nreject,l_dtmp=0;
 int npix2;
 double *skyvec1, *skyvec2;
 double mean,variance,median,value;

 skyvec1 = dvector(npix);
 skyvec2 = dvector(npix);
 for (i=0;i<npix;i++) { *(skyvec1+i) = *(inputvec+i); }
 *npix1=npix;

 /* Calculate sky value before rejection */
 dstddev(skyvec1,*npix1,&mean,stddev,&variance);
 if ( !strncmp(pars.fitsky_algorithm,"mean",4) ) {
     	*sky = value = mean;
 } else if ( !strncmp(pars.fitsky_algorithm,"median",6) ) {
     	*sky = value = dquickfmedian(skyvec1,*npix1);
 }

 nreject=0;
 niter=1;
 while ( niter <= pars.fitsky_maxiter ) {
 
   /* Do rejection */
   npix2=0;
   for (i=0;i<*npix1;i++) {
     if ( (*(skyvec1+i) >= value - pars.fitsky_loreject*(*stddev)) &&
          (*(skyvec1+i) <= value + pars.fitsky_hireject*(*stddev)) ) {
	*(skyvec2+npix2) = *(skyvec1+i);
	npix2++;
     } else { nreject++; }
   }
   
   /* Calculate new sky value if conditions are still met*/
   if ( (nreject <= pars.fitsky_maxreject) &&
	(npix2 > 0) 			   &&
	(npix2 >= pars.fitsky_nkeep)       &&
	( (*npix1 - npix2) > 0 )           ) {
   printf("niter=%d   nreject=%d\n",niter,nreject);
      *npix1 = npix2;
      dstddev(skyvec2,npix2,&mean,stddev,&variance);
      if ( !strncmp(pars.fitsky_algorithm,"mean",4) ) {
     	  value = mean;
      } else if ( !strncmp(pars.fitsky_algorithm,"median",6) ) {
     	  value = dquickfmedian(skyvec2,npix2);
      }
      printf("value=%f   previous_sky=%f  diff=%f\n",value,*sky,
      		fabs(value-*sky));
      if ( fabs(value - *sky) < FITSKY_TOLERANCE ) {
      	  *sky = value;
	  free_dvector(skyvec1);
	  free_dvector(skyvec2);
	  return(serr);
      } else { *sky = value; }
   } else {
      return(serr);   /* return with current value, ignore last rejection */
   }

   /* condition are still met, precision not reached yet 
    *  ==> prepare for another rejection 
    */

   /* Copy new vector to work vector */
   for (i=0;i<npix2;i++) { *(skyvec1+i) = *(skyvec2+i); }

   niter++;
 }
 /* figure out how it got out and set serr */

 free_dvector(skyvec1);
 free_dvector(skyvec2);
 return(serr);
}
