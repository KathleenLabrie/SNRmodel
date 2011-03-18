/* Function : artimg_xnoise */
/* Version : 0.1.0 */
/*   Add gaussian noise to an image.
 *
 * int artimg_xnoise(ARTIMGPARS pars, double **ppix);
 *
 * Calling artimg_xnoise
 *   status = artimg_xnoise(pars,ppix);
 *
 * Needs:
 *   %%%libKLran%%%
 */

#include "./artdata.h"
#include <KLran.h>
#include <string.h>
#include <time.h>	/* to get the seed */

int artimg_xnoise(ARTIMGPARS pars, double **ppix)
{
 int status=0;
 long int ii,jj,idum;
 double sigma;
 time_t now;

 idum = -(long int)time(&now);
 idum++;	/* to make sure I don't use the same seed as for last call */
 sigma = pars.artimg_xnoise;
 for (jj=0;jj<pars.artimg_naxes[1];jj++)
   for (ii=0;ii<pars.artimg_naxes[0];ii++)
     *(*(ppix+jj)+ii) += sigma*(double)gasdev(&idum);

 return(status);
}
