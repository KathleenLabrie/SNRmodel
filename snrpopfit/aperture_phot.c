/* Function : aperture_phot */
/* Version: 0.1.0 */
/*   Do apphot on one point given the image array.
 *
 * Needs:
 *   %%%libphot%%%
 */

#include "./snrpopfit.h"
#include <phot.h>
#include <stddef.h>

#define APPHOTPARSFILE "apphot.param"
#define DATAPARSFILE "data.param"
#define FITSKYPARSFILE "fitsky.param"
#define PHOTPARSFILE "phot.param"

int aperture_phot(double **ppix, long int naxes[], 
	double *fluxtot, double *fluxerr, double *stddev, double *stddeverr)
{
 int FLAGS=0,status=0;
 double **pcoords=NULL;
 FILE *istream=NULL;
 APPHOTPARS apphotpars;
 DATAPARS datapars;
 FITSKYPARS fitskypars;
 PHOTPARS photpars;
 RESULTS results;

 /* READ THE APPHOT PARAMETER FILE */
 /* ----------- APPHOT_PARS ------------ */
 if ( (istream = fopen(APPHOTPARSFILE,"r")) == NULL) {
 	fprintf(stderr,ERRMSG_FILE_NOT_FOUND,APPHOTPARSFILE);
	return(ERRNO_FILE_NOT_FOUND);
 }
 if ( status = rd_apphot_param(istream, &apphotpars) ) {
   	fclose(istream);
   	fprintf(stderr,ERRMSG_IO_ERROR,APPHOTPARSFILE);
	return(ERRNO_IO_ERROR);
 }
 fclose(istream);
 istream=NULL;

 /* ----------- DATA_PARS ------------ */
 if ( (istream = fopen(DATAPARSFILE,"r")) == NULL) {
 	fprintf(stderr,ERRMSG_FILE_NOT_FOUND,DATAPARSFILE);
	return(ERRNO_FILE_NOT_FOUND);
 }
 if ( status = rd_data_param(istream, &datapars) ) {
   	fclose(istream);
   	fprintf(stderr,ERRMSG_IO_ERROR,DATAPARSFILE);
	return(ERRNO_IO_ERROR);
 }
 fclose(istream);
 istream=NULL;

 /* ----------- FITSKY_PARS ------------ */
 if ( (istream = fopen(FITSKYPARSFILE,"r")) == NULL) {
 	fprintf(stderr,ERRMSG_FILE_NOT_FOUND,FITSKYPARSFILE);
	return(ERRNO_FILE_NOT_FOUND);
 }
 if ( status = rd_fitsky_param(istream, &fitskypars) ) {
   	fclose(istream);
   	fprintf(stderr,ERRMSG_IO_ERROR,FITSKYPARSFILE);
	return(ERRNO_IO_ERROR);
 }
 fclose(istream);
 istream=NULL;

 /* ----------- PHOT_PARS ------------ */
 if ( (istream = fopen(PHOTPARSFILE,"r")) == NULL) {
 	fprintf(stderr,ERRMSG_FILE_NOT_FOUND,PHOTPARSFILE);
	return(ERRNO_FILE_NOT_FOUND);
 }
 if ( status = rd_phot_param(istream, &photpars) ) {
   	fclose(istream);
   	fprintf(stderr,ERRMSG_IO_ERROR,PHOTPARSFILE);
	return(ERRNO_IO_ERROR);
 }
 fclose(istream);
 istream=NULL;

 /* Get coordinates */
 if ( (istream = fopen(apphotpars.apphot_coords,"r")) == NULL) {
   fprintf(stderr,ERRMSG_FILE_NOT_FOUND,apphotpars.apphot_coords);
   return(ERRNO_FILE_NOT_FOUND);
 }
 if ( status = rd_coords(istream, &pcoords, &results.res_ncoo) ) {
   fclose(istream);
   return(status);
 }
 fclose(istream);
 istream=NULL;

 results.res_ncoo = 1;		/* force use of only the first point */
 results.res_nradii = 1;	/* force use of only the first radius */

 alloc_results(&results);
 if (status = fitsky( ppix, naxes, pcoords,0,fitskypars,photpars,datapars,
 			&results,FLAGS) ) {
	free_dmatrix(pcoords);
	free_results(&results);
	return(status);
 }
 
 status = aptotflux(ppix,naxes,pcoords,0,photpars,datapars,&results,FLAGS);
 
 *fluxtot = results.res_totalflux[0][0];
 *fluxerr = results.res_error[0][0];
 *stddev = results.res_apstddev[0][0];
 *stddeverr = 0.05* (*stddev);

 return(status);
}
