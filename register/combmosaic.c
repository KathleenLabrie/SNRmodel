/* Main : combmosaic */
/* Version : 0.1.0 */
/*    Combine images into a mosaic.  The images must already be
 *    registered (eg. use IRAF sregister and see the example in the help page).
 *    The images are combined in an "overwrite" mode, they are not averaged
 *    or median combined, they lay on top of each other.  The order of 
 *    precedence is defined by the image list, the firstmost being the lowest
 *    in the stack.
 *
 * Usage: combmosaic [-vh] img1 img2 .. imgN outimg -b oob [--help --verbose --version]
 *        combmosaic [-vh] -l imglist outimg -b oob [--help --verbose --version]
 *
 *	imgN		: Name of the input images
 *	outimg		: Name of the output image
 *	-l imglist	: Specify the name of the file with the list of images
 *			  [one per line]
 *	oob		: Maximum out of bound value.  Any pixels below that
 *			  value will be considered out of bound.
 *
 * Needs:
 *   %%%libKLutil%%%
 *   %%%libKLcfitsio%%%
 *   %%%libcfitsio%%%
 *   %%%libKLfile%%%
 */

#include "./register.h"
#include <KLutil.h>
#include <KLcfitsio.h>
#include <KLfile.h>	/* for rd_vec_string() */
#include <stdio.h>
#include <string.h>	/* for strcpy() */
#include <stddef.h>	/* for NULL */
#include <stdlib.h>	/* for atof() */

#define VERSION "0.1.0"

static void pre_exit(void);

/* File Scope Variables */
static double **ppix=NULL, **pout=NULL;
static char **pimglist=NULL,**parg=NULL;

main( argc, argv )
int argc;
char *argv[];
{
 int FLAGS, status=0;
 int i,n=0;
 long int nimg=0;
 long int ii,jj,naxes[2],naxes_out[2];
 double oob=-999;
 char imglist[MAXLENGTH],outimg[MAXLENGTH];
 
 /* Initialize */
 FLAGS=0;
 strcpy(outimg,"");
 strcpy(imglist,"");
 parg = svector(MAXARG,MAXLENGTH);
 
 /* Read command line */
 i=1;
 while (i < argc) {
   if (!strncmp("--",argv[i],2)) {
     if (!strncmp("--help",argv[i],6)) {
        fprintf(stdout,HELP_COMBMOSAIC);
	pre_exit();
	exit(status);
     }
     else if (!strncmp("--version",argv[i],9)) {
        fprintf(stdout,"combmosaic v%s\n",VERSION);
	pre_exit();
	exit(status);
     }
     else if (!strncmp("--verbose",argv[i],9)) { FLAGS |= 1 << VERBOSE; }
     else {
        fprintf(stderr,ERRMSG_INPUT_ERROR,argv[i]);
	fprintf(stderr,HELP_COMBMOSAIC);
	pre_exit();
	exit(ERRNO_INPUT_ERROR);
     }
   }
   else if (!strncmp("-",argv[i],1)) {
     if (!strncmp("-h",argv[i],2)) {
        fprintf(stdout,HELP_COMBMOSAIC);
	pre_exit();
	exit(status);
     }
     else if (!strncmp("-b",argv[i],2)) { oob = atof(argv[++i]); }
     else if (!strncmp("-l",argv[i],2)) { strcpy(imglist,argv[++i]); }
     else if (!strncmp("-v",argv[i],2)) { FLAGS |= 1 << VERBOSE; }
     else {
        fprintf(stderr,ERRMSG_INPUT_ERROR,argv[i]);
	fprintf(stderr,HELP_COMBMOSAIC);
	pre_exit();
	exit(ERRNO_INPUT_ERROR);
     }
   }
   else {
     strcpy(parg[n++],argv[i]);
   }
   i++;
 }
 
 sprintf(outimg,"!%s",parg[--n]);
 
 /* Open and read imglist, if required */
 if ( strlen(imglist) != 0 ) {
    if (n > 0) {
    	fprintf(stderr,ERRMSG_INPUT_ERROR);
	fprintf(stderr,"You cannot define images AND a list of images\n");
	pre_exit();
	exit(ERRNO_INPUT_ERROR);
    }
    free_svector(parg);
    parg=NULL;
    if ( status = rd_vec_string(imglist,NULL,NULL,1,&pimglist,&nimg) ) {
    	pre_exit();
	exit(status);
    }
    
 } else {
    nimg = n;
    pimglist = svector(nimg,MAXLENGTH);
    for (i=0;i<nimg;i++) { strcpy(pimglist[i],parg[i]); }
    free_svector(parg);
    parg=NULL;
 }
 
 for (i=0;i<nimg;i++) {
   if ( status = read_image( pimglist[i], &ppix, naxes ) ) {
   	pre_exit();
	printerror(status);
   }
   if ( pout == NULL ) { 
   	naxes_out[0] = naxes[0];
	naxes_out[1] = naxes[1];
   	pout = dmatrix(naxes_out[1],naxes_out[0]);
	for (jj=0;jj<naxes_out[1];jj++)
	  for (ii=0;ii<naxes_out[0];ii++)
	    *(*(pout+jj)+ii) = oob;
   }
   if ((naxes[0] != naxes_out[0]) || (naxes[1] != naxes_out[1])) {
   	fprintf(stderr,"ERROR: The images must have the same dimensions\n");
	pre_exit();
	exit(ERRNO_INPUT_ERROR);
   }
   for (jj=0;jj<naxes[1];jj++)
     for (ii=0;ii<naxes[0];ii++)
       if (*(*(ppix+jj)+ii) > oob) *(*(pout+jj)+ii) = *(*(ppix+jj)+ii);

   free_dmatrix(ppix);
   ppix=NULL;
 }
 
 /* Write final image */
 if ( status = edit_image( pimglist[nimg-1], outimg, pout ) ) {
 	pre_exit();
 	printerror(status);
 }

 pre_exit();
 exit(status);
}

void pre_exit(void)
{
 if (ppix != NULL)	{ free_dmatrix(ppix); }
 if (pout != NULL)	{ free_dmatrix(pout); }
 if (pimglist != NULL)	{ free_svector(pimglist); }
 if (parg != NULL)	{ free_svector(parg); }

 return;
}
