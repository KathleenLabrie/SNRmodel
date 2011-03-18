/* rd_param.h */

#include <mydef.h>	/* for MAXLENGTH */
#include <stdio.h>	/* for FILE */
#include <stddef.h>	/* for NULL */

#ifndef _SNRPOPFIT_RD_PARAM_H_
#define _SNRPOPFIT_RD_PARAM_H_

#define MAXDIM 10

typedef struct
{
 unsigned long int snrpopfit_section[4];
 int snrpopfit_Nnsnr, snrpopfit_Nsnrate, snrpopfit_Nnambient;
 int snrpopfit_convolve;
 double *snrpopfit_nsnr,*snrpopfit_snrate,*snrpopfit_nambient;
 double snrpopfit_lthreshold,snrpopfit_hthreshold;
 double snrpopfit_sigma,snrpopfit_width;
 char snrpopfit_input[MAXLENGTH], snrpopfit_output[MAXLENGTH];
 char snrpopfit_noiseimg[MAXLENGTH];
 char snrpopfit_mksnrpop[MAXLENGTH],snrpopfit_artimg[MAXLENGTH];
 char snrpopfit_addpop[MAXLENGTH];
} SNRPOPFITPARS;

int assign_bool_param( char *pstr, int *param );
int assign_int_param( char *pstr, int *param );
int assign_long_param( char *pstr, long int *param );
int assign_double_param( char *pstr, double *param );
int assign_str_param( char *pstr, char param[] );
int warn_unknown_param( char line[] );

int rd_snrpopfit_param( FILE *istream, SNRPOPFITPARS *pars );

#endif /* _SNRPOPFIT_RD_PARAM_H_ */
