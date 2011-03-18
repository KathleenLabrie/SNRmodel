/* Main : artimg */
/* Version: 0.1.0 */
/*   Create an artificial image of a population of objects
 *
 * Usage: createimg [-vh] paramfile [-o output]
 *
 *	paramfile	: Name of the parameter file [default: mksnrpop.param]
 *			  (Searches current directory first then looks into
 *			   installation directory.)
 *	-h|--help	: Print help page
 *	-o output	: Output image.  Override parameter file.
 *	-v		: Toggle verbose on
 *	--version	: Print version
 *
 * Parameters
 *	input		: Input image. If none, creates a new image.
 *	inputpop	: Input population files.
 *	output		: Name of the output image
 *	distance	: Distance to the galaxy or cluster
 *	pixscale	: Pixel scale of the instrument
 *	naxis		: Size of the image
 *	x0,y0		: Center of the distribution in pixel coordinates
 *			  [If INDEF, defaults to center of the image.]
 *	gain		: Gain of the instrument [ADU/e-]
 *	rdnoise		: Readnoise [e-]
 *	fluxcalib	: Absolute flux calibration [W/m2/ADU]
 *	seeing		: Seeing [arcsec]
 *	psf		: PSF profile [Default: moffat]
 *	skybkg		: Sky background flux [ADU/s]
 *	noiseimg	: Image to use to calculate pre continuum subtraction
 *			  noise.
 *	xnoise		: Add gaussian noise
 *
 * Needs:
 *   %%%libKLutil%%%
 *   %%%libKLcfitsio%%%
 *   %%%libcfitsio%%%
 */

#include "./artdata.h"
#include <KLutil.h>
#include <KLcfitsio.h>
#include <stdio.h>
#include <string.h>	/* for strcpy */
#include <stddef.h>	/* for NULL */

#define VERSION "0.1.0"

static void pre_exit(void);
static void artimg_main_headers(ARTIMGPARS pars, KLFITS_HEADER *list, int *n);
static int init_inputs(ARTIMGPARS *inputs);
static int rd_inputs(ARTIMGPARS *pars, ARTIMGPARS inputs);

/* File Scope variables */
static char **inputpop=NULL;
static double **ppix=NULL,**pmain=NULL,**pbase=NULL,**pnoiseimg=NULL;
static KLFITS_HEADER *headers_vec=NULL;

main( argc, argv )
int argc;
char *argv[];
{
 int i,exp, FLAGS, status=0;
 int npop,pop,nsnrpop=0,nstarpop=0,ngalpop=0,nheaders=0;
 long int ii,jj,input_naxes[2];
 char paramfile[MAXLENGTH],the_default[MAXLENGTH],output[MAXLENGTH];
 char line[2*MAXLENGTH],**p_parse;
 FILE *fstream=NULL;
 ARTIMGPARS pars, inputs;
 KLFITS_HEADER *h,*headers=NULL;

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
     	fprintf(stdout,HELP_ARTIMG);
     	exit(status);
     }
     else if (!strncmp("--version",argv[i],9)) {
     	fprintf(stdout,"artimg v%s\n",VERSION);
     	exit(status);
     }
     else if (!strncmp("--inputpop",argv[i],10)) {
        p_parse = svector(2,MAXLENGTH);
	splitstr(argv[i],p_parse,"=");
	strcpy(inputs.artimg_inputpop,p_parse[1]);
	free_svector(p_parse);
     }
     else if (!strncmp("--input",argv[i],7)) {
        p_parse = svector(2,MAXLENGTH);
	splitstr(argv[i],p_parse,"=");
	strcpy(inputs.artimg_input,p_parse[1]);
	free_svector(p_parse);
     }
     else if (!strncmp("--output",argv[i],8)) {
        p_parse = svector(2,MAXLENGTH);
	splitstr(argv[i],p_parse,"=");
	strcpy(inputs.artimg_output,p_parse[1]);
	free_svector(p_parse);
     }
     else if (!strncmp("--distance",argv[i],10)) {
        p_parse = svector(2,MAXLENGTH);
	splitstr(argv[i],p_parse,"=");
	inputs.artimg_distance = atof(p_parse[1]);
	free_svector(p_parse);
     }
     else if (!strncmp("--pixscale",argv[i],10)) {
        p_parse = svector(2,MAXLENGTH);
	splitstr(argv[i],p_parse,"=");
	inputs.artimg_pixscale = atof(p_parse[1]);
	free_svector(p_parse);
     }
     else if (!strncmp("--naxes",argv[i],7)) {
        p_parse = svector(2,MAXLENGTH);
	splitstr(argv[i],p_parse,"=");
	strcpy(line,p_parse[1]);
	splitstr(line,p_parse,",");
	inputs.artimg_naxes[0] = atoi(p_parse[0]);
	inputs.artimg_naxes[1] = atoi(p_parse[1]);
	free_svector(p_parse);
     }
     else if (!strncmp("--x0",argv[i],4)) {
        p_parse = svector(2,MAXLENGTH);
	splitstr(argv[i],p_parse,"=");
	inputs.artimg_x0 = atof(p_parse[1]);
	free_svector(p_parse);
     }
     else if (!strncmp("--y0",argv[i],4)) {
        p_parse = svector(2,MAXLENGTH);
	splitstr(argv[i],p_parse,"=");
	inputs.artimg_y0 = atof(p_parse[1]);
	free_svector(p_parse);
     }
     else if (!strncmp("--gain",argv[i],6)) {
        p_parse = svector(2,MAXLENGTH);
	splitstr(argv[i],p_parse,"=");
	inputs.artimg_gain = atof(p_parse[1]);
	free_svector(p_parse);
     }
     else if (!strncmp("--rdnoise",argv[i],9)) {
        p_parse = svector(2,MAXLENGTH);
	splitstr(argv[i],p_parse,"=");
	inputs.artimg_rdnoise = atof(p_parse[1]);
	free_svector(p_parse);
     }
     else if (!strncmp("--fluxcalib",argv[i],11)) {
        p_parse = svector(2,MAXLENGTH);
	splitstr(argv[i],p_parse,"=");
	inputs.artimg_fluxcalib = atof(p_parse[1]);
	free_svector(p_parse);
     }
     else if (!strncmp("--seeing",argv[i],8)) {
        p_parse = svector(2,MAXLENGTH);
	splitstr(argv[i],p_parse,"=");
	inputs.artimg_seeing = atof(p_parse[1]);
	free_svector(p_parse);
     }
     else if (!strncmp("--psfwidth",argv[i],10)) {
        p_parse = svector(2,MAXLENGTH);
	splitstr(argv[i],p_parse,"=");
	inputs.artimg_psfwidth = atof(p_parse[1]);
	free_svector(p_parse);
     }
     else if (!strncmp("--psf",argv[i],5)) {
        p_parse = svector(2,MAXLENGTH);
	splitstr(argv[i],p_parse,"=");
	strcpy(inputs.artimg_psf,p_parse[1]);
	free_svector(p_parse);
     }
     else if (!strncmp("--skybkg",argv[i],8)) {
        p_parse = svector(2,MAXLENGTH);
	splitstr(argv[i],p_parse,"=");
	inputs.artimg_skybkg = atof(p_parse[1]);
	free_svector(p_parse);
     }
     else if (!strncmp("--rmvbkg",argv[i],8)) {
        p_parse = svector(2,MAXLENGTH);
	splitstr(argv[i],p_parse,"=");
	if (!strncmp("yes",p_parse[1],3)) inputs.artimg_rmvbkg=1;
	else if (!strncmp("no",p_parse[1],2)) inputs.artimg_rmvbkg=0;
	else {
		fprintf(stderr,ERRMSG_INPUT_ERROR,argv[i]);
		free_svector(p_parse);
		exit(ERRNO_INPUT_ERROR);
	}
	free_svector(p_parse);
     }
     else if (!strncmp("--noiseimg",argv[i],10)) {
        p_parse = svector(2,MAXLENGTH);
	splitstr(argv[i],p_parse,"=");
	strcpy(inputs.artimg_noiseimg,p_parse[1]);
	free_svector(p_parse);
     }
     else if (!strncmp("--xnoise",argv[i],8)) {
        p_parse = svector(2,MAXLENGTH);
	splitstr(argv[i],p_parse,"=");
	inputs.artimg_xnoise = atof(p_parse[1]);
	free_svector(p_parse);
     }
     else if (!strncmp("--exptime",argv[i],9)) {
        p_parse = svector(2,MAXLENGTH);
	splitstr(argv[i],p_parse,"=");
	inputs.artimg_exptime = atof(p_parse[1]);
	free_svector(p_parse);
     }
     else if (!strncmp("--nexp",argv[i],6)) {
        p_parse = svector(2,MAXLENGTH);
	splitstr(argv[i],p_parse,"=");
	inputs.artimg_nexp = atoi(p_parse[1]);
	free_svector(p_parse);
     }
     else {
     	fprintf(stderr, ERRMSG_INPUT_ERROR, argv[i]);
     	fprintf(stderr,HELP_ARTIMG);
     	exit(ERRNO_INPUT_ERROR);
     }
   }
   else if (!strncmp("-",argv[i],1)) {
     if (!strncmp("-h",argv[i],2)) { 
     	fprintf(stdout,HELP_ARTIMG);
	exit(status);
     }
     else if (!strncmp("-o",argv[i],2)) {
     	strcpy(inputs.artimg_output,argv[++i]);
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
 if ( strlen(paramfile) == 0 ) { strcpy(paramfile,DEFAULT_ARTIMG_PARS); }
 if ( (fstream = fopen(paramfile,"r")) == NULL ) { /* look in current dir or as is */
   sprintf(the_default,"%s%s",DEFAULT_DIR,paramfile);
   if ( (fstream = fopen(the_default,"r")) == NULL ) {
   	fprintf(stderr,ERRMSG_FILE_NOT_FOUND,paramfile);
	exit(ERRNO_FILE_NOT_FOUND);
   }
   if (FLAGS & 1 << VERBOSE) { printf("WARNING: Using '%s'\n",the_default); }
 }

 /* Read parameter file */
 if ( status = rd_artimg_param(fstream, &pars) ) {
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

 /* Parse inputpop field */
 p_parse=svector(MAXPOP,MAXLENGTH);
 npop = splitstr(pars.artimg_inputpop,p_parse,",");
 inputpop=svector(npop,MAXLENGTH);
 for (pop=0;pop<npop;pop++) { strcpy(*(inputpop+pop),*(p_parse+pop)); }
 free_svector(p_parse);

 
 /* Open input image, or allocate memory for new image */
 h = create_list_klfits_header();
 h->datatype = TSTRING;
 strcpy(h->keyword,"INPUTIMG");
 strcpy(h->comment,""); 
 if ( strlen(pars.artimg_input) != 0 ) {
    if (status = read_image( pars.artimg_input, &pbase, input_naxes )) {
    	pre_exit();
	printerror( status );
    }
    if ( (input_naxes[0] != pars.artimg_naxes[0]) || 
         (input_naxes[1] != pars.artimg_naxes[1]) ) {
	if (FLAGS & 1<<VERBOSE) {
	  fprintf(stderr,"WARNING: Using input image NAXES values (%d,%d)\n",
	  	input_naxes[0],input_naxes[1]);
	}
	pars.artimg_naxes[0] = input_naxes[0];
	pars.artimg_naxes[1] = input_naxes[1];
    }
    strcpy(h->value_str,pars.artimg_input);
 } else {
    pbase = dmatrixc(pars.artimg_naxes[1],pars.artimg_naxes[0]);
    strcpy(h->value_str,"");
 }
 add_klfits_header( h, &headers );
 nheaders++;

 /* Add artimg parameters to headers */
 artimg_main_headers(pars,headers,&nheaders);

 /* Read noise image */
 pnoiseimg = dmatrixc(pars.artimg_naxes[1],pars.artimg_naxes[0]);
 if (strlen(pars.artimg_noiseimg) != 0) {
   if (status = read_image( pars.artimg_noiseimg, &pnoiseimg, 
   			    pars.artimg_naxes) ) {
	pre_exit();
	printerror(status);
   }
   for (jj=0;jj<pars.artimg_naxes[1];jj++)
     for (ii=0;ii<pars.artimg_naxes[0];ii++) {
       if (*(*(pnoiseimg+jj)+ii) < 0.0) *(*(pnoiseimg+jj)+ii) = 0.0;
       *(*(pnoiseimg+jj)+ii) *= pars.artimg_exptime;
     }
 }

 pmain = dmatrixc(pars.artimg_naxes[1],pars.artimg_naxes[0]);
 for (exp=1;exp<=pars.artimg_nexp;exp++) {
   
   ppix = dmatrixc(pars.artimg_naxes[1],pars.artimg_naxes[0]);
   for (jj=0;jj<pars.artimg_naxes[1];jj++)
     for (ii=0;ii<pars.artimg_naxes[0];ii++)
       *(*(ppix+jj)+ii) = *(*(pbase+jj)+ii);
   
   /* place pop in image */
   for (pop=0;pop<npop;pop++) {
     if ( (fstream = fopen(*(inputpop+pop), "r")) == NULL ) {
   	  fprintf(stderr,ERRMSG_FILE_NOT_FOUND,*(inputpop+pop));
	  pre_exit();
	  exit(ERRNO_FILE_NOT_FOUND);
     }
     if ( fgets( line, 2*MAXLENGTH, fstream ) == NULL ) { 
   	  fprintf(stderr,ERRMSG_INPUT_ERROR,line);
	  pre_exit();
   	  exit(ERRNO_INPUT_ERROR);
     }
     if ( !strncmp(line,"#NSNR",5) ) {
   	  h = create_list_klfits_header();
	  h->datatype = TSTRING;
	  sprintf(h->keyword,"SNRPOP%d",++nsnrpop);
	  strcpy(h->value_str,*(inputpop+pop));
	  strcpy(h->comment,"");
	  add_klfits_header( h, &headers );
	  nheaders++;

   	  rewind(fstream);
	  if (status = artimg_snr(fstream,pars,ppix,headers,&nheaders)) {
		  fprintf(stderr,ERRMSG_INTERNAL_ERROR);
		  pre_exit();
		  exit(ERRNO_INTERNAL_ERROR);
	  }
     } else if ( !strncmp(line,"#NSTAR",6) ) {
   	  /* rewind(fstream);
	  artimg_star();*/
	  fclose(fstream);
   	  fprintf(stderr,"Stellar population not implemented yet.\n");
	  pre_exit();
	  exit(ERRNO_INTERNAL_ERROR);
     } else if ( !strncmp(line,"#NGAL",5) ) {
   	  /* rewind(fstream);
	  artimg_gal(); */
	  fclose(fstream);
   	  fprintf(stderr,"Galaxy population not implemented yet.\n");
	  pre_exit();
	  exit(ERRNO_INTERNAL_ERROR);
     } else {
   	  fclose(fstream);
   	  fprintf(stderr,"Unknown population type.\n");
   	  fprintf(stderr,ERRMSG_INPUT_ERROR,line);
	  pre_exit();
	  exit(ERRNO_INPUT_ERROR);
     }

     fclose(fstream);
     fstream=NULL;
   }

   /* Add background */
   if (status = artimg_bkg(pars,ppix) ) {
 	  pre_exit();
	  exit(status);
   }

   /* Integrate for n seconds */
   if (status = artimg_integrate(pars,ppix)) {
 	  pre_exit();
	  exit(status);
   }

   /* Add seeing */
   if (status = artimg_seeing(pars,ppix)) {
 	  pre_exit();
	  exit(status);
   }

   /* Add poisson noise */
   if (status = artimg_pnoise(pars,ppix,pnoiseimg)) {
 	  pre_exit();
	  exit(status);
   }

   /* Add readout noise */
   if (status = artimg_rdnoise(pars,ppix)) {
 	  pre_exit();
	  exit(status);
   }
   
   /* Add to pmain */
   for (jj=0;jj<pars.artimg_naxes[1];jj++)
     for (ii=0;ii<pars.artimg_naxes[0];ii++)
       *(*(pmain+jj)+ii) += *(*(ppix+jj)+ii);
 }
 
 /* Average image and remove bkg if required */
 if (pars.artimg_rmvbkg) {
    for (jj=0;jj<pars.artimg_naxes[1];jj++)
      for (ii=0;ii<pars.artimg_naxes[0];ii++)
        *(*(pmain+jj)+ii) = *(*(pmain+jj)+ii)/(double)pars.artimg_nexp
				- pars.artimg_skybkg * pars.artimg_exptime;
 } else {
    for (jj=0;jj<pars.artimg_naxes[1];jj++)
      for (ii=0;ii<pars.artimg_naxes[0];ii++)
        *(*(pmain+jj)+ii) /= (double)pars.artimg_nexp;
 }

 /* Add extra noise.  Simulates for example continuum subtraction */
 if (pars.artimg_xnoise > 0.) {
   if (status = artimg_xnoise(pars,pmain)) {
 	pre_exit();
	exit(status);
   }
 }

 /* Output image */
 headers_vec = klfits_header_vector( nheaders );
 h=headers;
 for (i=0;i<nheaders;i++) {
   strcpy((*(headers_vec+i)).keyword, h->keyword);
   strcpy((*(headers_vec+i)).value_str, h->value_str);
   (*(headers_vec+i)).value_byte = h->value_byte;
   (*(headers_vec+i)).value_lint = h->value_lint;
   (*(headers_vec+i)).value_double = h->value_double;
   (*(headers_vec+i)).datatype = h->datatype;
   strcpy((*(headers_vec+i)).comment, h->comment);
   h=h->next;
 }
 strcpy(output,"!");
 strcat(output,pars.artimg_output);
 if (status = create_image(output, pmain, pars.artimg_naxes, headers_vec, nheaders)){
 	pre_exit();
	printerror(status);
 }

 pre_exit();
 exit(status);
}

void pre_exit(void)
{
 if (inputpop != NULL)	{ free_svector(inputpop); }
 if (ppix != NULL)	{ free_dmatrix(ppix); }
 if (pmain != NULL)	{ free_dmatrix(pmain); }
 if (pbase != NULL)	{ free_dmatrix(pbase); }
 if (pnoiseimg != NULL)	{ free_dmatrix(pnoiseimg); }
 /* if (headers_vec != NULL) { free_klfits_header_vector(headers_vec); } */
 return;
}

void artimg_main_headers(ARTIMGPARS pars, KLFITS_HEADER *list, int *n)
{
 KLFITS_HEADER *h;
 
 h = create_list_klfits_header();
 h->datatype=TFLOAT;
 strcpy(h->keyword,"DISTANCE");
 h->value_double=pars.artimg_distance;
 strcpy(h->comment,"[pc]");
 add_klfits_header( h, &list );
 (*n)++;
 
 h=create_list_klfits_header();
 h->datatype=TFLOAT;
 strcpy(h->keyword,"PIXSCALE");
 h->value_double=pars.artimg_pixscale;
 strcpy(h->comment,"[arcsec/pix]");
 add_klfits_header( h, &list );
 (*n)++;
 
 h=create_list_klfits_header();
 h->datatype=TFLOAT;
 strcpy(h->keyword,"X0");
 h->value_double=pars.artimg_x0;
 strcpy(h->comment,"[pixel]");
 add_klfits_header( h, &list );
 (*n)++;
 
 h=create_list_klfits_header();
 h->datatype=TFLOAT;
 strcpy(h->keyword,"Y0");
 h->value_double=pars.artimg_y0;
 strcpy(h->comment,"[pixel]");
 add_klfits_header( h, &list );
 (*n)++;
 
 h=create_list_klfits_header();
 h->datatype=TFLOAT;
 strcpy(h->keyword,"GAIN");
 h->value_double=pars.artimg_gain;
 strcpy(h->comment,"[ADU/e-]");
 add_klfits_header( h, &list );
 (*n)++;
 
 h=create_list_klfits_header();
 h->datatype=TFLOAT;
 strcpy(h->keyword,"RDNOISE");
 h->value_double=pars.artimg_rdnoise;
 strcpy(h->comment,"[e-]");
 add_klfits_header( h, &list );
 (*n)++;
 
 h=create_list_klfits_header();
 h->datatype=TFLOAT;
 strcpy(h->keyword,"FLXCALIB");
 h->value_double=pars.artimg_fluxcalib;
 strcpy(h->comment,"[W/m2/ADU]");
 add_klfits_header( h, &list );
 (*n)++;
 
 h=create_list_klfits_header();
 h->datatype=TFLOAT;
 strcpy(h->keyword,"SEEING");
 h->value_double=pars.artimg_seeing;
 strcpy(h->comment,"[arcsec]");
 add_klfits_header( h, &list );
 (*n)++;
 
 h=create_list_klfits_header();
 h->datatype=TSTRING;
 strcpy(h->keyword,"PSF");
 strcpy(h->value_str,pars.artimg_psf);
 strcpy(h->comment,"");
 add_klfits_header( h, &list );
 (*n)++;
 
 h=create_list_klfits_header();
 h->datatype=TFLOAT;
 strcpy(h->keyword,"PSFWIDTH");
 h->value_double=pars.artimg_psfwidth;
 strcpy(h->comment,"PSFWIDTH * seeing");
 add_klfits_header( h, &list );
 (*n)++;

 h=create_list_klfits_header();
 h->datatype=TFLOAT;
 strcpy(h->keyword,"SKYBKG");
 h->value_double=pars.artimg_skybkg;
 strcpy(h->comment,"[ADU/s]");
 add_klfits_header( h, &list );
 (*n)++;

 h=create_list_klfits_header();
 h->datatype=TSTRING;
 strcpy(h->keyword,"RMVBKG");
 sprintf(h->value_str,"%d",pars.artimg_rmvbkg);
 strcpy(h->comment,"0=false 1=true");
 add_klfits_header( h, &list );
 (*n)++;
 
 h=create_list_klfits_header();
 h->datatype=TSTRING;
 strcpy(h->keyword,"NOISEIMG");
 strcpy(h->value_str,pars.artimg_noiseimg);
 strcpy(h->comment,"");
 add_klfits_header( h, &list );
 (*n)++;

 h=create_list_klfits_header();
 h->datatype=TFLOAT;
 strcpy(h->keyword,"XNOISE");
 h->value_double=pars.artimg_xnoise;
 strcpy(h->comment,"[ADU]");
 add_klfits_header( h, &list );
 (*n)++;
 
 h=create_list_klfits_header();
 h->datatype=TFLOAT;
 strcpy(h->keyword,"EXPTIME");
 h->value_double=pars.artimg_exptime;
 strcpy(h->comment,"[seconds]");
 add_klfits_header( h, &list );
 (*n)++;
 
 h=create_list_klfits_header();
 h->datatype=TINT;
 strcpy(h->keyword,"NEXP");
 h->value_lint=pars.artimg_nexp;
 strcpy(h->comment,"");
 add_klfits_header( h, &list );
 (*n)++;

 return;
}

int init_inputs(ARTIMGPARS *inputs)
{
 int status=0;
 
 strcpy(inputs->artimg_input,"");
 strcpy(inputs->artimg_inputpop,"");
 strcpy(inputs->artimg_output,"");
 inputs->artimg_distance = -1;
 inputs->artimg_pixscale = -1;
 inputs->artimg_naxes[0]=inputs->artimg_naxes[1] = 0;
 inputs->artimg_x0 = -1;
 inputs->artimg_y0 = -1;
 inputs->artimg_gain = -1;
 inputs->artimg_rdnoise = -1;
 inputs->artimg_fluxcalib = -1;
 inputs->artimg_seeing = -1;
 strcpy(inputs->artimg_psf,"");
 inputs->artimg_psfwidth = -1;
 inputs->artimg_skybkg = -1;
 inputs->artimg_rmvbkg = -1;
 strcpy(inputs->artimg_noiseimg,"");
 inputs->artimg_xnoise = -1;
 inputs->artimg_exptime = -1;
 inputs->artimg_nexp = -1;

 return(status);
}

int rd_inputs(ARTIMGPARS *pars, ARTIMGPARS inputs)
{
 int status=0;
 
 if (strlen(inputs.artimg_input) != 0)
 	strcpy(pars->artimg_input,inputs.artimg_input);
 if (strlen(inputs.artimg_inputpop) != 0)
 	strcpy(pars->artimg_inputpop,inputs.artimg_inputpop);
 if (strlen(inputs.artimg_output) != 0)
 	strcpy(pars->artimg_output,inputs.artimg_output);
 if (inputs.artimg_distance >= 0.0) 
 	pars->artimg_distance = inputs.artimg_distance;
 if (inputs.artimg_pixscale >= 0.0)
 	pars->artimg_pixscale = inputs.artimg_pixscale;
 if (inputs.artimg_naxes[0] > 0) 
 	pars->artimg_naxes[0] = inputs.artimg_naxes[0];
 if (inputs.artimg_naxes[1] > 0)
 	pars->artimg_naxes[1] = inputs.artimg_naxes[1];
 if (inputs.artimg_x0 >= 0.0)
 	pars->artimg_x0 = inputs.artimg_x0;
 if (inputs.artimg_y0 >= 0.0)
 	pars->artimg_y0 = inputs.artimg_y0;
 if (inputs.artimg_gain >= 0.0)
 	pars->artimg_gain = inputs.artimg_gain;
 if (inputs.artimg_rdnoise >= 0.0)
 	pars->artimg_rdnoise = inputs.artimg_rdnoise;
 if (inputs.artimg_fluxcalib >= 0.0)
 	pars->artimg_fluxcalib = inputs.artimg_fluxcalib;
 if (inputs.artimg_seeing >= 0.0)
 	pars->artimg_seeing = inputs.artimg_seeing;
 if (strlen(inputs.artimg_psf) != 0)
 	strcpy(pars->artimg_psf,inputs.artimg_psf);
 if (inputs.artimg_psfwidth >= 0.0)
 	pars->artimg_psfwidth = inputs.artimg_psfwidth;
 if (inputs.artimg_skybkg >= 0.0)
 	pars->artimg_skybkg = inputs.artimg_skybkg;
 if (inputs.artimg_rmvbkg >= 0.0)
 	pars->artimg_rmvbkg = inputs.artimg_rmvbkg;
 if (strlen(inputs.artimg_noiseimg) != 0)
 	strcpy(pars->artimg_noiseimg,inputs.artimg_noiseimg);
 if (inputs.artimg_xnoise >= 0.0)
 	pars->artimg_xnoise = inputs.artimg_xnoise;
 if (inputs.artimg_exptime >= 0)
 	pars->artimg_exptime = inputs.artimg_exptime;
 if (inputs.artimg_nexp >= 0.0)
 	pars->artimg_nexp = inputs.artimg_nexp;
 
 return(status);
}
