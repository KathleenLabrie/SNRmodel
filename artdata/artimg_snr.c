/* Function : artimg_snr */
/* Version : 0.1.0 */
/*    Put SNR in image.
 *
 * int artimg_snr(FILE *fstream, ARTIMGPARS pars,
 *		  double **ppix, KLFITS_HEADER *headers, int *nheader)
 *
 * Calling artimg_snr
 *    fstream=fopen(inputpop,"r");
 *    artimg_snr(fstream,pars,ppix,headers,&nheader);
 *
 * Needs:
 *   %%%libKLutil%%%
 *   %%%libKLcfitsio%%%
 */

#include "./artdata.h"
#include <KLutil.h>
#include <KLcfitsio.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <math.h>

int artimg_snr(FILE *fstream, ARTIMGPARS pars, double **ppix, 
		KLFITS_HEADER *headers, int *nheaders)
{
 int ncol, status=0;
 long int xcenter,ycenter,ii,jj;
 double gcd,gcl,diameter,sb;
 double pc2pix,lum2flux,gcdpix,radius,sbinst,r;
 char line[2*MAXLENGTH],**p_parse;
 KLFITS_HEADER *h;

 pc2pix = (pars.artimg_distance * pars.artimg_pixscale) / SECPERRAD;
 lum2flux = 1./(4*PI*pow(pars.artimg_distance*MPERPC,2.));

 p_parse=svector(MKSNRPOP_OUTPUT_NCOL,MAXLENGTH);
 while (fgets( line, 2*MAXLENGTH, fstream ) != NULL) {
   if (!strncmp(line,"#",1)) { 
      h = create_list_klfits_header();
      if (status = parse_popheader(line, h)) {
      	 fprintf(stderr,"ERROR: Parsing header in artimg_snr.\n");
      	 fprintf(stderr,ERRMSG_INPUT_ERROR,line);
	 return(status);
      }
      if ( h->datatype != 0 ) {
        add_klfits_header( h, &headers );
	 (*nheaders)++;
      }
      h = NULL;
      continue;
   }
   ncol = splitstr(line,p_parse,SPACES);
   if (ncol != MKSNRPOP_OUTPUT_NCOL) {
   	fprintf(stderr,ERRMSG_INPUT_ERROR);
	return(ERRNO_INPUT_ERROR);
   }
   gcd = atof(p_parse[0]);
   gcl = atof(p_parse[1]);
   diameter = atof(p_parse[2]);
   sb = atof(p_parse[3]);
   gcdpix = gcd / pc2pix;
   xcenter = (long int)(pars.artimg_x0 + cos(gcl*PI/180.)*gcdpix);
   ycenter = (long int)(pars.artimg_y0 + sin(gcl*PI/180.)*gcdpix);
   radius = 0.5 * diameter / pc2pix;
   if ( (xcenter+radius < 1) || (xcenter-radius > pars.artimg_naxes[0]) ||
        (ycenter+radius < 1) || (ycenter-radius > pars.artimg_naxes[1]) ) {
     continue;
   }
   sbinst = sb * pow(pc2pix,2) * lum2flux / pars.artimg_fluxcalib;
   
   for (jj=ycenter-radius-1; jj<ycenter+radius; jj++) {
     if ( (jj>=0) && (jj<pars.artimg_naxes[1]) ) {
       for (ii=xcenter-radius-1; ii<xcenter+radius; ii++) {
         if ( (ii>=0) && (ii<pars.artimg_naxes[0]) ) {
	   r = sqrt( pow(ii-xcenter,2.) + pow(jj-ycenter,2.) );
	   if ( r <= radius ) { *(*(ppix+jj)+ii) += sbinst; }
	 }
       }
     }
   }
 }
 free_svector(p_parse);

 return(status);
}
