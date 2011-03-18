/* Main : redcor */
/* Version: 0.1.0 */
/*   Apply reddening correction to line flux and broad-band photometry.
 *   Uses the standard Milky Way extinction law of Cardelli, Clayton &
 *   Mathis (1989) for foreground Galactic extinction.
 *   Uses starburst reddening curve of Calzetti etal. 2000.
 *
 * Usage: redcor [-vh] 'line' lambda flux CEf CEi [--starburst]
 *        redcor [-vh] 'cont' filterID mag CEf CEi [--starburst]
 *        redcor --filters
 *
 *   'line'/'cont'	: mode to adopt
 *   lambda		: wavelength of the line (Angstrom)
 *   flux		: observed flux
 *   filterID		: code name of the filter transmission to use
 *   mag		: observed magnitude
 *   CEf		: E(B-V) foreground
 *   CEi		: E(B-V) intrinsic
 *   --starburst	: toggle on the use of the starburst reddening curve
 *			  for intrinsic extinction
 *   --filters		: print the filter description
 *   -h			: print help
 *   -v			: toggle verbose on
 *
 * Needs:
 *   %%%libKLutil%%%
 *   %%%libKLfile%%%
 */

#include "./redcor.h"
#include <KLutil.h>
#include <KLfile.h>
#include <stddef.h>	/* for NULL */
#include <string.h>	/* for strncmp(), strcpy() */
#include <stdlib.h>	/* for atof() */
#include <math.h>	/* for log10(), exp() */

#define LINE 0
#define CONTINUUM 1

main(argc,argv)
int argc;
char *argv[];
{
 int i,n,status=0;
 int FLAGS,OPTIONS;
 unsigned short int filter,mode;
 long l,nlambda;
 double *lambda=NULL, *fluxobs=NULL;
 double fluxobs_err;
 double CEf,CEi,CEf_err,CEi_err;
 double fluxfc,fluxic,sum,norm;
 double fluxfc_err,fluxic_err,sum_err;
 double magobs,deltamag,magint;
 double magobs_err,deltamag_err,magint_err;
 double prev_fluxic,prev_fluxic_err,dlambda;
 char **p_arguments=NULL;

 /* Initialize */
 FLAGS = 0;
 /*  FLAGS |= 1 << DEBUG; */ 
 OPTIONS = 0;

 /* Read command line */
 i=1;
 n=0;
 p_arguments = svector(MAXCOLS,MAXLENGTH);
 while (i < argc) {
   if (!strncmp("--",argv[i],2)) {
     if (!strncmp("--filters",argv[i],9)) { 
     	free_svector(p_arguments);
     	printf("%s",HELP_FILTERS);
	exit(0);
     }
     else if (!strncmp("--starburst",argv[i],11)) { 
     	OPTIONS |= 1 << STARBURST;
     }
     else {
     	free_svector(p_arguments);
     	fprintf(stderr,ERRMSG_INPUT_ERROR,argv[i]);
	fprintf(stderr,HELP_REDCOR);
	exit(ERRNO_INPUT_ERROR);
     }
   }
   else if (!strncmp("-",argv[i],1)) {
     if (!strncmp("-h",argv[i],2)) {
     	free_svector(p_arguments);
     	printf("%s",HELP_REDCOR);
	exit(0);
     }
     else if (!strncmp("-v",argv[i],2)) { FLAGS |= 1 << VERBOSE; }
   }
   else {
     strcpy(p_arguments[n++],argv[i]);
   }
   i++;
 }

 if (!strncmp("line",p_arguments[0],4)) {
   if (n != 8) {
   	free_svector(p_arguments);
	fprintf(stderr,ERRMSG_INPUT_ERROR,p_arguments[0]);
	fprintf(stderr,HELP_REDCOR);
	exit(ERRNO_INPUT_ERROR);
   }
   mode = LINE;
   lambda = dvector(1);
   fluxobs = dvector(1);
   *(lambda+0) = atof(p_arguments[1]);
   *(fluxobs+0) = atof(p_arguments[2]);
   fluxobs_err = atof(p_arguments[3]);
   CEf = atof(p_arguments[4]);
   CEf_err = atof(p_arguments[5]);
   CEi = atof(p_arguments[6]);
   CEi_err = atof(p_arguments[7]);
 }
 else if (!strncmp("cont",p_arguments[0],4)) {
   if (n != 8) {
   	free_svector(p_arguments);
	fprintf(stderr,ERRMSG_INPUT_ERROR,p_arguments[0]);
	fprintf(stderr,HELP_REDCOR);
	exit(ERRNO_INPUT_ERROR);
   }
   mode = CONTINUUM;
   if (status = filterID(p_arguments[1],&filter)) {
   	free_svector(p_arguments);
	fprintf(stderr,HELP_FILTERS);
	exit(status);
   }
   magobs = atof(p_arguments[2]);
   magobs_err = atof(p_arguments[3]);
   CEf = atof(p_arguments[4]);
   CEf_err = atof(p_arguments[5]);
   CEi = atof(p_arguments[6]);
   CEi_err = atof(p_arguments[7]);
 }
 else {
 	free_svector(p_arguments);
	fprintf(stderr,ERRMSG_INPUT_ERROR,p_arguments[0]);
	fprintf(stderr,HELP_REDCOR);
	exit(ERRNO_INPUT_ERROR);
 }
 free_svector(p_arguments);

 switch (mode)
 {
   case LINE:
   	OPTIONS |= 1<<NEBULAR;
   	status = frg_correction(*(lambda+0),*(fluxobs+0),fluxobs_err,
			CEf,CEf_err,&fluxfc,&fluxfc_err,OPTIONS);
	status = int_correction(*(lambda+0),fluxfc,fluxfc_err,CEi,CEi_err,
			&fluxic,&fluxic_err,OPTIONS);
	if (status) {
		free_dvector(lambda);
		free_dvector(fluxobs);
		exit(status);
	}
	printf("Lambda=%g A  Intrisic flux=(%g +/- %g) W/m^2\n",
			*(lambda+0),fluxic,fluxic_err);
   	break;
   case CONTINUUM:
   	/* fluxobs is trans(lambda), the filter's transmission curve */
    	/* fluxfc is trans(lambda) corrected for foreground */
	/* fluxic is trans(lambda) corrected for intrinsic absorption */
	/*  --- no need to define new variables --- */
   	if (status = get_transmission(filter,&lambda,&fluxobs,&nlambda)) {
		if (lambda != NULL) { free_dvector(lambda); }
		if (fluxobs != NULL) { free_dvector(fluxobs); }
		exit(status);
	}
	sum=sum_err=0.;
	norm=0.;
	status = frg_correction(*(lambda+0),*(fluxobs+0),0,CEf,CEf_err,
			&fluxfc,&fluxic_err,OPTIONS);
	status = int_correction(*(lambda+0),fluxfc,fluxfc_err,CEi,CEi_err,
	   	     	&fluxic,&fluxic_err,OPTIONS);
	if (status) {
	     free_dvector(lambda);
	     free_dvector(fluxobs);
	     fprintf(stderr,"ERROR at lambda=%f\n",*(lambda+l));
	     exit(status);
	}
	for (l=1;l<nlambda;l++) {
	   prev_fluxic = fluxic;
	   prev_fluxic_err = fluxic_err;
	   status = frg_correction(*(lambda+l),*(fluxobs+l),0,CEf,CEf_err,
	   		&fluxfc,&fluxic_err,OPTIONS);
	   status = int_correction(*(lambda+l),fluxfc,fluxfc_err,CEi,CEi_err,
	   		&fluxic,&fluxic_err,OPTIONS);
	   if (status) {
	   	free_dvector(lambda);
		free_dvector(fluxobs);
		fprintf(stderr,"ERROR at lambda=%f\n",*(lambda+l));
		exit(status);
	   }
	   dlambda = fabs(*(lambda+l)-*(lambda+l-1));
	   sum += dlambda*(fluxic+prev_fluxic)/2.;
	   sum_err += dlambda*(fluxic_err+prev_fluxic_err)/2.;
	   norm += dlambda*( *(fluxobs+l)+*(fluxobs+l-1))/2.;
	   if (FLAGS & 1 <<DEBUG) {
	     printf("fluxic=%g +/- %g   prev_fluxic=%g +/- %g\n",
	   		fluxic,fluxic_err,prev_fluxic,prev_fluxic_err);
	     printf("dlambda=%g   ",dlambda);
	     printf("sum=%g  sum_err=%g  norm=%g\n",sum,sum_err,norm);
	   }
	}
	deltamag = -2.5*log10(sum/norm);
	deltamag_err = fabs(-2.5 * log10(exp(1.)) * sum_err / sum);
	magint = magobs + deltamag;
	magint_err = magobs_err + deltamag_err;
	printf("Corrected magnitude = %f +/- %f\n",magint,magint_err);
   	break;
   default:
   	fprintf(stderr,ERRMSG_INTERNAL_ERROR);
	exit(ERRNO_INTERNAL_ERROR);
 }

 exit(0);
}
