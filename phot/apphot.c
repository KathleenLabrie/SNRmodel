/* Main : apphot */
/* Version: 0.1.0 */
/*   Aperture photometry.  Apertures can be circular or elliptical.
 *
 * Usage: apphot [-v] images [-o output -p paramfile]
 *	  apphot [-v] -l imglist [-o output -p paramfile]
 *
 *	images		: Input images [FITS file]
 *	-l imglist	: List of input images
 *	-o output	: Result file
 *	-p paramfile	: Name of parameter file [default: apphot.param]
 *			  (Searches current directory first then looks into
 *			   installation directory.)
 *	-v		: Toggle on verbose
 *
 * Parameters:
 *	apphot.param	: apphot parameters
 *	fitsky.param	: sky fitting parameters
 *	data.param	: data dependent parameters
 *	phot.param	: photometry parameters
 *
 * Needs
 *   %%%libKLcfitsio%%%
 *   %%%libKLutil%%%
 */

#include "./phot.h"
#include <KLutil.h>
#include <KLcfitsio.h>		/* my cfitsio wrappers */
#include <stddef.h>		/* for NULL */
#include <string.h>		/* for strcpy, strncmp */
#include <stdlib.h>		/* for atof() */

main( argc, argv )
int argc;
char *argv[];
{
 int i,j,n,status=0;
 int FLAGS;
 long int naxes[2];
 double **pcoords, **ppix=NULL;
 char **p_images=NULL,imglist[MAXLENGTH];
 char line[MAXLENGTH],**p_arguments;
 char apphotpars_file[MAXLENGTH];
 char output[MAXLENGTH], the_default[MAXLENGTH],outfile[MAXLENGTH];
 char **p_parse,header_value[MAXLENGTH],comment[MAXLENGTH];
 FILE *istream=NULL;
 APPHOTPARS apphotpars;
 DATAPARS datapars;
 FITSKYPARS fitskypars;
 PHOTPARS photpars;
 RESULTS results;

 /* Initialize */
 FLAGS=0;
 /*FLAGS |= 1 << DEBUG;*/
 strcpy(apphotpars_file,"");
 strcpy(imglist,"");
 strcpy(output,"");
 p_arguments = svector(MAX_N_IMAGES,MAXLENGTH);

 
 /* Read command line */
 n=0;
 i=1;
 while (i < argc) {
   if (!strncmp("-l",argv[i],2)) { strcpy(imglist,argv[++i]); }
   else if (!strncmp("-o",argv[i],2)) { strcpy(output,argv[++i]); }
   else if (!strncmp("-p",argv[i],2)) { strcpy(apphotpars_file,argv[++i]); }
   else if (!strncmp("-v",argv[i],2)) { FLAGS |= 1 << VERBOSE; }
   else {
     strcpy(p_arguments[n++],argv[i]);
   }
   i++;
 }

 /* READ THE APPHOT PARAMETER FILE */
 /* ----------- APPHOT_PARS ------------ */
 if ( strlen(apphotpars_file) == 0 ) { 
 	strcpy(apphotpars_file,DEFAULT_APPHOT_PARS);
 }
 if ( (istream = fopen(apphotpars_file,"r")) == NULL) { /* look in current dir 
 						      or as is */
   sprintf(the_default,"%s%s",DEFAULT_DIR,apphotpars_file);
   if ( (istream = fopen(the_default,"r")) == NULL) {
     fprintf(stderr,ERRMSG_FILE_NOT_FOUND,apphotpars_file);
     free_svector(p_images);
     exit(ERRNO_FILE_NOT_FOUND);
   }
   if (FLAGS & 1 << VERBOSE) { printf("WARNING: Using '%s'\n",the_default); }
 }
 if ( status = rd_apphot_param(istream, &apphotpars) ) {
   fclose(istream);
   free_svector(p_images);
   exit(status);
 }
 fclose(istream);
 istream=NULL;

 /* read image list */ 
 if (strlen(imglist) != 0 ) {
    if ( (istream = fopen(imglist,"r")) == NULL) {
      fprintf(stderr,ERRMSG_FILE_NOT_FOUND,imglist);
      free_svector(p_arguments);
      exit(ERRNO_FILE_NOT_FOUND);
    }
 } else if (strlen(apphotpars.apphot_imglist) != 0) {
    if ( (istream = fopen(apphotpars.apphot_imglist,"r")) == NULL) {
      fprintf(stderr,ERRMSG_FILE_NOT_FOUND,apphotpars.apphot_imglist);
      free_svector(p_arguments);
      exit(ERRNO_FILE_NOT_FOUND);
    }
 }
 if (istream != NULL) {
   while (fgets( line, MAXLENGTH, istream ) != NULL) {
     p_parse = svector(1,MAXLENGTH);
     splitstr(line,p_parse,SPACES);
     strcpy(p_arguments[n++],p_parse[0]);
     free_svector(p_parse);
   }
   fclose(istream);
   istream=NULL;
 }

 /* set image vector */
 if (n>0) {
    p_images = svector(n,MAXLENGTH);
    for (i=0;i<n;i++) { strcpy(p_images[i],p_arguments[i]); }
    apphotpars.apphot_n = n;
    free_svector(p_arguments);
 } else {
    p_images = svector(apphotpars.apphot_n,MAXLENGTH);
    for (i=0;i<apphotpars.apphot_n;i++) {
    	strcpy(p_images[i],apphotpars.apphot_images[i]);
    }
    free_svector(apphotpars.apphot_images);
 }
 
 if (apphotpars.apphot_n <=0 ) {
    fprintf(stderr,ERRMSG_INPUT_ERROR);
    fprintf(stderr,"\tNo images specified\n");
    exit(ERRNO_INPUT_ERROR);
 }

 
 /* ----------- DATA_PARS ------------ */
 if ( (istream = fopen(apphotpars.apphot_datapars,"r")) == NULL) { /* look in current dir
 							or as is */
   sprintf(the_default,"%s%s",DEFAULT_DIR,apphotpars.apphot_datapars);
   if ( (istream = fopen(the_default,"r")) == NULL) { /* look in def dir */
     fprintf(stderr,ERRMSG_FILE_NOT_FOUND,apphotpars.apphot_datapars);
     free_svector(p_images);
     exit(ERRNO_FILE_NOT_FOUND);
   }
   if (FLAGS & 1 << VERBOSE) { printf("WARNING: Using '%s'\n",the_default); }
 }
 if ( status = rd_data_param(istream, &datapars) ) {
   fclose(istream);
   free_svector(p_images);
   exit(status);
 }
 fclose(istream);
 istream=NULL;

 /* ----------- FITSKY_PARS ------------ */
 if ( (istream = fopen(apphotpars.apphot_fitskypars,"r")) == NULL) {
   sprintf(the_default,"%s%s",DEFAULT_DIR,apphotpars.apphot_fitskypars);
   if ( (istream = fopen(the_default,"r")) == NULL) {
     fprintf(stderr,ERRMSG_FILE_NOT_FOUND,apphotpars.apphot_fitskypars);
     free_svector(p_images);
     exit(ERRNO_FILE_NOT_FOUND);
   }
   if (FLAGS & 1 << VERBOSE) { printf("WARNING: Using '%s'\n",the_default); }
 }
 if ( status = rd_fitsky_param(istream, &fitskypars) ) {
   fclose(istream);
   free_svector(p_images);
   exit(status);
 }
 fclose(istream);
 istream=NULL;

 /* ----------- PHOT_PARS ------------ */
 if ( (istream = fopen(apphotpars.apphot_photpars,"r")) == NULL ) {
   sprintf(the_default,"%s%s",DEFAULT_DIR,apphotpars.apphot_photpars);
   if ( (istream = fopen(the_default,"r")) == NULL) {
     fprintf(stderr,ERRMSG_FILE_NOT_FOUND,apphotpars.apphot_photpars);
     free_svector(p_images);
     exit(ERRNO_FILE_NOT_FOUND);
   }
   if (FLAGS & 1 << VERBOSE) { printf("WARNING: Using '%s'\n",the_default); }
 }
 if ( status = rd_phot_param(istream, &photpars) ) {
   fclose(istream);
   free_svector(p_images);
   exit(status);
 }
 fclose(istream);
 istream=NULL;

/* Get coordinates */
 if ( (istream = fopen(apphotpars.apphot_coords,"r")) == NULL) {
   fprintf(stderr,ERRMSG_FILE_NOT_FOUND,apphotpars.apphot_coords);
   free_svector(p_images);
   exit(ERRNO_FILE_NOT_FOUND);
 }
 if ( status = rd_coords(istream, &pcoords, &results.res_ncoo) ) {
   fclose(istream);
   free_svector(p_images);
   exit(status);
 }
 fclose(istream);
 istream=NULL;

 results.res_nradii = photpars.phot_n;
 /* Run through the images */
 for (i=0;i<apphotpars.apphot_n;i++) {
   /* Load image */
   if ( status = read_image( p_images[i], &ppix, naxes ) ) {
     if (ppix != NULL) { free_dmatrix(ppix); }
     free_svector(p_images);
     printerror( status );
   }
   /* Find exposure time and store in datapars */
   if ( strcmp(datapars.data_hexposure,"INDEF") ) {
     if ( status = read_key( p_images[i], datapars.data_hexposure,
   			     header_value, "double", comment ) ) {
       free_dmatrix(ppix);
       free_svector(p_images);
       printerror( status );
     }
     datapars.data_itime = atof(header_value);
   }
   alloc_results(&results);
   for (j=0;j<results.res_ncoo;j++) {
     /* fit sky */
     if (status = fitsky( ppix,naxes,pcoords,j,fitskypars,photpars,datapars,
     			  &results,FLAGS)) {
       free_svector(p_images);
       free_dmatrix(ppix);
       free_dmatrix(pcoords);
       free_results(&results);
       exit(status);
     }
     /* printf("%f\n",results.res_skyperpix[j]);*/

     /* find total flux */
     status = aptotflux(ppix,naxes,pcoords,j,photpars,datapars,&results,FLAGS);
     /* calculate flux and magnitude */
     status = mag(pcoords, j, photpars, datapars, &results);
   }
   
   /* print result to file */
   if (strlen(output) == 0) {
     strcpy(output,apphotpars.apphot_output);
   }
   sprintf(outfile,"%s.%s",p_images[i],output);
   print_result(p_images[i],outfile,pcoords,datapars,fitskypars,photpars,results);
   
   /* Reset ppix and result structures */
   free_dmatrix(ppix);
   ppix=NULL;
   free_results(&results);
 }
 
 free_svector(p_images);
 exit(0);
}
