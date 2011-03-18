/* rd_param.h */

#include <mydef.h>      /* for MAXLENGTH */
#include <stdio.h>      /* for FILE */

#ifndef _ARTDATA_RD_PARAM_H_
#define _ARTDATA_RD_PARAM_H_

typedef struct
{
 int artimg_rmvbkg;
 int artimg_nexp;
 unsigned long int artimg_naxes[2];
 double artimg_distance,artimg_x0,artimg_y0,artimg_fluxcalib;
 double artimg_seeing,artimg_psfwidth,artimg_skybkg,artimg_exptime;
 double artimg_pixscale,artimg_gain,artimg_rdnoise,artimg_xnoise;
 char artimg_psf[MAXLENGTH];
 char artimg_input[MAXLENGTH],artimg_output[MAXLENGTH];
 char artimg_noiseimg[MAXLENGTH], artimg_inputpop[MAXLENGTH];
} ARTIMGPARS;

typedef struct
{
 long mksnrpop_nsnr;
 double mksnrpop_snrate,mksnrpop_nambient;
 double mksnrpop_scalerad,mksnrpop_ellip,mksnrpop_pa;
 double mksnrpop_life;
 char mksnrpop_moddiam[MAXLENGTH],mksnrpop_modpos[MAXLENGTH];
 char mksnrpop_modlum[MAXLENGTH];
 char mksnrpop_output[MAXLENGTH];
} MKSNRPOPPARS;

int assign_bool_param( char *pstr, int *param );
int assign_int_param( char *pstr, int *param );
int assign_long_param( char *pstr, long int *param );
int assign_double_param( char *pstr, double *param );
int assign_str_param( char *pstr, char param[] );
int warn_unknown_param( char line[] );

int rd_artimg_param( FILE *istream, ARTIMGPARS *pars );
int rd_mksnrpop_param( FILE *istream, MKSNRPOPPARS *pars );

#endif /* _ARTDATA_RD_PARAM_H_ */
