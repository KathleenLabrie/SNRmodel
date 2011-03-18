/* Function : artimg_integrate */
/* Version : 0.1.0 */
/*   Integrate for n seconds.
 *
 * int artimg_integrate(ARTIMGPARS pars, double **ppix);
 *
 * Calling artimg_integrate
 *   status = artimg_integrate(pars, ppix);
 */

#include "./artdata.h"

int artimg_integrate(ARTIMGPARS pars, double **ppix)
{
 int status=0;
 long int ii,jj;
 
 for (jj=0;jj<pars.artimg_naxes[1];jj++) 
   for (ii=0;ii<pars.artimg_naxes[0];ii++) 
     *(*(ppix+jj)+ii) *= pars.artimg_exptime;

 return(status);
}
