/* Function : int_correction */
/* Version: 0.1.0 */
/*   Correct flux for internal reddening.
 *
 * int int_correction(double lambda, double flux, double flux_err, double CEi,
 *	double CEi_err, double *flux_cor, double *flux_cor_err, int OPTIONS)
 *
 * Calling int_correction
 *  OPTIONS |= 1 << OPT_BIT
 *  status = int_correction(lambda, flux, CEi, &flux_cor, OPTIONS);
 *
 * Needs:
 *   %%%libKLutil%%%
 *
 */

#include "./redcor.h"
#include <KLutil.h>	/* error codes and equals() */
#include <stdio.h>	/* for stderr */
#include <math.h>	/* for pow() */

int int_correction(double lambda, double flux, double flux_err, double CEi,
 	double CEi_err, double *flux_cor, double *flux_cor_err, int OPTIONS)
{
 int status=0;
 double l,CE,CE_err;
 double kl,kl_err;
 
 /* printf("lambda=%g   flux=%g    flux_err=%g\n",lambda,flux,flux_err);
 printf("\tCEi=%f  CEi_err=%f\n",CEi,CEi_err);
 fflush(stdout);
 */
 l = lambda*1.e-4;
 CE = CEi;
 CE_err = CEi_err;
 if (OPTIONS & 1<<STARBURST) {
   if ( (l >= 0.63) && (l <= 2.2) ) {
      kl = 2.659*(-1.857 + 1.040/l) + RV_STARBURST;
      kl_err = 0.2*kl;
/*      kl_err = RV_STARBURST_ERR;*/
   } else if ( (l >= 0.12) && (l <=0.63) ) {
      kl = 2.659*(-2.156 + 1.509/l - 0.198/pow(l,2.) +
      		0.011/pow(l,3.)) + RV_STARBURST;
      kl_err = 0.2*kl;
/*      kl_err = RV_STARBURST_ERR;*/
   }
   else {
   	fprintf(stderr,"ERROR: Lambda out of range (%g um).\n",l);
   	return(ERRNO_INPUT_ERROR);
   }
   if ( !(OPTIONS & 1<<NEBULAR) ) { 
      CE = CE2CESTELLAR * CEi;
      CE_err = sqrt(pow(CEi*CE2CESTELLAR_ERR,2.)+pow(CE2CESTELLAR*CEi_err,2.));
   }
 }
 *flux_cor = flux*pow(10, 0.4*CE*kl);

 if (equals(*flux_cor,0.)) { 
    *flux_cor_err = flux_err;
 } else { 
    *flux_cor_err = *flux_cor * (flux_err/flux +
 			      0.4*log(10)*( (kl*CE_err) + (CE*kl_err) ));
 }

 return(status);
}
