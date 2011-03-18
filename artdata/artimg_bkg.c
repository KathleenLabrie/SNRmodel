/* Function : artimg_bkg */
/* Version : 0.1.0 */
/*   Add sky background flux to image.
 *
 * int artimg_bkg(ARTIMGPARS pars, double **ppix)
 *
 * Calling artimg_bkg
 *   status=artimg_bkg(pars,ppix);
 *
 * Needs:
 */

#include "./artdata.h"
#include <stdio.h>

int artimg_bkg(ARTIMGPARS pars, double **ppix)
{
 int status=0;
 long int ii,jj;
 double bkg;
 
 bkg = pars.artimg_skybkg;
 for (jj=0;jj<pars.artimg_naxes[1];jj++) {
   for (ii=0;ii<pars.artimg_naxes[0];ii++) {
     *(*(ppix+jj)+ii) += bkg;
   }
 }

 return(status);
}
