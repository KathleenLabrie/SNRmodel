/* Function: fitsky */
/* Version: 0.1.0 */
/*   Fit sky background
 *
 * int fitsky( double **ppix, long int naxes[], FITSKYPARS pars,
 *		 RESULTS *res )
 *
 * Usage: status = fitsky( ppix, naxes, fitskypars, &results );
 *
 * Needs:
 */

#include "./phot.h"
#include <stdio.h>
#include <math.h>
#include <float.h>	/* for DBL_EPSILON */
#include <KLutil.h>
#include <KLstats.h>	/* for dquickfmedian(),stddev() */
#include <string.h>

int fitsky( double **ppix, long int naxes[], double **pcoo, int idcoo, 
		FITSKYPARS fspars, PHOTPARS ppars, DATAPARS dpars, 
		RESULTS *res, int FLAGS )
{
 int status=0;
 unsigned long i,npix,maxpix;
 long int ii,jj;
 double x0,y0,xmin,xmax,ymin,ymax;
 double dr2,dx,dy,theta,alpha;
 double router2,rinner2,aouter2,bouter2,ainner2,binner2,topouter,topinner,pa;
 double *skypix;
 double mean,stddev,variance;
 char debug_file[MAXLENGTH];
 FILE *debug_skypix;

 if ( strncmp(fspars.fitsky_algorithm,"mean",4) &&
      strncmp(fspars.fitsky_algorithm,"median",6) ) {
   fprintf(stderr,ERRMSG_INPUT_ERROR,fspars.fitsky_algorithm);
   return(ERRNO_INPUT_ERROR);
 }
 /* +/- 1 just to make sure the array is big enough */
 maxpix = (unsigned long)( PI * ( 
 		pow(fspars.fitsky_annulus+fspars.fitsky_wannulus+1,2) -
 		pow(fspars.fitsky_annulus-1,2) 
		));
 skypix = dvector(maxpix);

 x0 = *(*(pcoo+0)+idcoo)-1;  /* -1 to transform to 0-array ref frame of ppix */
 y0 = *(*(pcoo+1)+idcoo)-1;
 pa = ppars.phot_pa;

/* printf("(%f,%f)\n",x0+1,y0+1);*/
  
 aouter2 = pow(fspars.fitsky_annulus+fspars.fitsky_wannulus,2);
 bouter2 = aouter2 * (1 - ppars.phot_ellip);
 ainner2 = pow(fspars.fitsky_annulus,2);
 binner2 = ainner2 * (1 - ppars.phot_ellip);
 topouter = aouter2*bouter2;
 topinner = ainner2*binner2;

 /* the +/- 1 are just to make sure I test a big enough area */
 xmin = x0 - (fspars.fitsky_annulus+fspars.fitsky_wannulus) - 1;
 xmax = x0 + (fspars.fitsky_annulus+fspars.fitsky_wannulus) + 1;
 ymin = y0 - (fspars.fitsky_annulus+fspars.fitsky_wannulus) - 1;
 ymax = y0 + (fspars.fitsky_annulus+fspars.fitsky_wannulus) + 1;

 if (xmin < 0) { xmin=0; }
 if (xmax >= naxes[0]) { xmax = naxes[0]-1; }
 if (ymin < 0) { ymin=0; }
 if (ymax >= naxes[1]) { ymax = naxes[1]-1; }
 if (FLAGS & 1<<DEBUG) {
   sprintf(debug_file,"debug_skypix");
   debug_skypix = fopen(debug_file,"a+");
 }
 npix=0;
 for (jj=(long int)ymin; jj<=(long int)ymax; jj++) {
   for (ii=(long int)xmin; ii<=(long int)xmax; ii++) {
     /* first level rejection */
     if (( *(*(ppix+jj)+ii) > dpars.data_datamin )  &&
     		( *(*(ppix+jj)+ii) < dpars.data_datamax)) {
	/* calculate location */
	dx = ii-x0;
	dy = jj-y0;
	theta = atan2(dy,dx);
	dr2 = dx*dx + dy*dy;
	alpha = pa*PI/180. - theta;
	router2 = topouter / 
       		 (aouter2*cos(alpha)*cos(alpha) + bouter2*sin(alpha)*sin(alpha));
	rinner2 = topinner / 
       		 (ainner2*cos(alpha)*cos(alpha) + binner2*sin(alpha)*sin(alpha));
	if ( (dr2>=rinner2-DBL_EPSILON) && (dr2<=router2+DBL_EPSILON) ) {
	  *(skypix+npix)= *(*(ppix+jj)+ii);
	  npix++;
	  if (FLAGS & 1<<DEBUG) { fprintf(debug_skypix,"%ld  %ld\n",ii+1,jj+1); }
	}
     }   
   }
 }
 if (FLAGS & 1<<DEBUG) { fclose(debug_skypix); }
 res->res_serr[idcoo] = skyvalue(skypix,npix,fspars,
 				&res->res_skyperpix[idcoo],
				&res->res_skystddev[idcoo],
				&res->res_nskypix[idcoo], FLAGS);
printf("skyperpix=%f  skystddev=%f  nskypix=%d\n",res->res_skyperpix[idcoo],
				res->res_skystddev[idcoo],
				res->res_nskypix[idcoo]);
/* dstddev(skypix,npix,&mean,&stddev,&variance);
 res->res_nskypix[idcoo] = npix;
 res->res_skystddev[idcoo] = stddev;
 if ( !strncmp(fspars.fitsky_algorithm,"mean",4) ) {
   res->res_skyperpix[idcoo] = mean;
 }
 else if ( !strncmp(fspars.fitsky_algorithm,"median",6) ) {
   res->res_skyperpix[idcoo] = dquickfmedian(skypix, npix);
 }
 else {
   fprintf(stderr,ERRMSG_INPUT_ERROR,fspars.fitsky_algorithm);
   free_dvector(skypix);
   return(ERRNO_INPUT_ERROR);
 }*/

 free_dvector(skypix);
 return(status);
}
