/* Function Package : rd_param */
/* Version: 0.1.1 */
/*   Functions used to read parameter files associated to the 'artdata' pkg
 *
 * int rd_artimg_param( FILE *istream; ARGIMG *pars )
 * int rd_mksnrpop_param( FILE *istream, MKSNRPOPPARS *pars )
 * int assign_bool_param( char *pstr, int *param )
 * int assign_int_param( char *pstr, int *param )
 * int assign_double_param( char *pstr, double *param )
 * int assign_str_param( char *pstr, char param[] )
 * int warn_unknown_param( char line[] )
 *
 * Needs:
 *   %%%libKLutil%%%
 */

#include "./rd_param.h"
#include <KLutil.h>	/* for definitions, svector() */
#include <stddef.h>	/* for NULL */
#include <string.h>
#include <stdlib.h>	/* for atof(), atoi() */
#include <stdio.h>

/* Function : rd_artimg_param */
/* Version : 0.1.0 */
/*   Read artimg.param file
 *
 * int rd_artimg_param( FILE *istream, ARTIMGPARS *pars )
 *
 * Usage: status = rd_artimg_param(istream, &artimgppars);
 */

int rd_artimg_param( FILE *istream, ARTIMGPARS *pars )
{
 int status=0;
 int i;
 char line[2*MAXLENGTH], *pstr, tmp[2*MAXLENGTH],**p_parse=NULL;

 while (fgets( line, 2*MAXLENGTH, istream ) != NULL) {
   if ( !strncmp(line,"artimg.input=",13) ) {
     pstr = line+13;
     status = assign_str_param( pstr, pars->artimg_input );
   }
   else if ( !strncmp(line,"artimg.inputpop=",16) ) {
     pstr = line+16;
     status = assign_str_param( pstr, pars->artimg_inputpop );
   }
   else if ( !strncmp(line,"artimg.output=",14) ) {
     pstr = line+14;
     status = assign_str_param( pstr, pars->artimg_output );
   }
   else if ( !strncmp(line,"artimg.distance=",16) ) {
     pstr = line+16;
     if (status = assign_double_param( pstr, &pars->artimg_distance )) {
       fprintf(stderr,ERRMSG_MISSING_INPUT,"artimg.distance");
       return(status);
     }
   }
   else if ( !strncmp(line,"artimg.pixscale=",16) ) {
     pstr = line+16;
     if (status = assign_double_param( pstr, &pars->artimg_pixscale )) {
       fprintf(stderr,ERRMSG_MISSING_INPUT,"artimg.pixscale");
       return(status);
     }
   }
   else if ( !strncmp(line,"artimg.naxes=",13) ) {
     pstr = line+13;
     status = assign_str_param( pstr, tmp );
     p_parse = svector(2,MAXLENGTH);
     splitstr(tmp,p_parse,",");
     pars->artimg_naxes[0] = atoi(p_parse[0]);
     pars->artimg_naxes[1] = atoi(p_parse[1]);
     free_svector(p_parse);
   }
   else if ( !strncmp(line,"artimg.x0=",10) ) {
     pstr = line+10;
     if (status = assign_double_param( pstr, &pars->artimg_x0 )) {
       fprintf(stderr,ERRMSG_MISSING_INPUT,"artimg.x0");
       return(status);
     }
   }
   else if ( !strncmp(line,"artimg.y0=",10) ) {
     pstr = line+10;
     if (status = assign_double_param( pstr, &pars->artimg_y0 )) {
       fprintf(stderr,ERRMSG_MISSING_INPUT,"artimg.y0");
       return(status);
     }
   }
   else if ( !strncmp(line,"artimg.gain=",12) ) {
     pstr = line+12;
     if (status = assign_double_param( pstr, &pars->artimg_gain )) {
       fprintf(stderr,ERRMSG_MISSING_INPUT,"artimg.gain");
       return(status);
     }
   }
   else if ( !strncmp(line,"artimg.rdnoise=",15) ) {
     pstr = line+15;
     if (status = assign_double_param( pstr, &pars->artimg_rdnoise )) {
       fprintf(stderr,ERRMSG_MISSING_INPUT,"artimg.rdnoise");
       return(status);
     }
   }
   else if ( !strncmp(line,"artimg.fluxcalib=",17) ) {
     pstr = line+17;
     if (status = assign_double_param( pstr, &pars->artimg_fluxcalib )) {
       fprintf(stderr,ERRMSG_MISSING_INPUT,"artimg.fluxcalib");
       return(status);
     }
   }
   else if ( !strncmp(line,"artimg.seeing=",14) ) {
     pstr = line+14;
     if (status = assign_double_param( pstr, &pars->artimg_seeing )) {
       fprintf(stderr,ERRMSG_MISSING_INPUT,"artimg.seeing");
       return(status);
     }
   }
   else if ( !strncmp(line,"artimg.psf=",11) ) {
     pstr = line+11;
     status = assign_str_param( pstr, pars->artimg_psf );
   }
   else if ( !strncmp(line,"artimg.psfwidth=",16) ) {
     pstr = line+16;
     if (status = assign_double_param( pstr, &pars->artimg_psfwidth )) {
       fprintf(stderr,ERRMSG_MISSING_INPUT,"artimg.psfwidth");
       return(status);
     }
   }
   else if ( !strncmp(line,"artimg.skybkg=",14) ) {
     pstr = line+14;
     if (status = assign_double_param( pstr, &pars->artimg_skybkg )) {
       fprintf(stderr,ERRMSG_MISSING_INPUT,"artimg.skybkg");
       return(status);
     }
   }
   else if ( !strncmp(line,"artimg.rmvbkg=",14) ) {
     pstr = line+14;
     if (status = assign_bool_param( pstr, &pars->artimg_rmvbkg )) {
       fprintf(stderr,ERRMSG_MISSING_INPUT,"artimg.rmvbkg");
       return(status);
     }
   }
   else if ( !strncmp(line,"artimg.noiseimg=",16) ) {
     pstr = line+16;
     status = assign_str_param( pstr, pars->artimg_noiseimg );
   }
   else if ( !strncmp(line,"artimg.xnoise=",14) ) {
     pstr = line+14;
     if (status = assign_double_param( pstr, &pars->artimg_xnoise )) {
       fprintf(stderr,ERRMSG_MISSING_INPUT,"artimg.xnoise");
       return(status);
     }
   }
   else if ( !strncmp(line,"artimg.exptime=",15) ) {
     pstr = line+15;
     if (status = assign_double_param( pstr, &pars->artimg_exptime )) {
       fprintf(stderr,ERRMSG_MISSING_INPUT,"artimg.exptime");
       return(status);
     }
   }
   else if ( !strncmp(line,"artimg.nexp=",12) ) {
     pstr = line+12;
     if (status = assign_int_param( pstr, &pars->artimg_nexp )) {
       fprintf(stderr,ERRMSG_MISSING_INPUT,"artimg.nexp");
       return(status);
     }
   }
   else {
     return(warn_unknown_param(line));
   }
 }
 return(status);
}


/* Function : rd_mksnrpop_param */
/* Version : 0.1.0 */
/*   Read mksnrpop.param file
 *
 * int rd_mksnrpop_param( FILE *istream, MKSNRPOPPARS *pars )
 *
 * Usage: status = rd_mksnrpop_param(istream, &mksnrpoppars);
 */

int rd_mksnrpop_param( FILE *istream, MKSNRPOPPARS *pars )
{
 int status=0;
 int i;
 char line[2*MAXLENGTH], *pstr, tmp[2*MAXLENGTH];

 while (fgets( line, 2*MAXLENGTH, istream ) != NULL) {
   if ( !strncmp(line,"mksnrpop.output=",16) ) {
     pstr = line+16;
     status = assign_str_param( pstr, pars->mksnrpop_output );
   }
   else if ( !strncmp(line,"mksnrpop.nsnr=",14) ) {
     pstr = line+14;
     if (status = assign_long_param( pstr, &pars->mksnrpop_nsnr )) {
       fprintf(stderr,ERRMSG_MISSING_INPUT,"mksnrpop.nsnr");
       return(status);
     }
   }
   else if ( !strncmp(line,"mksnrpop.snrate=",16) ) {
     pstr = line+16;
     if (status = assign_double_param( pstr, &pars->mksnrpop_snrate )) {
       fprintf(stderr,ERRMSG_MISSING_INPUT,"mksnrpop.snrate");
       return(status);
     }
   }
   else if ( !strncmp(line,"mksnrpop.nambient=",18) ) {
     pstr = line+18;
     if (status = assign_double_param( pstr, &pars->mksnrpop_nambient )) {
       fprintf(stderr,ERRMSG_MISSING_INPUT,"mksnrpop.nambient");
       return(status);
     }
   }
   else if ( !strncmp(line,"mksnrpop.life=",14) ) {
     pstr = line+14;
     if (status = assign_double_param( pstr, &pars->mksnrpop_life )) {
       fprintf(stderr,ERRMSG_MISSING_INPUT,"mksnrpop.life");
       return(status);
     }
   }
   else if ( !strncmp(line,"mksnrpop.scalerad=",18) ) {
     pstr = line+18;
     if (status = assign_double_param( pstr, &pars->mksnrpop_scalerad )) {
       fprintf(stderr,ERRMSG_MISSING_INPUT,"mksnrpop.scalerad");
       return(status);
     }
   }
   else if ( !strncmp(line,"mksnrpop.ellip=",15) ) {
     pstr = line+15;
     if (status = assign_double_param( pstr, &pars->mksnrpop_ellip )) {
       fprintf(stderr,ERRMSG_MISSING_INPUT,"mksnrpop.ellip");
       return(status);
     }
   }
   else if ( !strncmp(line,"mksnrpop.pa=",12) ) {
     pstr = line+12;
     if (status = assign_double_param( pstr, &pars->mksnrpop_pa )) {
       fprintf(stderr,ERRMSG_MISSING_INPUT,"mksnrpop.pa");
       return(status);
     }
   }
   else if ( !strncmp(line,"mksnrpop.moddiam=",17) ) {
     pstr = line+17;
     status = assign_str_param( pstr, pars->mksnrpop_moddiam );
   }
   else if ( !strncmp(line,"mksnrpop.modpos=",16) ) {
     pstr = line+16;
     status = assign_str_param( pstr, pars->mksnrpop_modpos );
   }
   else if ( !strncmp(line,"mksnrpop.modlum=",16) ) {
     pstr = line+16;
     status = assign_str_param( pstr, pars->mksnrpop_modlum );
   }
   else {
     return(warn_unknown_param(line));
   }
 }
 
 return(status);
}

/* Function : assign_bool_param */
/* Version : 0.1.0 */
/*   Parse and assign a boolean parameter
 *
 * int assign_bool_param( char *pstr, int *param )
 *
 * Usage: status = assign_bool_param( pstr, &param )
 */
int assign_bool_param( char *pstr, int *param )
{
 int status=0;
 char tmp[2*MAXLENGTH];
 
 strcpy(tmp,"");
 status = assign_str_param( pstr, tmp );
 if (strlen(tmp) != 0) { 
   if (!strncmp(tmp,"yes",3)) { *param=1; }

   else if (!strncmp(tmp,"no",2)) { *param=0; }
   else { 
     fprintf(stderr,ERRMSG_INPUT_ERROR,tmp);
     return( ERRNO_INPUT_ERROR );
   }
 }
 else { return( ERRNO_MISSING_INPUT ); }
 
 return(status);
}


/* Function : assign_int_param */
/* Version : 0.1.0 */
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

/* Function : assign_long_param */
/* Version : 0.1.0 */
/*   Parse and assign a integer precision parameter
 *
 * int assign_long_param( char *pstr, long int *param )
 *
 * Usage: status = assign_long_param( pstr, &param )
 */
int assign_long_param( char *pstr, long int *param )
{
 int status=0;
 char tmp[2*MAXLENGTH];
 
 strcpy(tmp,"");
 status = assign_str_param( pstr, tmp );
 if (strlen(tmp) != 0) { *param = atoi(tmp); }
 else { return( ERRNO_MISSING_INPUT ); }
 
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

 if ( strchr(pstr, '\11') != NULL ) {
    pend = ( strchr(pstr, '\40') < strchr(pstr,'\11') ) ? 
                                strchr(pstr,'\40')      :
                                strchr(pstr,'\11')      ;
 } else {
    pend = strchr(pstr,'\40');
 }

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
