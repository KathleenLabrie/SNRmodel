/* Function: print_result */
/* Version: 0.1.0 */
/*   Print apperture photometry results to file 
 *
 * int print_result( char imgname[], char outfile[], double **pcoo,
 *			DATAPARS datapars, FITSKYPARS fitskypars, 
 *			PHOTPARS photpars, RESULTS results )
 *
 * Usage: status = print_result( imgname, outfile, pcoo, datapars, fitskypars,
 *				 photpars, results );
 *
 * Needs:
 */

#include "./phot.h"
#include <stdio.h>

int print_result( char imgname[], char outfile[], double **pcoo, 
		  DATAPARS datapars, FITSKYPARS fitskypars, PHOTPARS photpars, 
		  RESULTS results )
{
 int status=0;
 int idcoo,idrad;
 FILE *ostream;
 
 if ( (ostream = fopen(outfile,"w")) == NULL ) {
    fprintf(stderr,ERRMSG_IO_ERROR,outfile);
    return(ERRNO_IO_ERROR);
 }
 fprintf(ostream,"#id   xcenter  ycenter   sky      stddev   nskypix\n");
 fprintf(ostream,"#              npix      total_flux  error");
 fprintf(ostream,"      flux/sec   mag      merr\n");
 for (idcoo=0;idcoo<results.res_ncoo;idcoo++) {
   fprintf(ostream,"%-4d  %-7.2f  %-7.2f   %-7.2f  %-7.2f  %-5d\n",
   			idcoo+1,*(*(pcoo+0)+idcoo),
   			*(*(pcoo+1)+idcoo),results.res_skyperpix[idcoo],
			results.res_skystddev[idcoo],
			results.res_nskypix[idcoo]);
   for (idrad=0;idrad<results.res_nradii;idrad++) {
     fprintf(ostream,"               %-5d     ",results.res_npix[idrad][idcoo]);
     fprintf(ostream,"%-10.2f  %-9.2f  %-9.2f  %-7.3f  %-5.3f\n",
     			results.res_totalflux[idrad][idcoo],
			results.res_error[idrad][idcoo],
     			results.res_fluxsec[idrad][idcoo],
			results.res_mag[idrad][idcoo],
			results.res_merr[idrad][idcoo]);
   }			
 }
 fclose(ostream);
 ostream=NULL;

 return(status);
}
