/* Function: aptotflux */
/* Version: 0.1.0 */
/*   Compute total flux within aperture 
 *
 * int aptotflux( double **ppix, long int naxes[], double **pcoo, int idcoo,
 *		  PHOTPARS photpars, RESULTS *results )
 *
 * Usage: status = aptotflux( ppix, naxes, pcoords, idcoo, photpars, &results );
 *
 * Needs:
 */

#include "./phot.h"
#include <stdio.h>
#include <KLutil.h>
#include <math.h>
#include <float.h>	/* for DBL_EPSILON */
#include <string.h>	/* for gcc4.0, strcpy */

int aptotflux( double **ppix, long int naxes[], double **pcoo, int idcoo,
		PHOTPARS ppars, DATAPARS dpars, RESULTS *res, int FLAGS )
{
 int i,status=0, n=0;
 long int ii, jj;
 double x0,y0,xmin,xmax,ymin,ymax;
 double dr2,dx,dy,theta;
 double r2,a2,b2,pa,top,alpha;
 char debug_file[MAXLENGTH];
 FILE *debug_pixused,*debug_allpix;

 x0 = *(*(pcoo+0)+idcoo)-1;  /* -1 to transform to 0-array ref frame of ppix */
 y0 = *(*(pcoo+1)+idcoo)-1;
 pa = ppars.phot_pa;

 /* loop through radii */
 for (i=0;i<ppars.phot_n;i++) {
   n=0;
   if (FLAGS & 1 << DEBUG) {
     sprintf(debug_file,"debug_pixused%d",i+1);
     debug_pixused = fopen(debug_file,"w");
     strcpy(debug_file,"");
     sprintf(debug_file,"debug_allpix%d",i+1);
     debug_allpix = fopen(debug_file,"w");
   }
   a2 = ppars.phot_radii[i] * ppars.phot_radii[i];
   b2 = a2 * ( 1 - ppars.phot_ellip );
   top = a2 * b2;
   xmin = x0 - ppars.phot_radii[i] - 1.;  /* just make sure you test a big */
   xmax = x0 + ppars.phot_radii[i] + 1.;  /* enough area */
   ymin = y0 - ppars.phot_radii[i] - 1.;
   ymax = y0 + ppars.phot_radii[i] + 1.;
   if (xmin < 0) { xmin=0; }
   if (xmax >= naxes[0]) { xmax = naxes[0]-1; }
   if (ymin < 0) { ymin=0; }
   if (ymax >= naxes[1]) { ymax = naxes[1]-1; }
   for (jj=(long int)ymin; jj<=(long int)ymax; jj++) {
     for (ii=(long int)xmin; ii<=(long int)xmax; ii++) {
       /* rejection */
       if (( *(*(ppix+jj)+ii) > dpars.data_datamin) && 
       		( *(*(ppix+jj)+ii) < dpars.data_datamax)) {
	  /* calculate location */
	  dx = ii-x0;
	  dy = jj-y0;
	  theta = atan2(dy,dx);
	  dr2 = dx*dx + dy*dy;
	  alpha = pa*PI/180.-theta;
	  r2 = top / (a2*cos(alpha)*cos(alpha) + b2*sin(alpha)*sin(alpha));
	  if (FLAGS & 1<<DEBUG) {fprintf(debug_allpix,"%d  %d  %f  %f\n",ii+1,jj+1,dr2,r2);}
	  if (dr2<=r2+DBL_EPSILON) {
            n++;
            res->res_totalflux[i][idcoo] += *(*(ppix+jj)+ii) - res->res_skyperpix[idcoo];
	    if (FLAGS & 1<<DEBUG) { fprintf(debug_pixused,"%d  %d\n",ii+1,jj+1);}
	  }
       }
     }
   }
   if (FLAGS & 1<<DEBUG) {
     fclose(debug_pixused);
     fclose(debug_allpix);
   }
   res->res_npix[i][idcoo] = n;
   res->res_error[i][idcoo] = 
   	sqrt(res->res_totalflux[i][idcoo]/dpars.data_gain +
   	     (double)res->res_npix[i][idcoo]*pow(res->res_skystddev[idcoo],2) +
	     pow((double)res->res_npix[i][idcoo],2)*
	     pow(res->res_skystddev[idcoo],2)/(double)res->res_nskypix[idcoo]);
 }

 return(status);
}
