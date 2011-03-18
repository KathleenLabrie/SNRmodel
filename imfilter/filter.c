/* Main: filter
 * Version: 0.1.0
 *   Apply a filter to an image. (eg. boxcar, median, gaussian filters)
 *   This is the interface to access the filtering algorithm.
 *   Variable box size allowed, masking allowed, clipping allowed.
 *   Currently implemented filter : boxcar.
 *
 * Usage: filter images|-l list [-o output|-lo list|--root=root] --type=filter 
 *		   --param=file --log=logfile
 *
 *   	images			List of images
 *	-l list		Name of the file with the list of images
 *	-o output		Output image's name
 *	-lo list		Name of the file with the list of output image names
 *	--root=root		Prefix to append to input images' name
 *	--type=filter		Type of filtering algorithm
 *	--param=file		Name of the file containing user defined parameters
 *	--log=logfile		Name of the log file
 *
 * Notes:
 *   If no output image names are provided, overwrite images.
 *   If output names are provided, their number must be equal to the number
 *      of input images.
 *   The content of the parameter file depends on the filter selected (see
 *      below).
 *
 * Needs:
 *   %%%libKLutil%%%
 *   %%%libKLcfitsio%%%
 *   %%%libKLimgutil%%%
 */

#include "./imfilter.h"
#include <KLutil.h>
#include <stdio.h>
#include <stddef.h>		/* for NULL */
#include <string.h>		/* for strlen() */
#include <stdlib.h>		/* for atof */


main(argc,argv)
int argc;
char *argv[];
{
 int FILTFLAG;
 int i,nimages=0,nnimages=0;
 char **pimages, **pnew_images, root[MAXLENGTH/2], output[MAXLENGTH];
 char paramfile[MAXLENGTH],ilist[MAXLENGTH],olist[MAXLENGTH];
 char logfile[MAXLENGTH],type[MAXLENGTH],line[MAXLENGTH];
 char **p_parse;
 FILE *istream;
 TBOXCAR bxparam;

 /* Initialize some variables */
 FILTFLAG=0;
 strcpy(ilist,"");
 strcpy(paramfile,"");
 strcpy(olist,"");
 strcpy(logfile,"");
 strcpy(type,"");
 strcpy(root,"");
 strcpy(output,"");

 /* Allocate memory for 'images'; max number of images = argc-3 
 /*							(filter,--type,--param)*/
 pimages = svector(argc-3,MAXLENGTH);

 /* Read command line */
 i=1;
 while (i < argc) {
   if (!strncmp("-l",argv[i],2))       { strcpy( ilist, argv[++i] ); }
   else if (!strncmp("-ol",argv[i],3)) { strcpy( olist, argv[++i] ); }
   else if (!strncmp("-o",argv[i],2)) { strcpy( output, argv[++i] ); }
   else if (!strncmp("--root",argv[i],6)) {
     p_parse = svector(2,MAXLENGTH);
     splitstr(argv[i], p_parse, "=");
     strcpy( root, p_parse[1]);
     free_svector(p_parse);
   }
   else if (!strncmp("--type",argv[i],6)) {
     p_parse = svector(2,MAXLENGTH);
     splitstr(argv[i], p_parse, "=");
     strcpy( type, p_parse[1]);
     free_svector(p_parse);
   }
   else if (!strncmp("--param",argv[i],7)) {
     p_parse = svector(2,MAXLENGTH);
     splitstr(argv[i], p_parse, "=");
     strcpy( paramfile, p_parse[1]);
     free_svector(p_parse);
   }
   else if (!strncmp("--log",argv[i],5)) {
     p_parse = svector(2,MAXLENGTH);
     splitstr(argv[i], p_parse, "=");
     strcpy( logfile, p_parse[1]);
     free_svector(p_parse);
   }
   else { strcpy( pimages[nimages++], argv[i]); }
   i++;
 }


/* Read files, do some checks on the inputs */
 if (strlen(type) == 0) {
   printf("ERROR: No filtering algorithm.\n");
   free_svector(pimages);
   exit(1);
 }
 if (strlen(paramfile) == 0) {
   printf("ERROR: No parameter file.\n");
   free_svector(pimages);
   exit(1);
 }
 if (strlen(ilist)) {
   istream = fopen(ilist,"r");
   while (fgets( line, MAXLENGTH, istream ) != NULL) {
     strncpy( pimages[nimages++], line, strcspn(line,"\n"));
   }
   fclose(istream);
 }
 if (nimages < 1) {
   printf("ERROR: No input images.\n");
   free_svector(pimages);
   exit(1);
 }

 pnew_images=svector(nimages,MAXLENGTH);	/* Allocate memory for output image names */

 if (strlen(olist)) {
   istream = fopen(olist,"r");
   while (fgets( line, MAXLENGTH, istream ) != NULL) {
     strncpy( pnew_images[nnimages++], line, strcspn(line,"\n"));
   }
   fclose(istream);
   if (nnimages != nimages) {
     printf("ERROR: Number of output images not equal to number of input images\n");
     free_svector(pimages);
     free_svector(pnew_images);
     exit(1);
   }
   for (i=0;i<nimages;i++) {
     if (!strncmp(pnew_images[i], pimages[i], strlen(pimages[i]))) {
       sprintf( output, "%s%s", "!",pimages[i]); 	/*overwrite*/
	strcpy(pnew_images[i],output);
     }
   }
 }
 else if (strlen(output)) {
   if (nimages > 1) {
     printf("ERROR: Number of output images not equal to number of input images\n");
     free_svector(pimages);
     exit(1);
   }
   /* nimages == 1 */
   if (!strncmp(pimages[0],output,strlen(pimages[0]))) {
     sprintf(pnew_images[nnimages++],"%s%s","!",pimages[0]);	/*overwrite*/
   } else {
     strcpy( pnew_images[nnimages++], output);
   }
 }
 else if (strlen(root)) {
   for (i=0;i<nimages;i++) {
     sprintf(pnew_images[nnimages++],"%s%s",root,pimages[i]);
   }
   if (nnimages != nimages) {
     printf("ERROR: Internal error.  Number of output images not equal to number of input images.\n");
     free_svector(pimages);
     free_svector(pnew_images);
     exit(1);
   }
 }
 else {	/* overwrite */
   for (i=0;i<nimages;i++) {
     sprintf(pnew_images[nnimages++],"%s%s","!",pimages[i]);
   }
   if (nnimages != nimages) {
     printf("ERROR: Internal error.  Number of output images not equal to number of input images.\n");
     free_svector(pimages);
     free_svector(pnew_images);
     exit(1);
   }
 }

 /* Translate filter type */
 if (!strncmp(type,"boxcar",strlen(type))) { FILTFLAG |= 1 << 0; }
 else {
   printf("ERROR: Bad filter selection.  Options are: 'boxcar'.\n");
   free_svector(pimages);
   free_svector(pnew_images);
   exit(1);
 }

 /* Open user parameter file and check inputs */
 /* Then start filter */
 switch (FILTFLAG) 
 {
 	case BOXCAR:
		if (initbx(paramfile,&bxparam) != 0) {
		  printf("ERROR: Unable to initialize BOXCAR.\n");
		  free_svector(pimages);
		  free_svector(pnew_images);
		  exit(1);
		}
		for (i=0; i<nimages; i++) {
		  if (dobx(pimages[i],pnew_images[i],&bxparam,logfile)) {
		    free_svector(pimages);
		    free_svector(pnew_images);
		    exit(1);
		  }
		}
		break;
	default:
		printf("ERROR: Internal error. Bad filter selection.\n");
		free_svector(pimages);
		free_svector(pnew_images);
		exit(1);
 }

 free_svector(pnew_images);
 free_svector(pimages);

 exit(0);
}
