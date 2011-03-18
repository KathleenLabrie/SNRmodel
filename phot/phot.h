/* phot.h */

#include <KLutil.h>
#include <stdio.h>

#ifndef _PHOT_H_
#define _PHOT_H_

#define MAX_N_IMAGES 100
#define MAX_N_RADII 20
#define MAX_N_COORDS 1000
#define FITSKY_TOLERANCE 1e-3

#define DEFAULT_DIR "/home/klabrie/prgc/bin/"
#define DEFAULT_APPHOT_PARS "apphot.param"
#define DEFAULT_DATA_PARS "data.param"
#define DEFAULT_FITSKY_PARS "fitsky.param"
#define DEFAULT_PHOT_PARS "phot.param"
#define DEFAULT_OUTPUT_SUFFIX "mag"

#define DEFAULT_FILTER_DIR "filters/"

typedef struct
{
 int apphot_n;
 char **apphot_images, apphot_imglist[MAXLENGTH];
 char apphot_coords[MAXLENGTH], apphot_output[MAXLENGTH];
 char apphot_datapars[MAXLENGTH],apphot_fitskypars[MAXLENGTH];
 char apphot_photpars[MAXLENGTH];
} APPHOTPARS;

typedef struct
{
 double data_scale, data_itime, data_gain;
 double data_datamin,data_datamax;
 char data_hexposure[MAXLENGTH], data_hairmass[MAXLENGTH]; 
} DATAPARS;

typedef struct
{
 int fitsky_maxiter,fitsky_maxreject,fitsky_nkeep;
 double fitsky_annulus,fitsky_wannulus,fitsky_skyvalue;
 double fitsky_loreject,fitsky_hireject;
 char fitsky_algorithm[MAXLENGTH];
} FITSKYPARS;

typedef struct
{
 int phot_n;
 double *phot_radii;
 double phot_ellip, phot_pa;
 double phot_zmag;
 char phot_ashape[10];
} PHOTPARS;

typedef struct
{
 int *res_serr;
 int res_ncoo,res_nradii,**res_npix,*res_nskypix;
 double *res_skyperpix,*res_skystddev;
 double **res_totalflux, **res_error, *res_radii;
 double **res_apstddev;
 double **res_mag,**res_merr,**res_fluxsec;
} RESULTS;

/* phot */
void alloc_results( RESULTS *strct );
void free_results( RESULTS *strct );

/* rd_param */
int phot_assign_double_param( char *pstr, double *param );
int phot_assign_int_param( char *pstr, int *param );
int phot_assign_str_param( char *pstr, char param[] );
int rd_apphot_param( FILE *istream, APPHOTPARS *pars );
int rd_data_param( FILE *istream, DATAPARS *pars );
int rd_fitsky_param ( FILE *istream, FITSKYPARS *pars );
int rd_phot_param ( FILE *istream, PHOTPARS *pars );
int phot_warn_unknown_param( char line[] );


int aptotflux(double **ppix, long int naxes[], double **pcoo, int idcoo,
		PHOTPARS ppars, DATAPARS dpars, RESULTS *res, int FLAGS );
int fitsky(double **ppix, long int naxes[], double **pcoo, int idcoo, 
		FITSKYPARS fspars, PHOTPARS ppars, DATAPARS dpars,
		RESULTS *res, int FLAGS );
int mag(double **pcoo, int idcoo, PHOTPARS ppars, DATAPARS dpars, RESULTS *res);
int print_result(char imgname[], char outfile[], double **pcoo, 
		  DATAPARS datapars, FITSKYPARS fitskypars, PHOTPARS photpars, 
		  RESULTS results);
int rd_coords( FILE *istream, double ***ptr_pcoords, int *ncoords );
int skyvalue(double *inputvec, int npix, FITSKYPARS pars,
		double *sky, double *stddev, int *npix1, int FLAGS);



#endif /* _PHOT_H_ */
