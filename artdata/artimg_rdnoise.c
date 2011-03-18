/* Function : artimg_rdnoise */
/* Version : 0.1.0 */
/*   Add read noise to an image.
 *
 * int artimg_rdnoise(ARTIMGPARS pars, double **ppix);
 *
 * Calling artimg_rdnoise
 *   status = artimg_rdnoise(pars,ppix);
 *
 * Needs:
 *   %%%libKLran%%%
 */

#include "./artdata.h"
#include <KLran.h>
#include <string.h>
#include <time.h>	/* to get the seed */

int artimg_rdnoise(ARTIMGPARS pars, double **ppix)
{
 int status=0;
 long int ii,jj,idum;
 double sigma;
 time_t now;

 idum = -(long int)time(&now);
 idum++;	/* to make sure I don't use the same seed as for pnoise */
 sigma = pars.artimg_rdnoise/pars.artimg_gain;
 for (jj=0;jj<pars.artimg_naxes[1];jj++)
   for (ii=0;ii<pars.artimg_naxes[0];ii++)
     *(*(ppix+jj)+ii) += sigma*(double)gasdev(&idum);

 return(status);
}
