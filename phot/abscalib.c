/* Main : abscalib */
/* Version: 0.1.0 */
/*   Calculate the absolute calibration of photometry.
 *
 *   Use blackbody at T=11200 K normalized at 5550Ang to the flux of Vega,
 *   as obtained in the reference below.
 *
 *   Outputs F_0 [W/m^2] for a given filter.
 *
 *   Ref: Bersanelli, Bouchet & Falomo, 1991, A&A, 252, 854.
 *        "JHKL' photometry on the ESO system: systematic effects
 *	   and absolute calibration"
 *
 * Usage: abscalib [-vh] [-r interval] filter 
 *
 *	filter		: Filter transmission file
 *	-h		: Print help
 *	-r interval	: Overwrite default wavelength resolution
 *	-v		: Toggle on verbose
 *
 * Needs:
 *   %%%libKLutil%%%
 *   %%%libKLfile%%%
 *   %%%libKLinter%%%
 *
 */

#include <KLutil.h>
#include <KLfile.h>
#include <KLinter.h>
#include <stddef.h>	/* for NULL */
#include <string.h>	/* for strncmp, strcpy */
#include <stdlib.h>	/* for atof() */
#include <math.h>	/* for pow(), exp() */

#define HELP_ABSCALIB \
"\nUsage: abscalib [-vh] filter\n\
  filter	: Filter transmission file\n\
  -h		: Print this help\n\
  -r interval	: Overwrite default wavelength resolution\n\
  -v		: Toggle on verbose\n"

#define H_PLANCK 6.626e-34	/* J s */
#define C_LIGHT 2.998e8		/* m/s */
#define K_BOLTZMANN 1.38e-23	/* J/K */

#define NORM_LAMBDA 5550e-10	/* m */
#define NORM_FLUX (1e10*3.44e-12)	/* W/m2/m (converted from W/cm2/um) */
#define BLACKBODY_T 11200	/* K */
#define BLACKBODY(t,l) ( (2*H_PLANCK*C_LIGHT*C_LIGHT/pow((l),5)) * \
		(1./(exp((H_PLANCK*C_LIGHT)/((l)*K_BOLTZMANN*(t))) - 1)) )

#define DEFAULT_INTERVAL 0.1
#define INTER_ORDER 3
#define UNITS 1e-10	/* Angstrom to meters (for input lambda in Angstrom) */

main( argc, argv )
int argc;
char *argv[];
{
 int i,status=0;
 long int nlambda,k;
 unsigned long pos;
 int FLAGS;
 double *plambda,*pS;
 double norm_factor,lambda_start,lambda_end,lambda,dlambda;
 double f1,f2,S2,dS,F_0;
 double units=UNITS,percent=0.01;
 char tfile[MAXLENGTH];

 /* Initialize */
 FLAGS=0;
 /*FLAGS |= 1 << DEBUG; */
 dlambda=DEFAULT_INTERVAL;

 /* Read command line */
 i=1;
 while (i < argc) {
   if (!strncmp("-h",argv[i],2)) { printf("%s",HELP_ABSCALIB); exit(0); }
   else if (!strncmp("-r",argv[i],2)) { dlambda = atof(argv[++i]); }
   else if (!strncmp("-u",argv[i],2)) { units = atof(argv[++i]); }
   else if (!strncmp("-v",argv[i],2)) { FLAGS |= 1 << VERBOSE; }
   else {
     strcpy(tfile,argv[i]);
   }
   i++;
 }

 /* Read filter transmission file */
 if ( status = rd_set_double(tfile, &plambda, &pS, &nlambda) ) {
   if (plambda != NULL) {free_dvector(plambda);}
   if (pS != NULL) {free_dvector(pS);}
   fprintf(stderr,ERRMSG_IO_ERROR,tfile);
   exit(status);
 }


 norm_factor = NORM_FLUX/BLACKBODY(BLACKBODY_T,NORM_LAMBDA);
 lambda_start = *(plambda+0);
 lambda_end = *(plambda+nlambda-1);

 pos=0;
 dhunt(plambda,nlambda,lambda_start,&pos);
 k=IMIN(IMAX(pos-(INTER_ORDER)/2,0),nlambda-(INTER_ORDER+1));
 dpolint(&plambda[k],&pS[k],INTER_ORDER+1,lambda_start,&S2,&dS);

 f2 = norm_factor*BLACKBODY(BLACKBODY_T,lambda_start*units)*S2*percent;
 F_0 = 0.;
 for (lambda=lambda_start; lambda<(lambda_end-dlambda/2); lambda += dlambda) {
   f1 = f2;
   dhunt(plambda,nlambda,lambda+dlambda,&pos);
   k=IMIN(IMAX(pos-(INTER_ORDER)/2,0),nlambda-(INTER_ORDER+1));
   dpolint(&plambda[k],&pS[k],INTER_ORDER+1,lambda+dlambda,&S2,&dS);
   f2 = norm_factor*BLACKBODY(BLACKBODY_T,(lambda+dlambda)*units)*S2*percent;
   F_0 += 0.5*(f2+f1)*dlambda*units;
 }

 printf("For filter defined in %s: F_0 = %g [W/m2]\n",tfile,F_0);

 free_dvector(plambda);
 free_dvector(pS);
 exit(0);
}
