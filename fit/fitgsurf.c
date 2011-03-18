/* Main : fitgsurf */
/* Version: 1.0.1 */
/*   Fit a symmetric gaussian surface to a set of data points.  Outputs
 *   amplitude, x and y center, sigma and the chi square.  Error on each
 *   data points is ignored.
 *
 * Usage: fitgsurf [-v] [--fwhm=#] file
 *
 *	file		: Input file [Fmt: x y z]
 *
 * Needs
 *   %%%libKLfit%%%
 *   %%%libKLfunc%%%
 *   %%%libKLutil%%%
 */

#include "./fit.h"
#include <KLutil.h>
#include <KLfunc.h>
#include <KLfit.h>
#include <stdio.h>
#include <math.h>
#include <stddef.h>	/* for NULL */
#include <stdlib.h>

#define NDIM 3
#define MA 4
#define MAXITER 1e6
#define CHIDIFF 1e-6
#define CHIFRAC 1e-6
#define NCOLUMN 6
#define YCOLUMN 4
#define FWHM 20

main(argc,argv)
int argc;
char *argv[];
{
 int stable=0,verbose=0;
 int i, dim, niter;
 int ndata, ia[MA];
 double prevchisq, fwhm=FWHM;
 double **x, *y, *sig, a[MA], **covar, **alpha, chisq, alamda;
 char datafile[MAXLENGTH],line[MAXLENGTH],**p_parse;
 void (*pfunc)();
 FILE *istream;

 /* Set defaults */
 pfunc=fgaussS;

 /* Read command line */
 if ((argc < 2)&&(argc > 4)) { printf("%s",USAGE_FITGSURF); exit(99);}
 i=1;
 while (i < argc) {
   if (!strncmp("-v",argv[i],2)) {verbose = 1;}
   else if (!strncmp("--fwhm",argv[i],6)) {
     p_parse = svector(2,MAXLENGTH);
     splitstr(argv[i],p_parse,"=");
     fwhm=atof(p_parse[1]);
     free_svector(p_parse);
   }
   else if (!strncmp("-",argv[i],1)) {
     printf("ERROR: (%d) Unknown switch (%s)\n",INPUT_ERROR,argv[i]);
     printf("%s",USAGE_FITGSURF);
     exit(1);
   }
   else {
     strcpy( datafile, argv[i]);
   }
   i++;
 }

 
 /* count the number of data points */
 ndata=0;
 if ( (istream = fopen(datafile,"r")) == NULL ) {
 	printf("ERROR: Unable to open file %s for reading.\n",datafile);
	exit(FILE_NOT_FOUND);
 }
 while (fgets( line, MAXLENGTH-1, istream) != NULL) 
 	ndata++;
 fclose(istream);

 /* allocate memory for the various arrays */
 x = dmatrix(NDIM,ndata);
 y = dvector(ndata);
 sig = dvector(ndata);
 covar = dmatrix(MA,MA);
 alpha = dmatrix(MA,MA);

 /* Read the data file */
 if ( (istream = fopen(datafile,"r")) == NULL ) {
 	free_dmatrix(x);
	free_dvector(y);
	free_dvector(sig);
	free_dmatrix(covar);
	free_dmatrix(alpha);
 	printf("ERROR: Unable to open file %s for reading.\n",datafile);
	exit(FILE_NOT_FOUND);
 }
 p_parse=svector(NCOLUMN,MAXLENGTH);
 i=0;
 while (fgets( line, MAXLENGTH-1, istream) != NULL) {
   splitstr(line, p_parse, SPACES);
   for (dim=0;dim<NDIM-1;dim++) x[dim][i] = atof(p_parse[dim]);
   y[i]=atof(p_parse[YCOLUMN-1]);
   sig[i]=1.0;		/* ignore errors */
   i++;
 }
 fclose(istream);

 /* Set variables related to fitting */
 for (i=0;i<MA;i++) ia[i]=1;	/* fit all coefficient */
 /* initialize coefficient */
 /*a[0]=y[0];	*/
 /*a[0]=fmax(y,ndata);*/
 a[0]=(DMAX_VECTOR(y,ndata)+y[0])/2.;		
 a[1]=x[0][0];
 a[2]=x[1][0];
 a[3]=fwhm/2.;
 alamda=-1;		/* will trigger initialization in mrqmin */
 if (verbose) {
   printf("\n");
   printf("Initial values: Amplitude = %g\n",a[0]);
   printf("                X center  = %f\n",a[1]);
   printf("                Y center  = %f\n",a[2]);
   printf("                Sigma     = %f\n",a[3]);
   printf("\n");
 }
 
 /* Fit */
 prevchisq=1E30;
 chisq = 0;
 niter=0;
 if (verbose) printf("Chisq=                  ");
 while ((niter<MAXITER) && (stable<=10)) {
   prevchisq = chisq;
   niter++;
   dmrqminD(x,y,sig,NDIM,ndata,a,ia,MA,covar,alpha,&chisq,pfunc,&alamda);
   if (verbose) printf("\b\b\b\b\b\b\b\b\b\b\b\b%10e",chisq);
/*   if (fabs(prevchisq-chisq) < CHIDIFF)  
   	stable++;*/
   if ( fabs(1. - (prevchisq/chisq)) < CHIFRAC )
   	stable++;
   else
   	stable=0;	/*reset*/
 }
 if (verbose) printf("\n\n");

 /* What's the result */
 if (niter == MAXITER) {
 	free_dmatrix(x);
	free_dvector(y);
	free_dvector(sig);
	free_dmatrix(covar);
	free_dmatrix(alpha);
   	printf("ERROR: Reached maximum number of iterations.\n");
  	exit(MAX_ITER);
 }
 else {
   /* final call to mrqmin with alamda=0 */
   alamda=0;
   dmrqminD(x,y,sig,NDIM,ndata,a,ia,MA,covar,alpha,&chisq,pfunc,&alamda);

   /* Print results */
   printf("#%s:\n",datafile);
   printf("#Coefficients\tUncertainty\n");
   for (i=0;i<MA;i++) printf("%f\t%e\n",a[i],sqrt(covar[i][i]));
 }

 free_dmatrix(x);
 free_dvector(y);
 free_dvector(sig);
 free_dmatrix(covar);
 free_dmatrix(alpha);
 
 exit(0);
}
