/* Function : frg_correction */
/* Version: 0.1.0 */
/*   Correct flux for Galactic absorption.
 *
 * int frg_correction(double lambda, double flux, double flux_err, double CEf,
 *	double CEf_err, double *flux_cor, double *flux_cor_err, int OPTIONS)
 *
 * Calling frg_correction
 *  OPTIONS |= 1 << OPT_BIT
 *  status = frg_correction(lambda,flux,flux_err,CEf,CEf_err,
 *		&flux_cor, &flux_cor_err, OPTIONS);
 *
 * Needs:
 *
 */

#include "./redcor.h"
#include <KLutil.h>	/* error codes and equals() */
#include <stdio.h>	/* for stderr */
#include <math.h>	/* for pow() */

int frg_correction(double lambda, double flux, double flux_err, double CEf,
	double CEf_err, double *flux_cor, double *flux_cor_err, int OPTIONS)
{
 int status=0;
 double CE,CE_err;
 double a,b,x,y,Alambda,Alambda_err;

 /*printf("lambda=%g   flux=%g    flux_err=%g\n",lambda,flux,flux_err);
 printf("\tCEf=%f  CEf_err=%f\n",CEf,CEf_err);
 fflush(stdout);*/

 CE = CEf;
 CE_err = CEf_err;
 x = 1./(lambda*1.e-4);
 
 if ( (x >= 0.3) && (x <= 1.1) ) {
    a = 0.574*pow(x,1.61);
    b = -0.527*pow(x,1.61);
 } else if ( (x >= 1.1) && (x <= 3.3) ) {
    y = x-1.82;
    a = 1 + 0.17699*y - 0.50447*y*y - 0.02427*pow(y,3) + 0.72085*pow(y,4) +
    	0.01979*pow(y,5) - 0.77530*pow(y,6) +0.32999*pow(y,7);
    b = 1.41338*y + 2.28305*y*y + 1.07233*pow(y,3) - 5.38434*pow(y,4) - 
    	0.62251*pow(y,5) + 5.30260*pow(y,6) - 2.09002*pow(y,7);
 } else {
 	fprintf(stderr,"ERROR: Lambda out of range (%g).\n",lambda);
	return(ERRNO_INPUT_ERROR);
 }
 
 Alambda = RV_STD*CE*(a + b/RV_STD);
 Alambda_err = Alambda * sqrt(pow(CE_err/CE,2.) + pow(FRG_ERR,2.));
 
 *flux_cor = flux * pow(10,0.4*Alambda);
 if (equals(*flux_cor,0.)) {
    *flux_cor_err = flux_err;
 } else {
    *flux_cor_err = *flux_cor * (flux_err/flux + 0.4*log(10)*( Alambda_err ));
 }

 return(status);
}
