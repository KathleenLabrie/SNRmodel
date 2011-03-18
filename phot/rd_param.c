/* Function Package : rd_param */
/* Version: 0.1.0 */
/*   Functions used to read parameter files associated to the 'phot' package
 *
 * int rd_apphot_param( FILE istream, APPHOTPARS *pars )
 * int rd_data_param( FILE istream, DATAPARS *pars )
 * int rd_fitsky_param ( FILE istream, FITSKYPARS *pars )
 * int rd_phot_param ( FILE istream, PHOTPARS *pars )
 * int assign_double_param( char *pstr, double *param )
 * int assign_str_param( char *pstr, char param[] )
 * int warn_unknown_param( char line[] )
 */

#include "./phot.h"
#include <KLutil.h>	/* for definitions */
#include <stddef.h>	/* for NULL */
#include <string.h>
#include <stdlib.h>	/* for atof() */

/* Function : rd_apphot_param */
/* Version: 0.1.0 */
/*   Read apphot.param file 
 *
 * int rd_apphot_param( FILE istream, APPHOTPARS *pars )
 *
 * Usage: status = rd_apphot_param(istream, &apphotpars);
 */

int rd_apphot_param( FILE *istream, APPHOTPARS *pars )
{
 int status=0;
 int i;
 char line[2*MAXLENGTH], *pstr, tmp[2*MAXLENGTH],**p_images;

 p_images = svector(MAX_N_IMAGES,2*MAXLENGTH);
 while (fgets( line, 2*MAXLENGTH, istream ) != NULL) {
   if ( !strncmp(line,"apphot.image=",13) ) {
     pstr = line+13;
     status = assign_str_param( pstr, tmp );
     if (strlen(tmp) != 0) {
       pars->apphot_n = splitstr(tmp,p_images,",");
       pars->apphot_images = svector(pars->apphot_n,2*MAXLENGTH);
       for (i=0;i<pars->apphot_n;i++) {
         strcpy(pars->apphot_images[i],p_images[i]);
       }
       free_svector(p_images);
     }
   }
   else if ( !strncmp(line,"apphot.imglist=",15) ) {
     pstr = line+15;
     status = assign_str_param( pstr, pars->apphot_imglist );
   }
   else if ( !strncmp(line,"apphot.coords=",14) ) {
     pstr = line+14;
     status = assign_str_param( pstr, pars->apphot_coords );
   }
   else if ( !strncmp(line,"apphot.output=",14) ) {
     pstr = line+14;
     status = assign_str_param( pstr, pars->apphot_output );
     if (!strncmp(pars->apphot_output,"default",strlen(pars->apphot_output))) {
       strcpy(pars->apphot_output,DEFAULT_OUTPUT_SUFFIX);
     }
   }
   else if ( !strncmp(line,"apphot.datapars=",16) ) {
     pstr = line+16;
     status = assign_str_param( pstr, pars->apphot_datapars );
     if (!strncmp(pars->apphot_datapars,"default",strlen(pars->apphot_datapars))) {
       strcpy(pars->apphot_datapars,DEFAULT_DATA_PARS);
     }
   }
   else if ( !strncmp(line,"apphot.fitskypars=",18) ) {
     pstr = line+18;
     status = assign_str_param( pstr, pars->apphot_fitskypars );
     if (!strncmp(pars->apphot_fitskypars,"default",strlen(pars->apphot_fitskypars))) {
       strcpy(pars->apphot_fitskypars,DEFAULT_FITSKY_PARS);
     }
   }
   else if ( !strncmp(line,"apphot.photpars=",16) ) {
     pstr = line+16;
     status = assign_str_param( pstr, pars->apphot_photpars );
     if (!strncmp(pars->apphot_photpars,"default",strlen(pars->apphot_photpars))) {
       strcpy(pars->apphot_photpars,DEFAULT_PHOT_PARS);
     }
   }
   else {
     return(warn_unknown_param(line));
   }
 }

 return(status);
}


/* Function : rd_data_param */
/* Version: 0.1.0 */
/*   Read data.param file 
 *
 * int rd_data_param( FILE istream, DATAPARS *pars )
 *
 * Usage: status = rd_data_param(istream, datapars);
 */
int rd_data_param( FILE *istream, DATAPARS *pars )
{
 int status=0;
 char line[2*MAXLENGTH], *pstr, tmp[2*MAXLENGTH];

 while (fgets( line, 2*MAXLENGTH, istream ) != NULL) {
   if ( !strncmp(line, "datapars.scale=",15) ) {
     pstr = line+15;
     if (status = assign_double_param( pstr, &pars->data_scale )) {
       fprintf(stderr,ERRMSG_MISSING_INPUT,"datapars.scale");
       return(status);
     }
   }
   else if ( !strncmp(line, "datapars.datamin=",17) ) {
     pstr = line+17;
     status = assign_str_param( pstr, tmp );
     if (!strcmp(tmp,"INDEF")) { pars->data_datamin = SMALL_NUMBER; }
     else { pars->data_datamin = atof(tmp); }
   }
   else if ( !strncmp(line, "datapars.datamax=",17) ) {
     pstr = line+17;
     status = assign_str_param( pstr, tmp );
     if (!strcmp(tmp,"INDEF")) { pars->data_datamax = BIG_NUMBER; }
     else { pars->data_datamax = atof(tmp); }
   }
   else if ( !strncmp(line, "datapars.hexposure=",19) ) {
     pstr = line+19;
     status = assign_str_param( pstr, pars->data_hexposure );
   }
   else if ( !strncmp(line, "datapars.hairmass=",18) ) {
     pstr = line+18;
     status = assign_str_param( pstr, pars->data_hairmass );
   }
   else if ( !strncmp(line, "datapars.itime=",15) ) {
     pstr = line+15;
     if (status = assign_double_param( pstr, &pars->data_itime )) {
       fprintf(stderr,ERRMSG_MISSING_INPUT,"datapars.itime");
       return(status);
     }
   }
   else if ( !strncmp(line, "datapars.gain=",14) ) {
     pstr = line+14;
     if (status = assign_double_param( pstr, &pars->data_gain )) {
       fprintf(stderr,ERRMSG_MISSING_INPUT,"datapars.gain");
       return(status);
     }
   }
   else {
     return(warn_unknown_param(line));
   }
 }

 return(status);
}

/* Function : rd_fitsky_param */
/* Version: 0.1.0 */
/*   Read fitsky.param file 
 *
 * int rd_fitsky_param( FILE istream, FITSKYPARS *pars )
 *
 * Usage: status = rd_fitsky_param(istream, fitskypars);
 */
int rd_fitsky_param( FILE *istream, FITSKYPARS *pars )
{
 int status=0;
 char line[2*MAXLENGTH], *pstr;

 while (fgets( line, 2*MAXLENGTH, istream ) != NULL) {
   if ( !strncmp(line, "fitskypars.algorithm=",21) ) {
     pstr = line+21;
     status = assign_str_param( pstr, pars->fitsky_algorithm );
   }
   else if ( !strncmp(line, "fitskypars.annulus=",19) ) {
     pstr = line+19;
     if (status = assign_double_param( pstr, &pars->fitsky_annulus )) {
       fprintf(stderr,ERRMSG_MISSING_INPUT,"fitskypars.annulus");
       return(status);
     }
   }
   else if ( !strncmp(line, "fitskypars.wannulus=",20) ) {
     pstr = line+20;
     if (status = assign_double_param( pstr, &pars->fitsky_wannulus )) {
       fprintf(stderr,ERRMSG_MISSING_INPUT,"fitskypars.wannulus");
       return(status);
     }
   }
   else if ( !strncmp(line, "fitskypars.skyvalue=",20) ) {
     pstr = line+20;
     if (status = assign_double_param( pstr, &pars->fitsky_skyvalue )) {
       fprintf(stderr,ERRMSG_MISSING_INPUT,"fitskypars.skyvalue");
       return(status);
     }
   }
   else if ( !strncmp(line, "fitskypars.maxiter=",19) ) {
     pstr = line+19;
     if (status = assign_int_param( pstr, &pars->fitsky_maxiter )) {
       fprintf(stderr,ERRMSG_MISSING_INPUT,"fitskypars.maxiter");
       return(status);
     }
   }
   else if ( !strncmp(line, "fitskypars.maxreject=",21) ) {
     pstr = line+21;
     if (status = assign_int_param( pstr, &pars->fitsky_maxreject )) {
       fprintf(stderr,ERRMSG_MISSING_INPUT,"fitskypars.maxreject");
       return(status);
     }
   }
   else if ( !strncmp(line, "fitskypars.nkeep=",17) ) {
     pstr = line+17;
     if (status = assign_int_param( pstr, &pars->fitsky_nkeep )) {
       fprintf(stderr,ERRMSG_MISSING_INPUT,"fitskypars.nkeep");
       return(status);
     }
   }
   else if ( !strncmp(line, "fitskypars.loreject=",20) ) {
     pstr = line+20;
     if (status = assign_double_param( pstr, &pars->fitsky_loreject )) {
       fprintf(stderr,ERRMSG_MISSING_INPUT,"fitskypars.loreject");
       return(status);
     }
   }
   else if ( !strncmp(line, "fitskypars.hireject=",20) ) {
     pstr = line+20;
     if (status = assign_double_param( pstr, &pars->fitsky_hireject )) {
       fprintf(stderr,ERRMSG_MISSING_INPUT,"fitskypars.hireject");
       return(status);
     }
   }
   else { return(warn_unknown_param(line)); }
 }

 return(status);
}


/* Function : rd_phot_param */
/* Version: 0.1.0 */
/*   Read phot.param file 
 *
 * int rd_phot_param( FILE istream, PHOTPARS *pars )
 *
 * Usage: status = rd_phot_param(istream, photpars);
 */
int rd_phot_param( FILE *istream, PHOTPARS *pars )
{
 int i,status=0;
 char line[2*MAXLENGTH], *pstr, tmp[2*MAXLENGTH], **p_radii;

 p_radii = svector(MAX_N_RADII,2*MAXLENGTH);
 while (fgets( line, 2*MAXLENGTH, istream ) != NULL) {
   if ( !strncmp(line, "photpars.ashape=",16) ) {
     pstr = line+16;
     status = assign_str_param( pstr, pars->phot_ashape );
   }
   else if ( !strncmp(line, "photpars.radii=",15) ) {
     pstr = line+15;
     status = assign_str_param( pstr, tmp );
     if (strlen(tmp) != 0) {
       pars->phot_n = splitstr(tmp,p_radii,",");
       pars->phot_radii = dvector(pars->phot_n);
       for (i=0;i<pars->phot_n;i++) {
         pars->phot_radii[i] = atof(p_radii[i]);
       }
       free_svector(p_radii);
     }
     else {
       fprintf(stderr,ERRMSG_MISSING_INPUT,"photpars.radii");
       return(ERRNO_MISSING_INPUT);
     }
   }
   else if ( !strncmp(line, "photpars.ellip=",15) ) {
     pstr = line+15;
     if (status = assign_double_param( pstr, &pars->phot_ellip )) {
       fprintf(stderr,ERRMSG_MISSING_INPUT,"photpars.ellip");
       return(status);
     }
   }
   else if ( !strncmp(line, "photpars.pa=",12) ) {
     pstr = line+12;
     if (status = assign_double_param( pstr, &pars->phot_pa )) {
       fprintf(stderr,ERRMSG_MISSING_INPUT,"photpars.pa");
       return(status);
     }
   }
   else if ( !strncmp(line, "photpars.zmag=",14) ) {
     pstr = line+14;
     if (status = assign_double_param( pstr, &pars->phot_zmag )) {
       fprintf(stderr,ERRMSG_MISSING_INPUT,"photpars.zmag");
       return(status);
     }
   }
   else { return(warn_unknown_param(line)); }
 }

 return(status);
}


/* Function : assign_double_param */
/* Version: 0.1.0 */
/*   Parse and assign a double precision parameter
 *
 * int assign_double_param( char *pstr, double *param )
 *
 * Usage: status = assign_double_param( pstr, &param )
 */
int assign_double_param( char *pstr, double *param )
{
 int status=0;
 char tmp[2*MAXLENGTH];

 strcpy(tmp,"");
 status = assign_str_param( pstr, tmp );
 if (strlen(tmp) != 0) { *param = atof(tmp); }
 else { return( ERRNO_MISSING_INPUT ); }
 
 return(status);
}

/* Function : assign_int_param */
/* Version: 0.1.0 */
/*   Parse and assign a integer precision parameter
 *
 * int assign_int_param( char *pstr, int *param )
 *
 * Usage: status = assign_int_param( pstr, &param )
 */
int assign_int_param( char *pstr, int *param )
{
 int status=0;
 char tmp[2*MAXLENGTH];

 strcpy(tmp,"");
 status = assign_str_param( pstr, tmp );
 if (strlen(tmp) != 0) { *param = atoi(tmp); }
 else { return( ERRNO_MISSING_INPUT ); }
 
 return(status);
}

/* Function : assign_str_param */
/* Version: 0.1.0 */
/*   Parse and assign a string parameter
 *
 * int assign_str_param( char *pstr, char *param )
 *
 * Usage: status = assign_str_param( pstr, param );
 */
int assign_str_param( char *pstr, char param[] )
{
 int l,status=0;
 char *pend;

 pend = ( strchr(pstr, '\40') < strchr(pstr,'\11') ) ? 
     				strchr(pstr,'\40')      :
				strchr(pstr,'\11')      ;
 l = pend - pstr;
 if (l > 0) { strncpy(param,pstr,l); }
 param[l] = '\0';

 return(status);
}

/* Function : warn_unknown_param */
/* Version: 0.1.0 */
/*   Warn of unknown parameter in a param file
 *
 * int warn_unknown_param( char line[] )
 *
 * Usage: status = warn_unknown_param(line);
 */

int warn_unknown_param( char line[] )
{
 int l;
 char tmp[2*MAXLENGTH];

 l = strchr( line, '=' ) - line;
 strncpy( tmp, line, l );
 tmp[l]='\0';
 fprintf(stderr,ERRMSG_UNDEFINED_INPUT,tmp);
 return(ERRNO_UNDEFINED_INPUT);
}
