/* Main : snrpopfit */
/* Version : 0.2.2 */
/*   
 *
 * Usage: snrpopfit [-vh] paramfile [-o output]
 *
 *	paramfile	: Name fo the paramter files [default: snrpopfit.param]
 *			  (Searches current directory then looks into 
 *			   installation directory.)
 *	-h|--help	: Print help page
 *	-o output	: Root of output chi2 maps
 *	-v		: Toggle verbose on
 *	--version	: Print version
 *
 * Parameters
 *	input		: Input image, ie. the real image.
 *	output		: Root name of the outputs
 *	noiseimg	: Image used to calculate Poisson noise
 *	section		: Section to use for the fit. IRAF format
 *	nsnr		: Number of SNRs (eg. 100,1000,10000)
 *	snrate		: SN rate (eg. 0.001, 0.01, 0.1)
 *	nambient	: Ambient density (eg. 1, 3, 10)
 *	lthreshold	: Low flux cutoff
 *	hthreshold	: High flux cutoff
 *	mksnrpop	: Name of the paramfile for mksnrpop
 *	artimg		: Name of the paramfile for artimg
 *	addpop		: Name of additional populations of objects
 *	convolve	: Convolve image before comparing? (yes/no)
 *	sigma		: If convolve is yes, sigma of gaussian filter
 *	width		: If convolve is yes, width,in sigma, of the kernel
 *
 * Needs:
 *   %%%libKLutil%%%
 *   %%%artdata%%%
 *   %%%libKLcfitsio%%%
 *   %%%libcfitsio%%%
 *   %%%libKLstats%%%
 *   %%%libKLfile%%%
 */

#include "./snrpopfit.h"
#include <KLutil.h>
#include <KLcfitsio.h>
#include <KLstats.h>	/* for dchstwo() */
#include <KLfile.h>	/* for wrcube() */
#include <KLfft.h>	/* for gconv2d() */
#include <KLfunc.h>	/* for gammq() */
#include <stdio.h>
#include <string.h>	/* for strcpy */
#include <stddef.h>	/* for NULL */
#include <stdlib.h>	/* for system() */
#include <math.h>	/* for pow() */

#define VERSION "0.2.2"

#define KNSTRN 1
#define DF 2
#define SIMIMG "tmpsim.fits"
#define OUTPOP "tmpsnr.pop"

static void pre_exit(void);
static int init_inputs(SNRPOPFITPARS *inputs);
static int rd_inputs(SNRPOPFITPARS *pars, SNRPOPFITPARS inputs);

/* File Scope variables */
static double **preal=NULL, **psim=NULL, **prealtmp=NULL;
static double ***pchi2=NULL,***pprob=NULL;
static double ***pchi2ap=NULL,***pprobap=NULL;
static double **psec=NULL, **pstddev=NULL;

main( argc, argv )
int argc;
char *argv[];
{
 int     FLAGS, status=0;
 unsigned int  p, q, r;
 unsigned long int   nsim=0, isim, start_index=1, index;
 unsigned long int   i, ii, jj, ndata, kk, ll, halfbox[2];
 long int   naxes[2], naxes_sim[2], naxes_sec[2];
 double  nsnr, snrate, nambient;
 double  chi2, prob, best[5], apbest[5];
 double  apflux_real, apfluxerr_real, apstddev_real, apstddeverr_real;
 double  apflux_sim, apfluxerr_sim, apstddev_sim, apstddeverr_sim;
 char    simpop_command[2*MAXLENGTH], simimg_command[2*MAXLENGTH];
 char    paramfile[MAXLENGTH], output[MAXLENGTH], the_default[MAXLENGTH];
 char    **p_parse=NULL, tmp[MAXLENGTH];
 STATS   stats;
 SNRPOPFITPARS pars,inputs;
 FILE    *fstream=NULL;

 /* Initialize */
 FLAGS = 0;
 strcpy(paramfile, "");
 if (status = init_inputs(&inputs)) {
   pre_exit();
   exit(status);
 }
 
 /* Read command line */
 i = 1;
 while (i < argc) {
   if (!strncmp("--", argv[i], 2)) {
      if (!strncmp("--help", argv[i], 6)) {
         fprintf(stdout, HELP_SNRPOPFIT);
         exit(status);
      }
      else if (!strncmp("--version", argv[i], 9)) {
         fprintf(stdout,"snrpopfit v%s\n",VERSION);
         exit(status);
      }
      else if (!strncmp("--input", argv[i], 7)) {
         p_parse = svector(2, MAXLENGTH);
         splitstr(argv[i], p_parse, "=");
         strcpy(inputs.snrpopfit_input, p_parse[1]);
         free_svector(p_parse);
      }
      else if (!strncmp("--output", argv[i], 8)) {
         p_parse = svector(2,MAXLENGTH);
         splitstr(argv[i], p_parse, "=");
         strcpy(inputs.snrpopfit_output, p_parse[1]);
         free_svector(p_parse);
      }
      else if (!strncmp("--noiseimg", argv[i], 10)) {
         p_parse = svector(2, MAXLENGTH);
         splitstr(argv[i], p_parse, "=");
         strcpy(inputs.snrpopfit_noiseimg, p_parse[1]);
         free_svector(p_parse);
      }
      else if (!strncmp("--section", argv[i], 9)) {
         p_parse = svector(2, MAXLENGTH);
         splitstr(argv[i], p_parse, "=");
         sscanf(p_parse[1], "[%lu:%lu,%lu:%lu]%*s\n",
            &inputs.snrpopfit_section[0],
            &inputs.snrpopfit_section[1],
            &inputs.snrpopfit_section[2],
            &inputs.snrpopfit_section[3]);
         free_svector(p_parse);
      }
      else if (!strncmp("--nsnr", argv[i], 6)) {
         p_parse = svector(2, MAXLENGTH);
         splitstr(argv[i],p_parse, "=");
         strcpy(tmp, p_parse[1]);
         free_svector(p_parse);
         p_parse = svector(MAXDIM, MAXLENGTH);
         inputs.snrpopfit_Nnsnr = splitstr(tmp, p_parse, ",");
         inputs.snrpopfit_nsnr = dvector(inputs.snrpopfit_Nnsnr);
         for (i=0; i<inputs.snrpopfit_Nnsnr; i++) {
            inputs.snrpopfit_nsnr[i] = atof(p_parse[i]);
         }
         free_svector(p_parse);
      }
      else if (!strncmp("--snrate", argv[i], 8)) {
         p_parse = svector(2, MAXLENGTH);
         splitstr(argv[i], p_parse, "=");
         strcpy(tmp, p_parse[1]);
         free_svector(p_parse);
         p_parse = svector(MAXDIM,MAXLENGTH);
         inputs.snrpopfit_Nsnrate = splitstr(tmp, p_parse, ",");
         inputs.snrpopfit_snrate = dvector(inputs.snrpopfit_Nsnrate);
         for (i=0; i<inputs.snrpopfit_Nsnrate; i++) {
            inputs.snrpopfit_snrate[i] = atof(p_parse[i]);
         }
         free_svector(p_parse);
      }
      else if (!strncmp("--nambient", argv[i], 10)) {
         p_parse = svector(2, MAXLENGTH);
         splitstr(argv[i], p_parse, "=");
         strcpy(tmp, p_parse[1]);
         free_svector(p_parse);
         p_parse = svector(MAXDIM, MAXLENGTH);
         inputs.snrpopfit_Nnambient = splitstr(tmp, p_parse, ",");
         inputs.snrpopfit_nambient = dvector(inputs.snrpopfit_Nnambient);
         for (i=0; i<inputs.snrpopfit_Nnambient; i++) {
            inputs.snrpopfit_nambient[i] = atof(p_parse[i]);
         }
         free_svector(p_parse);
      }
      else if (!strncmp("--lthreshold", argv[i], 12)) {
         p_parse = svector(2, MAXLENGTH);
         splitstr(argv[i], p_parse, "=");
         inputs.snrpopfit_lthreshold = atof(p_parse[1]);
         free_svector(p_parse);
      }
      else if (!strncmp("--hthreshold", argv[i], 12)) {
         p_parse = svector(2, MAXLENGTH);
         splitstr(argv[i], p_parse, "=");
         inputs.snrpopfit_hthreshold = atof(p_parse[1]);
         free_svector(p_parse);
      }
      else if (!strncmp("--mksnrpop", argv[i], 10)) {
         p_parse = svector(2, MAXLENGTH);
         splitstr(argv[i], p_parse, "=");
         strcpy(inputs.snrpopfit_mksnrpop, p_parse[1]);
         free_svector(p_parse);
      }
      else if (!strncmp("--artimg", argv[i], 8)) {
         p_parse = svector(2, MAXLENGTH);
         splitstr(argv[i], p_parse, "=");
         strcpy(inputs.snrpopfit_artimg, p_parse[1]);
         free_svector(p_parse);
      }
      else if (!strncmp("--addpop", argv[i], 8)) {
         p_parse = svector(2, MAXLENGTH);
         splitstr(argv[i], p_parse, "=");
         strcpy(inputs.snrpopfit_addpop, p_parse[1]);
         free_svector(p_parse);
      }
      else if (!strncmp("--convolve", argv[i], 10)) {
         p_parse = svector(2, MAXLENGTH);
         splitstr(argv[i], p_parse, "=");
         if (!strncmp(p_parse[1], "yes", 3))     { inputs.snrpopfit_convolve = 1; }
         else if (!strncmp(p_parse[1], "no", 2)) { inputs.snrpopfit_convolve = 0; }
         else {
            fprintf(stderr,ERRMSG_INPUT_ERROR, p_parse[1]);
            exit( ERRNO_INPUT_ERROR );
         }
         free_svector(p_parse);
      }
      else if (!strncmp("--sigma", argv[i], 7)) {
         p_parse = svector(2, MAXLENGTH);
         splitstr(argv[i], p_parse, "=");
         inputs.snrpopfit_sigma = atof(p_parse[1]);
         free_svector(p_parse);
      }
      else if (!strncmp("--width", argv[i], 7)) {
         p_parse = svector(2, MAXLENGTH);
         splitstr(argv[i], p_parse, "=");
         inputs.snrpopfit_width = atof(p_parse[1]);
         free_svector(p_parse);
      }
      else if (!strncmp("--index", argv[i], 7)) {
         p_parse = svector(2, MAXLENGTH);
         splitstr(argv[i], p_parse,"=");
         start_index = atoi(p_parse[1]);
         free_svector(p_parse);
      }
      else if (!strncmp("--debug", argv[i], 7)) {
         FLAGS |= 1 << DEBUG;
         FLAGS |= 1 << VERBOSE;
      }
      else {
         fprintf(stderr, ERRMSG_INPUT_ERROR, argv[i]);
         fprintf(stderr, HELP_SNRPOPFIT);
         exit(ERRNO_INPUT_ERROR);
      }
   }
   else if (!strncmp("-", argv[i], 1)) {
      if (!strncmp("-h", argv[i], 2)) {
         fprintf(stdout, HELP_SNRPOPFIT);
         exit(status);
      }
      else if (!strncmp("-n", argv[i], 2)) {
         nsim = atoi(argv[++i]);
      }
      else if (!strncmp("-o", argv[i], 2)) {
         strcpy(inputs.snrpopfit_output, argv[++i]);
      }
      else if (!strncmp("-v", argv[i], 2)) {
         FLAGS |= 1 << VERBOSE;
      }
      else {
         fprintf(stderr, ERRMSG_INPUT_ERROR, argv[i]);
         fprintf(stderr ,HELP_SNRPOPFIT);
         exit(ERRNO_INPUT_ERROR);
      }
   }
   else {
      strcpy(paramfile, argv[i]);
   }
   i++;
 }
 
 /* Find and open parameter file */
 if ( strlen(paramfile) == 0 ) { strcpy(paramfile, DEFAULT_SNRPOPFIT_PARS); }
 if ( (fstream = fopen(paramfile,"r")) == NULL ) { /* look in cur dir or as is*/
   sprintf(the_default, "%s%s", DEFAULT_DIR, paramfile);
   if ( (fstream = fopen(the_default, "r")) == NULL ) {
      fprintf(stderr, ERRMSG_FILE_NOT_FOUND, paramfile);
      exit(ERRNO_FILE_NOT_FOUND);
   }
   if (FLAGS & 1 << VERBOSE) { printf("WARNING: Using '%s'\n", the_default); }
 }
 
 /* Read parameter file */
 if ( status = rd_snrpopfit_param(fstream, &pars) ) {
   fclose(fstream);
   pre_exit();
   exit(status);
 }
 fclose(fstream);
 fstream = NULL;
 
 /* Override pars with inputs */
 if ( status = rd_inputs(&pars, inputs) ) {
   pre_exit();
   exit(status);
 }
 
 /* DEBUG : Print inputs */
 if (FLAGS & 1 << DEBUG) {
   printf ("DEBUG: Input parameters in SNRPOPFITPARS are:\n");
   printf ("DEBUG: pars.snrpopfit_section[0-3]=%lu %lu %lu %lu\n",
      pars.snrpopfit_section[0], pars.snrpopfit_section[1],
      pars.snrpopfit_section[2], pars.snrpopfit_section[3]);
   printf ("DEBUG: pars.snrpopfit_Nnsnr=%d\n", pars.snrpopfit_Nnsnr);
   printf ("DEBUG: pars.snrpopfit_Nsnrate=%d\n", pars.snrpopfit_Nsnrate);
   printf ("DEBUG: pars.snrpopfit_Nambient=%d\n", pars.snrpopfit_Nnambient);
   printf ("DEBUG: pars.snrpopfit_nsnr=");
   for (i=0; i<pars.snrpopfit_Nnsnr; i++) {
      printf ("%f ", pars.snrpopfit_nsnr[i]);
   }
   printf ("\n");
   printf ("DEBUG: pars.snrpopfit_snrate=");
   for (i=0; i<pars.snrpopfit_Nsnrate; i++) {
      printf ("%f ", pars.snrpopfit_snrate[i]);
   }
   printf ("\n");
   printf ("DEBUG: pars.snrpopfit_nambient=");
   for (i=0; i<pars.snrpopfit_Nnambient; i++) {
      printf ("%f ", pars.snrpopfit_nambient[i]);
   }
   printf ("\n");
   printf ("DEBUG: pars.snrpopfit_lthreshold/hthreshold=%f %f\n",
      pars.snrpopfit_lthreshold, pars.snrpopfit_hthreshold);
   printf ("DEBUG: pars.snrpopfit_sigma=%f\n", pars.snrpopfit_sigma);
   printf ("DEBUG: pars.snrpopfit_width=%f\n", pars.snrpopfit_width);
   printf ("DEBUG: pars.snrpopfit_input=%s\n", pars.snrpopfit_input);
   printf ("DEBUG: pars.snrpopfit_output=%s\n", pars.snrpopfit_output);
   printf ("DEBUG: pars.snrpopfit_noiseimg=%s\n", pars.snrpopfit_noiseimg);
   printf ("DEBUG: pars.snrpopfit_mksnrpop=%s\n", pars.snrpopfit_mksnrpop);
   printf ("DEBUG: pars.snrpopfit_artimg=%s\n", pars.snrpopfit_artimg);
   printf ("DEBUG: pars.snrpopfit_addpop=%s\n", pars.snrpopfit_addpop);
   fflush(stdout);
 }

 /* open real img */
 if (status = read_image(pars.snrpopfit_input, &preal, naxes)) {
   pre_exit();
   printerror(status);
 }
 prealtmp = dmatrix(naxes[1],naxes[0]);
 /* cut low and high threshold */
 for (jj=pars.snrpopfit_section[2]; jj<pars.snrpopfit_section[3]; jj++) {
   for (ii=pars.snrpopfit_section[0]; ii<pars.snrpopfit_section[1]; ii++) {
      if (*(*(preal+jj)+ii) < pars.snrpopfit_lthreshold) {
         *(*(preal+jj)+ii) = pars.snrpopfit_lthreshold - 1.;
         *(*(prealtmp+jj)+ii) = 0;
      } else if (*(*(preal+jj)+ii) > pars.snrpopfit_hthreshold) {
         *(*(preal+jj)+ii) = pars.snrpopfit_lthreshold - 1.;
         *(*(prealtmp+jj)+ii) = 0;
      } else
         *(*(prealtmp+jj)+ii) = *(*(preal+jj)+ii);
   }
 }
 /* Calculate 5x5 stddev for pixel => error */
 naxes_sec[0] = pars.snrpopfit_section[1] - pars.snrpopfit_section[0] + 1;
 naxes_sec[1] = pars.snrpopfit_section[3] - pars.snrpopfit_section[2] + 1;
 halfbox[0] = halfbox[1] = 2;	/* 5x5 */
 psec = dmatrix(naxes_sec[1], naxes_sec[0]);
 for (ll=0, jj=pars.snrpopfit_section[2]; 
      jj<pars.snrpopfit_section[3]; ll++,jj++) {
      
   for (kk=0, ii=pars.snrpopfit_section[0];
        ii<pars.snrpopfit_section[1]; kk++, ii++) {
        
      *(*(psec+ll)+kk) = *(*(prealtmp+jj)+ii);
   }
 }
 pstddev = dmatrixc(naxes[1], naxes[0]);
 for (ll=0, jj=pars.snrpopfit_section[2];
      jj<pars.snrpopfit_section[3]; ll++, jj++) {
      
   for (kk=0, ii=pars.snrpopfit_section[0];
        ii<pars.snrpopfit_section[1]; kk++, ii++) {
        
      if (status = stddev2D(psec, kk, ll, naxes_sec, halfbox, &stats, NULL)) {
         fprintf(stderr,ERRMSG_INTERNAL_ERROR);
         pre_exit();
         exit(status);
      }
      /* assume there's as much noise in the model as in the real image */
      *(*(pstddev+jj)+ii) = sqrt(2 * stats.stats_sdev * stats.stats_sdev);
   }
 }
 free_dmatrix(psec);
 psec = NULL;
 free_dmatrix(prealtmp);
 prealtmp = NULL;
 
 /* Calculate apflux and apstddev for the real image */
 status = aperture_phot(preal, naxes, &apflux_real, &apfluxerr_real,
 		             &apstddev_real, &apstddeverr_real);

 pchi2 = dcube(pars.snrpopfit_Nnambient,
 		pars.snrpopfit_Nsnrate,
		pars.snrpopfit_Nnsnr);
 pprob = dcube(pars.snrpopfit_Nnambient,
 		pars.snrpopfit_Nsnrate,
		pars.snrpopfit_Nnsnr);
 pchi2ap = dcube(pars.snrpopfit_Nnambient,
 		pars.snrpopfit_Nsnrate,
		pars.snrpopfit_Nnsnr);
 pprobap = dcube(pars.snrpopfit_Nnambient,
 		pars.snrpopfit_Nsnrate,
		pars.snrpopfit_Nnsnr);

 /* Run nsim realizations */
 for (index=start_index, isim=1; isim<=nsim; index++, isim++) {
 
   fflush(stdout);
   if ( FLAGS & 1 << VERBOSE ) {
      printf("Working on simulation %lu, index %lu ... ", isim, index);
      fflush(stdout);
      
   }
    
   best[0] = 1e30;
   apbest[0] = 1e30;
   for (i=1; i<5; i++) { best[i] = 0; apbest[i] = 0; }
    
   /* Loop over parameter space */
   for (r=0; r<pars.snrpopfit_Nnambient; r++) {
   for (q=0; q<pars.snrpopfit_Nsnrate; q++) {
   for (p=0; p<pars.snrpopfit_Nnsnr; p++) {
   
      nsnr = *(pars.snrpopfit_nsnr + p);
      snrate = *(pars.snrpopfit_snrate + q);
      nambient = *(pars.snrpopfit_nambient + r);
      

      sprintf(simpop_command,
         "nice -19 %s %s -o %s --nsnr=%g --snrate=%g --nambient=%g",
         MKSNRPOP, pars.snrpopfit_mksnrpop, OUTPOP, nsnr, snrate, nambient);
      sprintf(simimg_command,
         "nice -19 %s %s -o %s --inputpop=%s",
         ARTIMG, pars.snrpopfit_artimg, SIMIMG, OUTPOP);
      if (strlen(pars.snrpopfit_addpop) != 0) {
         strcat(simimg_command, ",");
         strcat(simimg_command, pars.snrpopfit_addpop);
      }
      if (FLAGS & 1 << DEBUG) {
         strcat(simpop_command, " --debug");
      }

      if (status = system(simpop_command)) {
         fflush(stdout);
         fprintf(stderr, "\nERROR: Child-process error (%s)\n", MKSNRPOP);
         if (FLAGS & 1 << DEBUG) {
            printf("DEBUGsnrpopfit: isim, index, nsnr, snrate, nambient = %lu %lu %f %f %f\n",
               isim, index, nsnr, snrate, nambient);
            printf("DEBUG: simpop_command = %s\n", simpop_command);
            fflush(stdout);
         }
         pre_exit();
         exit(status);
      }
      if (status = system(simimg_command)) {
         fflush(stdout);
         fprintf(stderr, "\nERROR: Child-process error (%s)\n", ARTIMG);
         if (FLAGS & 1 << DEBUG) {
            printf("DEBUGsnrpopfit: isim, index, nsnr, snrate, nambient = %lu %lu %f %f %f\n",
               isim, index, nsnr, snrate, nambient);
            printf("DEBUG: simimg_command = %s\n", simimg_command);
            fflush(stdout);
         }
         pre_exit();
         exit(status);
      }

      /* open sim img */
      if (status = read_image(SIMIMG, &psim, naxes_sim)) {
         pre_exit();
         printerror(status);
      }
      if ( (naxes_sim[0] != naxes[0]) || (naxes_sim[1] != naxes[1]) ) {
         fprintf(stderr,ERRMSG_INTERNAL_ERROR);
         pre_exit();
         exit(status);
      }

      /* delete tmp files */
      remove(OUTPOP);
      remove(SIMIMG);
      
      /* convolve image if required */
      if (pars.snrpopfit_convolve) {
         if (status = gconv2d(psim, naxes, pars.snrpopfit_sigma, 1., 1.,
             pars.snrpopfit_width)) {
             
            fprintf(stderr, ERRMSG_INTERNAL_ERROR);
            pre_exit();
            exit(ERRNO_INTERNAL_ERROR);
         }
      }
      
      /* mask psim */
      for (jj=0;jj<naxes[1];jj++)
      for (ii=0;ii<naxes[0];ii++)
         if ( *(*(preal+jj)+ii) < pars.snrpopfit_lthreshold )
            *(*(psim+jj)+ii) = pars.snrpopfit_lthreshold - 1.;
      
      /* Compute pixel-pixel chi2 */
      dchisqimg(preal, psim, naxes, pars.snrpopfit_section, pstddev, 
         &chi2, &prob);
      *(*(*(pchi2+r)+q)+p) = chi2;
      *(*(*(pprob+r)+q)+p) = prob;

      /* Find best fit for this realisation */
      if (chi2 < best[0]) {
         best[0] = chi2;
         best[1] = prob;
         best[2] = nsnr;
         best[3] = snrate;
         best[4] = nambient;
      }
      
      /* Compute apflux and apstddev */
      status = aperture_phot(psim, naxes, &apflux_sim, &apfluxerr_sim,
                             &apstddev_sim, &apstddeverr_sim);

      /* Free psim */
      free_dmatrix(psim);
      psim = NULL;

      /* Calculate apflux and apstddev chi2 */
      *(*(*(pchi2ap+r)+q)+p) = 
            pow( (apflux_real - apflux_sim), 2.) /
            (apfluxerr_real*apfluxerr_real + apfluxerr_sim*apfluxerr_sim)
            +
            pow( (apstddev_real - apstddev_sim), 2.) /
            (apstddeverr_real*apstddeverr_real + apstddeverr_sim*apstddeverr_sim);

      *(*(*(pprobap+r)+q)+p) = gammq(0.5*(DF),0.5*(float)chi2);

      /* Find best fit for this realisation */
      if (*(*(*(pchi2ap+r)+q)+p) < apbest[0]) {
         apbest[0] = *(*(*(pchi2ap+r)+q)+p);
         apbest[1] = *(*(*(pprobap+r)+q)+p);
         apbest[2] = nsnr;
         apbest[3] = snrate;
         apbest[4] = nambient;
      }
      
   }}}  /* End loop over parameter space */

   /* Print chi2 and prob maps to files */
   sprintf(output, "%schisq%lu.dat", pars.snrpopfit_output, index);
   if (status = wrcube(output, NULL, NULL, pchi2, pars.snrpopfit_nsnr,
         pars.snrpopfit_snrate, pars.snrpopfit_nambient,
         pars.snrpopfit_Nnsnr, pars.snrpopfit_Nsnrate,
         pars.snrpopfit_Nnambient)) {
      pre_exit();
      exit(status);
   }
   sprintf(output, "%sprob%lu.dat", pars.snrpopfit_output, index);
   if (status = wrcube(output, NULL, NULL, pprob, pars.snrpopfit_nsnr,
         pars.snrpopfit_snrate, pars.snrpopfit_nambient,
         pars.snrpopfit_Nnsnr, pars.snrpopfit_Nsnrate,
         pars.snrpopfit_Nnambient)) {
      pre_exit();
      exit(status);
   }

   /* Print chi2ap and probap maps to files */
   sprintf(output, "%schisqap%lu.dat", pars.snrpopfit_output, index);
   if (status = wrcube(output, NULL, NULL, pchi2ap,pars.snrpopfit_nsnr,
 		   pars.snrpopfit_snrate, pars.snrpopfit_nambient,
		   pars.snrpopfit_Nnsnr, pars.snrpopfit_Nsnrate,
		   pars.snrpopfit_Nnambient)) {
      pre_exit();
      exit(status);
   }
   sprintf(output, "%sprobap%lu.dat", pars.snrpopfit_output, index);
   if (status = wrcube(output, NULL, NULL, pprobap, pars.snrpopfit_nsnr,
         pars.snrpopfit_snrate, pars.snrpopfit_nambient,
         pars.snrpopfit_Nnsnr, pars.snrpopfit_Nsnrate,
         pars.snrpopfit_Nnambient)) {
      pre_exit();
      exit(status);
   }
    
   /* Append best fit file */
   sprintf(output, "%sbest.dat", pars.snrpopfit_output);
   if ( (fstream = fopen(output,"a+")) == NULL) {
      fprintf(stderr,ERRMSG_IO_ERROR,output);
      pre_exit();
      exit(ERRNO_IO_ERROR);
   }
   for (i=0; i<5; i++) fprintf(fstream,"%g  ",best[i]);
   fprintf(fstream, "\n");
   fclose(fstream);
   fstream = NULL;

   /* Append bestap fit file */
   sprintf(output, "%sbestap.dat", pars.snrpopfit_output);
   if ( (fstream = fopen(output,"a+")) == NULL) {
      fprintf(stderr, ERRMSG_IO_ERROR, output);
      pre_exit();
      exit(ERRNO_IO_ERROR);
   }
   for (i=0; i<5; i++) fprintf(fstream,"%g  ",apbest[i]);
   fprintf(fstream,"\n");
   fclose(fstream);
   fstream = NULL;

   fflush(stdout);
   if ( FLAGS & 1 << VERBOSE ) {
      printf("done\n");
      fflush(stdout);
   }

 } /* End run nsim realizations */

 pre_exit();
 exit(status);
}

void pre_exit(void)
{
 if (preal != NULL)     { free_dmatrix(preal); }
 if (prealtmp != NULL)  { free_dmatrix(prealtmp); }
 if (psim != NULL)      { free_dmatrix(psim); }
 if (pchi2 != NULL)     { free_dcube(pchi2); }
 if (pprob != NULL)     { free_dcube(pprob); }
 if (psec != NULL)      { free_dmatrix(psec); }
 if (pstddev != NULL)   { free_dmatrix(pstddev); }
 
 return;
}

int init_inputs(SNRPOPFITPARS *inputs)
{
 int i, status=0;
 
 strcpy(inputs->snrpopfit_input, "");
 strcpy(inputs->snrpopfit_output, "");
 strcpy(inputs->snrpopfit_noiseimg, "");
 strcpy(inputs->snrpopfit_mksnrpop, "");
 strcpy(inputs->snrpopfit_artimg, "");
 strcpy(inputs->snrpopfit_addpop, "");
 for (i=0; i<4; i++) inputs->snrpopfit_section[i] = 0;
 inputs->snrpopfit_Nnsnr = -1;
 inputs->snrpopfit_Nsnrate = -1;
 inputs->snrpopfit_Nnambient = -1;
 inputs->snrpopfit_lthreshold = -999.;
 inputs->snrpopfit_hthreshold = -999.;
 inputs->snrpopfit_convolve = -1;
 inputs->snrpopfit_sigma = -999.;
 inputs->snrpopfit_width = -999.;
 
 return(status);
}

int rd_inputs(SNRPOPFITPARS *pars, SNRPOPFITPARS inputs)
{
 int i, status=0;
 
 if (strlen(inputs.snrpopfit_input) != 0)
   strcpy(pars->snrpopfit_input, inputs.snrpopfit_input);
 if (strlen(inputs.snrpopfit_output) != 0)
   strcpy(pars->snrpopfit_output, inputs.snrpopfit_output);
 if (strlen(inputs.snrpopfit_noiseimg) != 0)
   strcpy(pars->snrpopfit_noiseimg, inputs.snrpopfit_noiseimg);
 if (strlen(inputs.snrpopfit_mksnrpop) != 0)
   strcpy(pars->snrpopfit_mksnrpop, inputs.snrpopfit_mksnrpop);
 if (strlen(inputs.snrpopfit_artimg) != 0)
   strcpy(pars->snrpopfit_artimg, inputs.snrpopfit_artimg);
 if (strlen(inputs.snrpopfit_addpop) != 0)
   strcpy(pars->snrpopfit_addpop, inputs.snrpopfit_addpop);
 if (inputs.snrpopfit_section[0] != 0) {
   for (i=0; i<4; i++)
      pars->snrpopfit_section[i] = inputs.snrpopfit_section[i];
 }
 if (inputs.snrpopfit_Nnsnr >= 0) {
   pars->snrpopfit_Nnsnr = inputs.snrpopfit_Nnsnr;
   free_dvector(pars->snrpopfit_nsnr);
   pars->snrpopfit_nsnr = dvector(pars->snrpopfit_Nnsnr);
   for (i=0; i<pars->snrpopfit_Nnsnr; i++)
      pars->snrpopfit_nsnr[i] = inputs.snrpopfit_nsnr[i];
 }
 if (inputs.snrpopfit_Nsnrate >= 0) {
   pars->snrpopfit_Nsnrate = inputs.snrpopfit_Nsnrate;
   free_dvector(pars->snrpopfit_snrate);
   pars->snrpopfit_snrate = dvector(pars->snrpopfit_Nsnrate);
   for (i=0; i<pars->snrpopfit_Nsnrate; i++)
      pars->snrpopfit_snrate[i] = inputs.snrpopfit_snrate[i];
 }
 if (inputs.snrpopfit_Nnambient >= 0) {
   pars->snrpopfit_Nnambient = inputs.snrpopfit_Nnambient;
   free_dvector(pars->snrpopfit_nambient);
   pars->snrpopfit_nambient = dvector(pars->snrpopfit_Nnambient);
   for (i=0; i<pars->snrpopfit_Nnambient; i++)
      pars->snrpopfit_nambient[i] = inputs.snrpopfit_nambient[i];
 }
 if (inputs.snrpopfit_lthreshold != -999.)
   pars->snrpopfit_lthreshold = inputs.snrpopfit_lthreshold;
 if (inputs.snrpopfit_hthreshold != -999.)
   pars->snrpopfit_hthreshold = inputs.snrpopfit_hthreshold;
 if (inputs.snrpopfit_convolve >= 0) 
   pars->snrpopfit_convolve = inputs.snrpopfit_convolve;
 if (inputs.snrpopfit_sigma != -999.)
   pars->snrpopfit_sigma = inputs.snrpopfit_sigma;
 if (inputs.snrpopfit_width != -999.)
   pars->snrpopfit_width = inputs.snrpopfit_width;

 return(status);
}
