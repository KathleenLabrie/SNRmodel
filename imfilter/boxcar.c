/* Function: boxcar
 * Version: 0.1.0
 *   Smooth an image using a boxcar average.  The size of the box varies
 *   across the image, being small near the edges and growing to maximum
 *   size towards the center.  Clipping and masking can be done.
 *
 * int boxcar( double **ppix, double **pmask, long naxes[], 
 *                 VBOX *vbpar, CLIP *cpar, > double ***pnew )
 *
 * Arguments:
 *	Input: 2-D image (pix[jj][ii] = value)		- double **ppix
 *	       2-D mask (mask[jj][ii] = 1|0)		- double **pmask
 *	       length of each axis (ii,jj)			- unsigned long naxes[]
 *	       box sizes structure				- VBOX *vbpar
 *	         minimum size x-axis			- double xmin
 *	         maximum size x-axis			- double xmax
 *	         minimum size y-axis			- double ymin
 *	         maximum size y-axis			- double ymax
 *	       clipping parameters structure		- CLIP *cpar
 *	         number of iterations			- int niter
 *		  size of the box used to calculate stddev- unsigned long sigbox[niter][2]
 *	         lower threshold in sigma			- double lsigma[niter]
 *		  upper threshold in sigma			- double hsigma[niter]
 *	         type of clipping				- int tclip[niter]
 *	         CCD characteristics			- CCD chip
 *	           read noise				- double rnoise
 *	           gain					- double gain
 *	           sensitivity noise			- double snoise
 *	Output: 2-D smooth image (new[jj][ii] = value)   - double ***pnew
 *
 * Output:
 *	Error code: 0 if no error; 1 if error;
 *
 * Calling boxcar
 *	status = boxcar(ppix,pmask,naxes,&vbpar,&cpar,&pnew);
 *
 * Needs:
 *	/astro/labrie/progc/general/headers/myutil.o
 *	/astro/labrie/progc/general/headers/mystruct.o
 *	/astro/labrie/progc/general/myfunc/stddevR2D.o	#V.1
 *	/astro/labrie/progc/general/myfunc/stddevF2D.o	#V.2
 *	-I/astro/labrie/progc/include
 *	         
 *
 * Notes:
 *	If no masking required, set pmask to NULL when calling.
 *	If no clipping required, set cpar to NULL when calling.
 *	For constant box size, set minimums equal to maximums in vbpar
 *	For relative box sizes, set box size to fraction of images (eg 0.25 for
 *		a box a quarter of the image size)
 *	Except for relative box sizes, use integer box sizes
 *
 *Needs:
 *   %%%libKLutil%%%
 *   %%%libKLimgutil%%%
 *   %%%libKLstats%%%
 */

#include "./imfilter.h"
#include <KLutil.h>
#include <KLimgutil.h>
#include <KLstats.h>	/* for stddevR2D(), stddevF2D() */
#include <stdio.h>
#include <stddef.h>
#include <math.h>

int boxcar( double **ppix, double **pmask, long naxes[], 
                 VBOX *vbpar, CLIP *cpar, double ***pnew )
{
 int iter,niter;
 long i, ii, jj;
 long d,ndigits;
 unsigned long halfsigbox[2], halfvbox[2];
 double  **pdeviant = NULL;		/* incremental (only grows) */
 STATS stats;
 RASTER rscan;

 printf("Boxcar filtering ... ");
 fflush(stdout);

 /* Check if pnew as been created. If not allocate memory. */
 if (*pnew == NULL) {
   *pnew = dmatrixc(naxes[1],naxes[0]);
 }

 /* Check if pdeviant is needed, if so, allocate memory. */
 /*   Needed if: pmask defined and/or cpar defined */
 if ((pmask != NULL) || (cpar != NULL)) {
   pdeviant = dmatrixc(naxes[1],naxes[0]);
 }

 /* If pmask defined, copy to pdeviant */
 if (pmask != NULL) {
   for (jj=0; jj<naxes[1]; jj++) {
     for (ii=0; ii<naxes[0]; ii++) {
       *(*(pdeviant + jj) + ii) = *(*(pmask + jj) + ii);
     }
   }
 }

 /* Set number of clipping iteration */
 if (cpar == NULL) { niter = 0; }
 else { niter = cpar->clip_niter; }

 /* Start clipping */
 for (iter=0; iter < niter; iter++) {
   if (iter==0) {printf("\tClipping ...\n");}
   printf("\t\tIteration %d of %d ... ", iter+1, niter);
   fflush(stdout);

   /* Initialize rscan */
   rscan.rstr_oldcol = rscan.rstr_newcol = -1;
   rscan.rstr_oldline = rscan.rstr_newline = -1;
   rscan.rstr_up = -1;		/* -1 : First pixel flag */

   /* For each pixel, calculate average and stddev in 'pix' and compare 'pix'
    * to 'avg +/- sigma*stddev'.  If outside range => deviant pixel.
    */
   printf("Line ");
   fflush(stdout);
   ndigits=0;
   for (jj=0; jj<naxes[1]; jj++) {
     if ( (jj%50) == 0) {
       for (d=0;d<ndigits;d++) printf("\b");
	if (jj==0) {ndigits=1;}
	else {ndigits=log10(jj) + 1;}
	printf("%d",jj+1);
	fflush(stdout);
     }
     if ( jj % 2) {		/* Going left */
       for (ii=naxes[0]-1; (long)ii>=0; (long)ii--) {
	  /* Calculate stddev and average */
	  stddev4clip(iter,cpar,ppix,pmask,ii,jj,naxes,halfsigbox,&stats,&rscan,pdeviant);
	  /* Clip */
	  clip(iter,ppix,ii,jj,&stats,cpar,pdeviant);
	}
     }
     else {			/* Going right */
       for (ii=0; ii<naxes[0]; ii++) {
	  /* Calculate stddev and average */
	  stddev4clip(iter,cpar,ppix,pmask,ii,jj,naxes,halfsigbox,&stats,&rscan,pdeviant);
	  /* Clip */
	  clip(iter,ppix,ii,jj,&stats,cpar,pdeviant);
	}
     }	
     rscan.rstr_up=1;
   }
   for (d=0;d<ndigits;d++) printf("\b");
   printf("\b\b\b\b\bdone ");
   for (d=0;d<ndigits;d++) printf(" ");
   printf("\n");
   fflush(stdout);
 }

 printf("\tApply boxcar ... ");
 fflush(stdout);

 /* Initialize rscan */
 rscan.rstr_oldcol = rscan.rstr_newcol = -1;
 rscan.rstr_oldline = rscan.rstr_newline = -1;
 rscan.rstr_up = -1;			/* -1 : First pixel flag */

 /* For each pixel, calculate average and save to pnew */
 printf("Line ");
 fflush(stdout);
 ndigits=0;
 for (jj=0; jj<naxes[1]; jj++) {
   if ( (jj%50) == 0) {
     for (d=0;d<ndigits;d++) printf("\b");
     if (jj==0) {ndigits=1;}
     else {ndigits=log10(jj) + 1;}
     printf("%d",jj+1);
     fflush(stdout);
   }
   if ( jj % 2) {		/* Going left */
     for (ii=naxes[0]-1; (long)ii>=0; (long)ii--) {
       /* Calculate halfvbox */
	halfvbox[0]=(unsigned long)vbpar->vbox_xmin/2;
	halfvbox[1]=(unsigned long)vbpar->vbox_ymin/2;
       /* Calculate average */
	if (pdeviant == NULL) {
		stddevF2D(ppix,ii,jj,naxes,halfvbox,&stats,&rscan);
	}
	else {
		stddevR2D(ppix,ii,jj,naxes,halfvbox,&stats,&rscan,pdeviant);
	}
	*(*(*pnew + jj) + ii) = stats.stats_ave;
     }
   }
   else {			/* Going right */
     for (ii=0; ii<naxes[0]; ii++) {
       /* Calculate halfvbox */
	halfvbox[0]=(unsigned long)vbpar->vbox_xmin/2;
	halfvbox[1]=(unsigned long)vbpar->vbox_ymin/2;
       /* Calculate average */
	if (pdeviant == NULL) {
		stddevF2D(ppix,ii,jj,naxes,halfvbox,&stats,&rscan);
	}
	else {
		stddevR2D(ppix,ii,jj,naxes,halfvbox,&stats,&rscan,pdeviant);
	}
	*(*(*pnew + jj) + ii) = stats.stats_ave;
     }
   }
   rscan.rstr_up=1;
 }
 for (d=0;d<ndigits;d++) printf("\b");
 printf("\b\b\b\b\bdone ");
 for (d=0;d<ndigits;d++) printf(" ");
 printf("\n");
 fflush(stdout);

 return(0);
}
