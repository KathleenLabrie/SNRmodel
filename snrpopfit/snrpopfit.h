/* snrpopfit.h */

#ifndef _SNRPOPFIT_H_
#define _SNRPOPFIT_H_

#include "./rd_param.h"

#define BINDIR "/home/klabrie/prgc/"

#define DEFAULT_DIR "/home/klabrie/prgc/bin/"
#define DEFAULT_SNRPOPFIT_PARS "snrpopfit.param"

#define MKSNRPOP "/home/klabrie/prgc/bin/mksnrpop"
#define ARTIMG "/home/klabrie/prgc/bin/artimg"

#define MAXPOP 30

#define HELP_SNRPOPFIT \
"\nUsage: snrpopfit [-vh] paramfile [-o output]\n\
  paramfile     : Name of the paramter files [default: snrpopfit.param]\n\
                  (Searches current directory then looks into \n\
                   installation directory.)\n\
  -h|--help     : Print help page\n\
  -o output     : Root of output chi2 maps\n\
  -v            : Toggle verbose on\n\
  --version     : Print version\n\
  \n\
Parameters\n\
  input         : Input image, ie. the real image.\n\
  output        : Root name of the outputs\n\
  noiseimg      : Image used to calculate Poisson noise\n\
  section       : Section to use for the fit. IRAF format\n\
  nsnr          : Number of SNRs (eg. 100,1000,10000)\n\
  snrate        : SN rate (eg. 0.001, 0.01, 0.1)\n\
  nambient      : Ambient density (eg. 1, 3, 10)\n\
  lthreshold    : Low flux cutoff\n\
  hthreshold    : High flux cutoff\n\
  mksnrpop      : Name of the paramfile for mksnrpop\n\
  artimg        : Name of the paramfile for artimg\n\
  addpop	: Name of additional populations of objects\n\
  convolve      : Convolve image before comparing? (yes/no)\n\
  sigma         : If convolve is yes, sigma of gaussian filter\n\
  width         : If convolve is yes, width,in sigma, of the kernel\n"

int aperture_phot(double **ppix, long int naxes[], 
	double *fluxtot, double *fluxerr, double *stddev, double *stddeverr);


#endif /* _SNRPOPFIT_H_ */
