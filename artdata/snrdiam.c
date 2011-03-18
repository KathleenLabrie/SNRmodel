/* Function : snrdiam */
/* Version : 0.1.0 */
/*   Generate SNR diameters.  Still need to implement position dependency.
 *
 * int snrdiam(SNRPARS *pars, ASNR *asnr)
 *
 * Calling snrdiam
 *   ASNR *asnr;
 *   status = snrdiam(&pars, asnr);
 *
 * Needs:
 */

#include "./artdata.h"
#include "./mksnrpop.h"
#include <KLran.h>
#include <math.h>

int snrdiam(SNRPARS *pars, ASNR *asnr)
{
 int status=0;
 double dum;
 double E51,zeta,xi,n0,tau;
 double N,tpds,Dpds;

 /* Assign physical parameters */
 E51 = DEFAULT_E51;
 zeta = DEFAULT_METAL;
 xi = DEFAULT_SEDOV_NUM_PARAM;
 n0 = pars->snr_n0;
 tau = 1./pars->snr_rate;

 if ( !strncmp(pars->snr_moddiam, "adia", 4) ) {
    /* N(<D) = (4.538/tau) * (no/(xi*E51))^0.5 * D^2.5 */
    dum = ran2(&pars->snr_seeddiam);
    N = pars->snr_nsnr * dum;
    asnr->asnr_diameter = pow( N*tau/4.538, 0.4 ) * pow( xi*E51/n0, 0.2 );    
 }
 else if ( !strncmp(pars->snr_moddiam, "rad", 3) ) {
    /* N(<D) = 3/4[ (D/Dpds)^10/3 + 1/3](tpds/tau) */
    tpds = 1.33e4 * pow(E51,3./14.) * pow(zeta,-5./14.) * pow(n0,-4./7.);
    Dpds = 28. * pow(E51,2./7.) * pow(zeta,-1./7.) * pow(n0,-3./7.);
    dum = ran2(&pars->snr_seeddiam);
    N = pars->snr_nsnr * dum;
    if ( N < (1./4.)*(tpds/tau)) { asnr->asnr_diameter = 0.; }
    else { 
    	asnr->asnr_diameter = Dpds * pow((4./3.)*(tau/tpds)*N - 1./3., 3./10.);
    }
 }
 else {
    fprintf(stderr,ERRMSG_INPUT_ERROR,pars->snr_moddiam);
    fprintf(stderr,"User models not implemented yet.\n");
    return(ERRNO_INPUT_ERROR);
 }

 return(status);
}
