/* Function : snrlum */
/* Version : 0.1.0 */
/*   Generate SNR luminosity.
 *
 * int snrlum(SNRPARS *pars, ASNR *asnr)
 *
 * Calling snrlum
 *   ASNR *asnr;
 *   status = snrlum(&pars, asnr);
 *
 * Needs:
 */

#include "./artdata.h"
#include "./mksnrpop.h"
#include <KLran.h>
#include <math.h>

int snrlum(SNRPARS *pars, ASNR *asnr)
{
 int status=0;
 static int iset=0;
 static float spare;
 float deviate;
 double lum,ne;
 double factor,stddev_factor,stddev_ne;
 double E51,zeta,n0,beta,Dpds;
 double life, tpds, tsf, Dlife;

 /* Assign physical parameters */
 E51 = DEFAULT_E51;
 zeta = DEFAULT_METAL;
 beta = DEFAULT_COMPRESSION_FACTOR;
 n0 = pars->snr_n0;
 life = pars->snr_life;

 if ( !strncmp(pars->snr_modlum, "morel", 5) ) {
    Dpds = 28. * pow(E51,2./7.) * pow(zeta,-1./7.) * pow(n0,-3./7.);
    tsf = 3.61e4* pow(E51,3./14.) * pow(zeta,-5./14.) * pow(n0,-4./7);
    tpds = tsf/exp(1.);
    Dlife = Dpds* pow((4./3.)*(life/tpds) - (1./3.) , 3./10.);
    if ( (asnr->asnr_diameter >= Dpds) && (asnr->asnr_diameter <= Dlife)) {
       if (iset == 0) {
          deviate = gasdev(&pars->snr_seedlum);
          spare = gasdev(&pars->snr_seedlum);
          iset=1;
       } else {
          deviate = spare;
          iset=0;
       }
       factor = 1.1;
       stddev_factor = 1.;
       stddev_ne = beta * n0 * stddev_factor/factor;
       ne = stddev_ne*(double)deviate + beta*n0;
       if ( ne < 0 ) { ne = 0.; }
       lum = factor*ne*LSOLAR;
       asnr->asnr_sb = lum / (PI/4. * asnr->asnr_diameter*asnr->asnr_diameter);
    } else {
       asnr->asnr_sb = 0.;
    }
 }
 else {
    fprintf(stderr,ERRMSG_INPUT_ERROR,pars->snr_modlum);
    fprintf(stderr,"User models not implemented yet.\n");
    return(ERRNO_INPUT_ERROR);
 }

 return(status);
}
