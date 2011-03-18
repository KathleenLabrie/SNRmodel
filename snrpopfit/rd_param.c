/* Function Package : rd_param */
/* Version: 0.1.2 */
/*   Functions used to read parameter files associated to the 'snrpopfit' pkg
 *
 * int rd_snrpopfit_param( FILE *istream; SNRPOPFITPARS *pars )
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
#include <KLutil.h>     /* for definitions, svector() */
#include <stddef.h>     /* for NULL */
#include <string.h>
#include <stdlib.h>     /* for atof(), atoi(), strtod() */
#include <stdio.h>

/* Function : rd_snrpopfit_param */
/* Version : 0.1.1 */
/*   Read snrpopfit.param file
 *
 * int rd_snrpopfit_param( FILE *istream, SNRPOPFITPARS *pars )
 *
 * Usage: status = rd_snrpopfit_param(istream, &snrpopfitpars);
 */

int rd_snrpopfit_param( FILE *istream, SNRPOPFITPARS *pars )
{
 int status=0;
 int i;
 char *ptail;
 char line[2*MAXLENGTH], *pstr, tmp[2*MAXLENGTH], **p_parse=NULL;

 while (fgets( line, 2*MAXLENGTH, istream ) != NULL) {
   if ( !strncmp(line,"snrpopfit.input=",16) ) {
      pstr = line+16;
      status = assign_str_param( pstr, pars->snrpopfit_input );
   }
   else if ( !strncmp(line,"snrpopfit.output=",17) ) {
      pstr = line+17;
      status = assign_str_param( pstr, pars->snrpopfit_output );
   }
   else if ( !strncmp(line,"snrpopfit.noiseimg=",19) ) {
      pstr = line+19;
      status = assign_str_param( pstr, pars->snrpopfit_noiseimg );
   }
   else if ( !strncmp(line,"snrpopfit.section=",18) ) {
      pstr = line+18;
      sscanf(pstr,"[%d:%d,%d:%d]%*s",
      			&pars->snrpopfit_section[0],
      			&pars->snrpopfit_section[1],
      			&pars->snrpopfit_section[2],
      			&pars->snrpopfit_section[3]);
   }
   else if ( !strncmp(line,"snrpopfit.nsnr=",15) ) {
      pstr = line+15;
      status = assign_str_param( pstr, tmp );
      p_parse = svector(MAXDIM,MAXLENGTH);
      pars->snrpopfit_Nnsnr = splitstr(tmp, p_parse, ",");
      pars->snrpopfit_nsnr = dvector(pars->snrpopfit_Nnsnr);
      for (i=0;i<pars->snrpopfit_Nnsnr;i++) {
        pars->snrpopfit_nsnr[i] = strtod(p_parse[i], &ptail);
        if ( p_parse[i] == ptail || *ptail != '\0' ) { /* Invalid input float */
            fprintf(stderr, "ERROR: Invalid float (%s)\n", p_parse[i]);
            fprintf(stderr, ERRMSG_INPUT_ERROR, "snrpopfit_nsnr");
            status = ERRNO_INPUT_ERROR;
            return(status);
        }
      }
      free_svector(p_parse);
   }
   else if ( !strncmp(line,"snrpopfit.snrate=",17) ) {
      pstr = line+17;
      status = assign_str_param( pstr, tmp );
      p_parse = svector(MAXDIM,MAXLENGTH);
      pars->snrpopfit_Nsnrate = splitstr(tmp, p_parse, ",");
      pars->snrpopfit_snrate = dvector(pars->snrpopfit_Nsnrate);
      for (i=0;i<pars->snrpopfit_Nsnrate;i++) {
        pars->snrpopfit_snrate[i] = strtod(p_parse[i], &ptail);
        if ( p_parse[i] == ptail || *ptail != '\0' ) { /* Invalid input float */
            fprintf(stderr, "ERROR: Invalid float (%s)\n", p_parse[i]);
            fprintf(stderr, ERRMSG_INPUT_ERROR, "snrpopfit_snrate");
            status = ERRNO_INPUT_ERROR;
            return(status);
        }
      }
      free_svector(p_parse);
   }
   else if ( !strncmp(line,"snrpopfit.nambient=",19) ) {
      pstr = line+19;
      status = assign_str_param( pstr, tmp );
      p_parse = svector(MAXDIM,MAXLENGTH);
      pars->snrpopfit_Nnambient = splitstr(tmp, p_parse, ",");
      pars->snrpopfit_nambient = dvector(pars->snrpopfit_Nnambient);
      for (i=0;i<pars->snrpopfit_Nnambient;i++) {
        pars->snrpopfit_nambient[i] = strtod(p_parse[i], &ptail);
        if ( p_parse[i] == ptail || *ptail != '\0' ) { /* Invalid input float */
            fprintf(stderr, "ERROR: Invalid float (%s)\n", p_parse[i]);
            fprintf(stderr, ERRMSG_INPUT_ERROR, "snrpopfit_nambient");
            status = ERRNO_INPUT_ERROR;
            return(status);
        }
      }
      free_svector(p_parse);
   }
   else if ( !strncmp(line,"snrpopfit.lthreshold=",21) ) {
      pstr = line+21;
      if (status = assign_double_param( pstr, &pars->snrpopfit_lthreshold )) {
         fprintf(stderr,ERRMSG_MISSING_INPUT,"snrpopfit.lthreshold");
	 return(status);
      }
   }
   else if ( !strncmp(line,"snrpopfit.hthreshold=",21) ) {
      pstr = line+21;
      if (status = assign_double_param( pstr, &pars->snrpopfit_hthreshold )) {
         fprintf(stderr,ERRMSG_MISSING_INPUT,"snrpopfit.hthreshold");
	 return(status);
      }
   }
   else if ( !strncmp(line,"snrpopfit.mksnrpop=",19) ) {
      pstr = line+19;
      status = assign_str_param( pstr, pars->snrpopfit_mksnrpop );
   }
   else if ( !strncmp(line,"snrpopfit.artimg=",17) ) {
      pstr = line+17;
      status = assign_str_param( pstr, pars->snrpopfit_artimg );
   }
   else if ( !strncmp(line,"snrpopfit.addpop=",17) ) {
      pstr = line+17;
      status = assign_str_param( pstr, pars->snrpopfit_addpop );
   }
   else if ( !strncmp(line,"snrpopfit.convolve=",19) ) {
      pstr = line+19;
      status = assign_bool_param( pstr, &pars->snrpopfit_convolve );
   }
   else if ( !strncmp(line,"snrpopfit.sigma=",16) ) {
      pstr = line+16;
      if (status = assign_double_param( pstr, &pars->snrpopfit_sigma )) {
         fprintf(stderr,ERRMSG_MISSING_INPUT,"snrpopfit.sigma");
	 return(status);
      }
   }
   else if ( !strncmp(line,"snrpopfit.width=",16) ) {
      pstr = line+16;
      if (status = assign_double_param( pstr, &pars->snrpopfit_width )) {
         fprintf(stderr,ERRMSG_MISSING_INPUT,"snrpopfit.width");
	 return(status);
      }
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
