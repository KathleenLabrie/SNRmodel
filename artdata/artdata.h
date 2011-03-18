/* artdata.h */

#include <KLcfitsio.h>		/* for KLFITS_HEADER */

#ifndef _ARTDATA_H_
#define _ARTDATA_H_

#define BINDIR "/home/klabrie/prgc/" 
/* #define SIMBINDIST "stats/simbindist" */

#define DEFAULT_DIR "/home/klabrie/prgc/bin/"
#define DEFAULT_MKSNRPOP_PARS "mksnrpop.param"
#define DEFAULT_ARTIMG_PARS "artimg.param"

#define MAXPOP 10

#define PI 3.1415926535
#define LSOLAR 3.9e26           /* J/s */
#define MPERPC 3.086e16         /* meters per parsec */
#define SECPERRAD 206265.       /* arcseconds per radian */

#define MKSNRPOP_OUTPUT_NCOL 4

#include "./rd_param.h"

#define HELP_ARTIMG \
"\nUsage: mksnrpop [-vh] paramfile [-o output]\n\
  paramfile     : Name of the parameter file [default: mksnrpop.param]\n\
                  (Searches current directory first then looks into\n\
                   installation directory.)\n\
  -h|--help     : Print help page\n\
  -o output     : Output file.  Override parameter file.\n\
  -v            : Toggle verbose on\n\
  --version     : Print version\n\
 \n\
Parameters\n\
  input         : Input image. If none, creates a new image.\n\
  inputpop      : Input population files.\n\
  output        : Name of the output image\n\
  distance      : Distance to the galaxy or cluster\n\
  pixscale      : Pixel scale of the instrument\n\
  naxis         : Size of the image\n\
  x0,y0         : Center of the distribution in pixel coordinates\n\
                  [If INDEF, defaults to center of the image.]\n\
  gain          : Gain of the instrument [ADU/e-]\n\
  rdnoise       : Readnoise [e-]\n\
  fluxcalib     : Absolute flux calibration [W/m2/ADU]\n\
  seeing        : Seeing [arcsec]\n\
  psf           : PSF profile [Default: moffat]\n\
  skybkg        : Sky background flux [ADU/s]\n\
  rmvbkg	: Remove background, ie use bkg for noise only\n\
  noiseimg      : Image to use to calculate pre continuum subtraction\n\
                  noise.\n\
  xnoise        : Add extra gaussian noise [ADU]\n\
  exptime       : Exposure time [seconds]\n\
  nexp          : Number of exposures to combine\n"


#define HELP_MKSNRPOP \
"\nUsage: mksnrpop [-vh] paramfile [-o output]\n\
  paramfile    : Name of the parameter file [default: mksnrpop.param]\n\
                 (Searches current directory first then looks into\n\
                  installation directory.)\n\
  -h|--help    : Print help page\n\
  -o output    : Output file.  Override parameter file.\n\
  -v           : Toggle verbose on\n\
  --version    : Print version\n\
  \n\
Parameters\n\
  output       : Name of the output file\n\
  nsnr         : Number of SNR to generate\n\
  snrate       : SN rate [1/yr]\n\
  nambient     : Ambient number density\n\
  scalerad     : Scale radius of the galactocentric distribution\n\
  ellip        : Ellipticity of the galactocentric distribution\n\
  pa           : Position angle of the galactocentric distribution\n\
  moddiam      : Diameter distribution model  [parsecs]\n\
                   - adiabatic : N(<D) for adiabatic phase\n\
                   - radiative : N(<D) for radiative phase\n\
                   - <user model>\n\
                     format: histogram -> diameter_bin number\n\
  modpos       : Galactocentric distribution model [parsecs]\n\
                   - gaussian\n\
                   - <user model>\n\
                     format: histogram -> distance_bin number pa_bin number\n\
  modlum       : Luminosity distribution model [Lsolar]\n\
                   - morel\n"

/* Typedef */
typedef struct
{
 double asnr_gcd, asnr_gcl, asnr_diameter;
 double asnr_sb;
} ASNR;

typedef struct
{
 long gcpos_seedgcd, gcpos_seedgcl;
 double gcpos_scalerad, gcpos_ellip, gcpos_pa;
 char gcpos_model[MAXLENGTH];
} GCPOSPARS;

typedef struct
{			/* need to add position dependent density */
 long snr_seeddiam,snr_seedlum, snr_nsnr;
 double snr_n0, snr_rate, snr_life;
 char snr_moddiam[MAXLENGTH],snr_modlum[MAXLENGTH];
} SNRPARS;

/* artdata.c */
ASNR *asnrvector(long int nh);
void free_asnrvector(ASNR *v);

/* Functions */
int artimg_bkg(ARTIMGPARS pars, double **ppix);
int artimg_integrate(ARTIMGPARS pars, double **ppix);
int artimg_pnoise(ARTIMGPARS pars, double **ppix, double **pnoiseimg);
int artimg_rdnoise(ARTIMGPARS pars, double **ppix);
int artimg_seeing(ARTIMGPARS pars, double **ppix);
int artimg_snr(FILE *fstream, ARTIMGPARS pars, double **ppix, 
                KLFITS_HEADER *headers, int *nheaders);
int artimg_xnoise(ARTIMGPARS pars, double **ppix);
int gcpos(GCPOSPARS *pars, double *gcd, double *gcl);
int parse_popheader(char line[], KLFITS_HEADER *h);
int snrdiam(SNRPARS *pars, ASNR *asnr);
int snrlum(SNRPARS *pars, ASNR *asnr);

#endif /* _ARTDATA_H_ */
