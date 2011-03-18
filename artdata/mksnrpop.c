/* Main : mksnrpop */
/* Version : 0.1.1 */
/*   Create a population of SNR.  Outputs are in intrinsic physical
 *   units.
 *
 * Usage: mksnrpop [-vh] paramfile [-o output]
 *
 *	paramfile	: Name of the parameter file [default: mksnrpop.param]
 *			  (Searches current directory first then looks into
 *			   installation directory.)
 *	-h|--help	: Print help page
 *	-o output	: Output file.  Override parameter file.
 *	-v		: Toggle verbose on
 *	--version	: Print version
 *
 * Parameters
 *	output		: Name of the output file
 *	nsnr		: Number of SNR to generate
 *	snrate		: SN rate [1/yr]
 *	nambient	: Ambient number density
 *  life        : Life time of the SNRs (0=unlimitted)
 *	scalerad	: Scale radius of the galactocentric distribution
 *	ellip		: Ellipticity of the galactocentric distribution
 *	pa		: Position angle of the galactocentric distribution
 *	moddiam		: Diameter distribution model  [parsecs]
 *			  	- adiabatic : N(<D) for adiabatic phase
 *				- radiative : N(<D) for radiative phase
 *				- <user model>
 *				  format: histogram -> diameter_bin number
 * 	modpos		: Galactocentric distribution model [parsecs]
 *				- gaussian
 *				- <user model>
 *				  format: histogram -> distance_bin number \
 *							      pa_bin number
 *	modlum		: Luminosity distribution model [Lsolar]
 *				- morel
 *
 * Needs:
 *   %%%libKLutil%%%
 *   %%%libKLran%%%
 */

#include "./artdata.h"
#include "./mksnrpop.h"
#include <KLutil.h>
#include <KLran.h>	/* for ran2() */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>	/* for NULL */
#include <time.h>	/* to get the seed */

#define VERSION "0.1.0"

void pre_exit(void);
int print_header_mksnrpop(FILE *fstream, MKSNRPOPPARS *pars);
static int init_inputs(MKSNRPOPPARS *inputs);
static int rd_inputs(MKSNRPOPPARS *pars, MKSNRPOPPARS inputs);

/* File Scope variables */
ASNR *snrpop=NULL;

main( argc, argv )
int argc;
char *argv[];
{
 int i, status=0;
 int FLAGS;
 long idnum, idnum_diam, idnum_lum;
 double gcd,gcl;
 char paramfile[MAXLENGTH],the_default[MAXLENGTH];
 char **p_parse=NULL;
 FILE *fstream=NULL;
 MKSNRPOPPARS pars,inputs;
 GCPOSPARS gcpospars;
 SNRPARS snrpars;
 ASNR *asnr;
 time_t now;
 
 /* Initialize */
 FLAGS=0;
 strcpy(paramfile,"");
 if (status = init_inputs(&inputs)) {
 	pre_exit();
	exit(status);
 } 
 
 /* Read command line */
 i=1;
 while (i < argc) {
   if (!strncmp("--",argv[i],2)) {
     if (!strncmp("--help",argv[i],6)) {
     	fprintf(stdout,HELP_MKSNRPOP);
     	exit(status);
     }
     else if (!strncmp("--version",argv[i],9)) {
     	fprintf(stdout,"mksnrpop v%s\n",VERSION);
     	exit(status);
     }
     else if (!strncmp("--output",argv[i],8)) {
        p_parse = svector(2,MAXLENGTH);
	splitstr(argv[i],p_parse,"=");
	strcpy(inputs.mksnrpop_output,p_parse[1]);
	free_svector(p_parse);
     }
     else if (!strncmp("--nsnr",argv[i],6)) {
        p_parse = svector(2,MAXLENGTH);
	splitstr(argv[i],p_parse,"=");
	inputs.mksnrpop_nsnr = atoi(p_parse[1]);
	free_svector(p_parse);
     }
     else if (!strncmp("--snrate",argv[i],8)) {
        p_parse = svector(2,MAXLENGTH);
	splitstr(argv[i],p_parse,"=");
	inputs.mksnrpop_snrate = atof(p_parse[1]);
	free_svector(p_parse);
     }
     else if (!strncmp("--nambient",argv[i],10)) {
        p_parse = svector(2,MAXLENGTH);
	splitstr(argv[i],p_parse,"=");
	inputs.mksnrpop_nambient = atof(p_parse[1]);
	free_svector(p_parse);
     }
     else if (!strncmp("--life",argv[i],6)) {
        p_parse = svector(2,MAXLENGTH);
	splitstr(argv[i],p_parse,"=");
	inputs.mksnrpop_life = atof(p_parse[1]);
	free_svector(p_parse);
     }
     else if (!strncmp("--scalerad",argv[i],10)) {
        p_parse = svector(2,MAXLENGTH);
	splitstr(argv[i],p_parse,"=");
	inputs.mksnrpop_scalerad = atof(p_parse[1]);
	free_svector(p_parse);
     }
     else if (!strncmp("--ellip",argv[i],7)) {
        p_parse = svector(2,MAXLENGTH);
	splitstr(argv[i],p_parse,"=");
	inputs.mksnrpop_ellip = atof(p_parse[1]);
	free_svector(p_parse);
     }
     else if (!strncmp("--pa",argv[i],4)) {
        p_parse = svector(2,MAXLENGTH);
	splitstr(argv[i],p_parse,"=");
	inputs.mksnrpop_pa = atof(p_parse[1]);
	free_svector(p_parse);
     }
     else if (!strncmp("--moddiam",argv[i],9)) {
        p_parse = svector(2,MAXLENGTH);
	splitstr(argv[i],p_parse,"=");
	strcpy(inputs.mksnrpop_moddiam,p_parse[1]);
	free_svector(p_parse);
     }
     else if (!strncmp("--modpos",argv[i],8)) {
        p_parse = svector(2,MAXLENGTH);
	splitstr(argv[i],p_parse,"=");
	strcpy(inputs.mksnrpop_modpos,p_parse[1]);
	free_svector(p_parse);
     }
     else if (!strncmp("--modlum",argv[i],8)) {
        p_parse = svector(2,MAXLENGTH);
	splitstr(argv[i],p_parse,"=");
	strcpy(inputs.mksnrpop_modlum,p_parse[1]);
	free_svector(p_parse);
     }
     else {
     	fprintf(stderr, ERRMSG_INPUT_ERROR, argv[i]);
     	fprintf(stderr,HELP_MKSNRPOP);
     	exit(ERRNO_INPUT_ERROR);
     }
   }
   else if (!strncmp("-",argv[i],1)) {
     if (!strncmp("-h",argv[i],2)) { 
     	fprintf(stdout,HELP_MKSNRPOP);
	exit(status);
     }
     else if (!strncmp("-o",argv[i],2)) { 
     	strcpy(inputs.mksnrpop_output,argv[++i]);
     }
     else if (!strncmp("-v",argv[i],2)) { FLAGS |= 1 << VERBOSE; }
     else {
     	fprintf(stderr,ERRMSG_INPUT_ERROR,argv[i]);
	fprintf(stderr,HELP_MKSNRPOP);
	exit(ERRNO_INPUT_ERROR);
     }
   }
   else {
     strcpy(paramfile,argv[i]);
   }
   i++;
 }

 /* Find and open parameter file */
 if ( strlen(paramfile) == 0 ) { strcpy(paramfile,DEFAULT_MKSNRPOP_PARS); }
 if ( (fstream = fopen(paramfile,"r")) == NULL ) { /* look in current dir or as is */
   sprintf(the_default,"%s%s",DEFAULT_DIR,paramfile);
   if ( (fstream = fopen(the_default,"r")) == NULL ) {
   	fprintf(stderr,ERRMSG_FILE_NOT_FOUND,paramfile);
	exit(ERRNO_FILE_NOT_FOUND);
   }
   if (FLAGS & 1 << VERBOSE) { printf("WARNING: Using '%s'\n",the_default); }
 }
 /* Read parameter file */
 if ( status = rd_mksnrpop_param(fstream, &pars) ) {
 	fclose(fstream);
	exit(status);
 }
 fclose(fstream);
 fstream=NULL;

 /* Override pars with inputs */
 if ( status = rd_inputs(&pars,inputs) ) {
 	pre_exit();
 	exit(status);
 }

 /* Assign snrpop array */
 snrpop = asnrvector(pars.mksnrpop_nsnr);

 /* Assign gcpospars */
 gcpospars.gcpos_scalerad = pars.mksnrpop_scalerad;
 gcpospars.gcpos_ellip = pars.mksnrpop_ellip;
 gcpospars.gcpos_pa = pars.mksnrpop_pa;
 strcpy(gcpospars.gcpos_model, pars.mksnrpop_modpos);

 /* Assign snrpars */
 snrpars.snr_nsnr = pars.mksnrpop_nsnr;
 snrpars.snr_n0 = pars.mksnrpop_nambient;
 snrpars.snr_rate = pars.mksnrpop_snrate;
 snrpars.snr_life = pars.mksnrpop_life;
 strcpy(snrpars.snr_moddiam, pars.mksnrpop_moddiam);
 strcpy(snrpars.snr_modlum, pars.mksnrpop_modlum);

 /* Get four seeds */
 idnum = -(long)time(&now);
 snrpars.snr_seeddiam = -(long)(1e6*ran2(&idnum));
 gcpospars.gcpos_seedgcd = -(long)(1e6*ran2(&idnum));
 gcpospars.gcpos_seedgcl = -(long)(1e6*ran2(&idnum));
 snrpars.snr_seedlum = idnum_lum = -(long)(1e6*ran2(&idnum));
 
 /* Generate SNR population */
 asnr = snrpop;
 for (i=0;i<pars.mksnrpop_nsnr;i++,asnr++) {
   /* Generate galactocentric position for SNR i */
   if ( status = gcpos(&gcpospars, &gcd, &gcl) ) {
   	pre_exit();
	exit(status);
   }
   asnr->asnr_gcd = gcd;
   asnr->asnr_gcl = gcl;
   
   /* Generate diameter for SNR i */
   if ( status = snrdiam(&snrpars,asnr) ) {
   	pre_exit();
	exit(status);
   }
   
   /* Generate a surface brightness for SNR i */
   if ( status = snrlum(&snrpars,asnr) ) {
   	pre_exit();
	exit(status);
   }   
 }

 /* Output population */
 if (strlen(pars.mksnrpop_output) == 0) { fstream = stdout; }
 else { 
    if ( (fstream = fopen(pars.mksnrpop_output,"w")) == NULL ) {
    	pre_exit();
	fprintf(stderr,ERRMSG_IO_ERROR,pars.mksnrpop_output);
	exit(ERRNO_IO_ERROR);
    }
 }
 asnr = snrpop;
 if ( status = print_header_mksnrpop(fstream,&pars) ) {
    pre_exit();
    fclose(fstream);
    fstream=NULL;
    exit(status);
 }
 for (i=0; i<pars.mksnrpop_nsnr; i++,asnr++) {
    fprintf(fstream,"%8.2f  %6.2f  %7.2f  %9.4g\n",
    		asnr->asnr_gcd,asnr->asnr_gcl,asnr->asnr_diameter,
		asnr->asnr_sb);
 }
 if (strlen(pars.mksnrpop_output) != 0) { 
    fclose(fstream);
    fstream=NULL;
 }

 pre_exit();
 exit(status);
}

void pre_exit(void)
{
 if (snrpop != NULL)	free_asnrvector(snrpop);
 return;
}

int print_header_mksnrpop(FILE *fstream, MKSNRPOPPARS *pars)
{
 int status=0;
 
 if (fprintf(fstream,"#NSNR\t\t= %d\n",pars->mksnrpop_nsnr) < 0) { status=-1;}
 if (fprintf(fstream,"#SNRATE\t\t= %g\n",pars->mksnrpop_snrate) < 0) { status=-1;}
 if (fprintf(fstream,"#NAMBIENT\t= %g\n",pars->mksnrpop_nambient) < 0) { status=-1;}
 if (fprintf(fstream,"#LIFE\t= %g\n",pars->mksnrpop_nambient) < 0) { status=-1;}
 if (fprintf(fstream,"#MODPOS\t\t= %s\n",pars->mksnrpop_modpos) < 0) { status=-1;}
 if (!strncmp(pars->mksnrpop_modpos,"gaussian",8)) {
    if (fprintf(fstream,"#SCALERAD\t= %g\n",pars->mksnrpop_scalerad) < 0) { status=-1;}
    if (fprintf(fstream,"#ELLIP\t\t= %.2f\n",pars->mksnrpop_ellip) < 0) { status=-1;}
    if (fprintf(fstream,"#PA\t\t= %.2f\n",pars->mksnrpop_pa) < 0) { status=-1;}
 }
 if (fprintf(fstream,"#MODDIAM\t= %s\n",pars->mksnrpop_moddiam) < 0) { status=-1;}
 if (fprintf(fstream,"#E51\t\t= %.2f\n",DEFAULT_E51) < 0) { status=-1;}
 if (fprintf(fstream,"#METAL\t\t= %g\n",DEFAULT_METAL) < 0) { status=-1;}
 if (!strncmp(pars->mksnrpop_moddiam,"adia",4)) {
    if (fprintf(fstream,"#NUM_PARAM (xi)\t= %f\n",DEFAULT_SEDOV_NUM_PARAM) < 0) { status=-1;}
 }
 if (fprintf(fstream,"#MODLUM\t\t= %s\n",pars->mksnrpop_modlum) < 0) { status=-1;}
 if (!strncmp(pars->mksnrpop_modlum,"morel",5)) {
    if (fprintf(fstream,"#COMP_RATIO\t= %.2f\n",DEFAULT_COMPRESSION_FACTOR) < 0) { status=-1;}
 }
 if (fprintf(fstream,"#\n") < 0) { status=-1;}
 if (fprintf(fstream,"#GCD[pc]  GCL[deg]  D[pc]  SB[W/pc2]\n") < 0) { status=-1;}

 if (status < 0) { 
 	fprintf(stderr,ERRMSG_IO_ERROR);
 	status = ERRNO_IO_ERROR;
 }
 return(status);
}

int init_inputs(MKSNRPOPPARS *inputs)
{
 int status=0;
 
 strcpy(inputs->mksnrpop_output,"");
 inputs->mksnrpop_nsnr = -1;
 inputs->mksnrpop_snrate = -1;
 inputs->mksnrpop_nambient = -1;
 inputs->mksnrpop_life = -1.;
 inputs->mksnrpop_scalerad = -1;
 inputs->mksnrpop_ellip = -1;
 inputs->mksnrpop_pa = -1;
 strcpy(inputs->mksnrpop_moddiam,"");
 strcpy(inputs->mksnrpop_modpos,"");
 strcpy(inputs->mksnrpop_modlum,"");

 return(status);
}

int rd_inputs(MKSNRPOPPARS *pars, MKSNRPOPPARS inputs)
{
 int status=0;

 if (strlen(inputs.mksnrpop_output) != 0)
 	strcpy(pars->mksnrpop_output,inputs.mksnrpop_output);
 if (inputs.mksnrpop_nsnr >= 0.0) pars->mksnrpop_nsnr = inputs.mksnrpop_nsnr;
 if (inputs.mksnrpop_snrate >= 0.0)
 	pars->mksnrpop_snrate = inputs.mksnrpop_snrate;
 if (inputs.mksnrpop_nambient >= 0.0)
 	pars->mksnrpop_nambient = inputs.mksnrpop_nambient;
 if (inputs.mksnrpop_life >= 0.0)
 	pars->mksnrpop_life = inputs.mksnrpop_life;
 if (inputs.mksnrpop_scalerad >= 0.0)
 	pars->mksnrpop_scalerad = inputs.mksnrpop_scalerad;
 if (inputs.mksnrpop_ellip >= 0.0)
 	pars->mksnrpop_ellip = inputs.mksnrpop_ellip;
 if (inputs.mksnrpop_pa >= 0.0)
 	pars->mksnrpop_pa = inputs.mksnrpop_pa;
 if (strlen(inputs.mksnrpop_moddiam) != 0)
 	strcpy(pars->mksnrpop_moddiam,inputs.mksnrpop_moddiam);
 if (strlen(inputs.mksnrpop_modpos) != 0)
 	strcpy(pars->mksnrpop_modpos,inputs.mksnrpop_modpos);
 if (strlen(inputs.mksnrpop_modlum) != 0)
 	strcpy(pars->mksnrpop_modlum,inputs.mksnrpop_modlum);

 return(status);
}
