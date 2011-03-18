/* Function : artimg_pnoise */
/* Version : 0.1.0 */
/*   Add Poisson noise to an image.
 *
 * int artimg_pnoise(ARTIMGPARS pars, double **ppix);
 *
 * Calling artimg_pnoise
 *   status = artimg_pnoise(pars,ppix);
 *
 * Needs
 *   %%%libKLutil%%%
 *   %%%libKLran%%%
 */

#include "./artdata.h"
#include <KLutil.h>
#include <KLran.h>
#include <string.h>
#include <KLcfitsio.h>
#include <time.h>	/* to get the seed */

int artimg_pnoise(ARTIMGPARS pars, double **ppix, double **pnoiseimg)
{
 int status=0;
 long int ii,jj,idum;
 double efluxtot;
 time_t now;

 idum = -(long int)time(&now);
 for (jj=0;jj<pars.artimg_naxes[1];jj++)
   for (ii=0;ii<pars.artimg_naxes[0];ii++) {
     efluxtot = ( *(*(ppix+jj)+ii) + *(*(pnoiseimg+jj)+ii) ) * 
     		 pars.artimg_gain;
     *(*(ppix+jj)+ii) = 
     	(double)poidev((float)efluxtot, &idum) / pars.artimg_gain;
     *(*(ppix+jj)+ii) -= *(*(pnoiseimg+jj)+ii);
   }

 return(status);
}
