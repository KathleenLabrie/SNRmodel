/* redcor.h */

#ifndef _REDCOR_H_
#define _REDCOR_H_

#define CFHT_REDEYEW_J_ID "cfh5103"
#define CFHT_REDEYEW_H_ID "cfh5201"

#define CFHT_REDEYEW_J_FILE "cfh5103c_approx.dat"
#define CFHT_REDEYEW_H_FILE "cfh5201c_approx.dat"

#define CFH5103 0
#define CFH5201 1

#define RV_STARBURST 4.05
#define RV_STARBURST_ERR 0.80	/* not used, set kl_err to 20% instead,*/
				/* based on results in section 4.2 Calzetti */
#define RV_STD 3.1
#define FRG_ERR 0.05	/* 5% from spread in A(l)/A{V), Table 3 Cardelli etal */
#define CE2CESTELLAR 0.44
#define CE2CESTELLAR_ERR 0.03

#define STARBURST 0
#define NEBULAR 1

#define HELP_FILTERS \
"\nDescription of filters\n\
  cfh5103\tJ-band\tRedeye Wide\tcool_approx\tcfh5103c_approx.dat\n\
  cfh5201\tH-band\tRedeye Wide\tcool_approx\tcfh5201c_approx.dat\n"

#define HELP_REDCOR \
"\n\
Usage: redcor [-vh] 'line' lambda flux flux_err CEf CEf_err CEi CEi_err\n\
              [--starburst]\n\
       redcor [-vh] 'cont' filterID mag mag_err CEf CEf_err CEi CEi_err\n\
              [--starburst]\n\
       redcor --filters\n\
 'line'/'cont'   : mode to adopt\n\
 lambda          : wavelength of the line (Angstrom)\n\
 flux            : observed flux\n\
 filterID        : code name of the filter transmission to use\n\
 mag             : observed magnitude\n\
 CEf             : E(B-V) foreground\n\
 CEi             : E(B-V) intrinsic\n\
 --starburst     : toggle on the use of the starburst reddening curve\n\
                   for intrinsic extinction\n\
 --filters       : print the filter description\n\
 -h              : print this help\n\
 -v              : toggle verbose on\n"

int filterID(char name[], unsigned short int *code);
int get_transmission(unsigned short int filter, double **ptr2lambda,
	double **ptr2trans, long int *n);
int int_correction(double lambda, double flux, double flux_err, double CEi,
 	double CEi_err, double *flux_cor, double *flux_cor_err, int OPTIONS);
int frg_correction(double lambda, double flux, double flux_err, double CEf,
	double CEf_err, double *flux_cor, double *flux_cor_err, int OPTIONS);

#endif /* _REDCOR_H_ */
