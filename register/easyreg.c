/* Main: easyreg */
/* Version: 1.0.1 */
/*	Register two images.  Simply shift and multiply two images 
 *	and outputs a 2-D map of the median flux.  A first guess is
 *	highly recommended.  The first image is assume to be the 
 *	reference image.  Both image must have the same dimensions.
 *
 * Usage: easyreg ref img -o root --init=dx,dy --box=size --section='[x1:x2,y1:y2]'
 *
 *	ref :			name of the reference image
 *	img :			name of the image to align
 *	-o root :		root of the output files
 *	--init=dx,dy :	user's first guess.  d = ref-img
 *	--box=size :		size of the search box centered on dx,dy
 *	--section:		section on which the stats will be calculated.
 *	--pedestal:		add a pedestal value to the image to match.
 *				  useful when the median flux is ~0 and there are
 *				  negative values.
 *	--cut:			any pixel value lower than this are set to zero.
 *
 * Needs:
 *   %%%libcfitsio%%%
 *   %%%libKLcfitsio%%%
 *   %%%libKLutil%%%
 *   %%%libKLstats%%%
 *   %%%libKLimgmath%%%
 */

#include <KLutil.h>		/* for STATS, MAXLENGTH and others */
#include <KLimgmath.h>		/* for mul2Images */
#include <stdio.h>		/* for file I/O */
#include <stddef.h>		/* for NULL */
#include <stdlib.h>		/* for atoi */
#include <string.h>		/* for strrchr */

#define NOCUT -1e30

main( argc, argv )
int argc;
char *argv[];
{
 int status=0, CHANGE_FLAG=0;
 int i, ii, jj, kk, ll, nimages=0, sh, pad;
 int *imgpos, length;
 long box, shiftx, shifty, firstdx, firstdy;
 unsigned long naxes[2], naxes_img[2], naxes_sec[2], section[4];
 double **pref=NULL, **pimg=NULL, **pmap=NULL, **pmult=NULL;
 double median, pedestal,cutlevel;
 char **pimages=NULL;
 char root[MAXLENGTH], output[MAXLENGTH], tmp1[MAXLENGTH], tmp2[MAXLENGTH];
 char **p_parse, *pdot;
 FILE *ostream;
 STATS stats;

 /* Initialize */
 strcpy(root, "");
 imgpos = ivector(argc - 1);
 pedestal=0.;
 cutlevel=NOCUT;

 /* Read command line */
 i=1;
 while (i < argc) {
   if (!strncmp("-o",argv[i],2)) {
     strcpy(root, argv[++i]);
   }
   else if (!strncmp("--init",argv[i],6)) {
     p_parse = svector(2,MAXLENGTH);
     splitstr(argv[i], p_parse, "=");
     strcpy(tmp1, p_parse[1]);
     splitstr(tmp1, p_parse, ",");
     firstdx = atoi(p_parse[0]);
     firstdy = atoi(p_parse[1]);
     free_svector(p_parse);
   }
   else if (!strncmp("--box",argv[i], 5)) {
     p_parse = svector(2,MAXLENGTH);
     splitstr(argv[i], p_parse, "=");
     box = atoi(p_parse[1]);
     free_svector(p_parse);
   }
   else if (!strncmp("--section",argv[i], 9)) {
     p_parse = svector(2,MAXLENGTH);
     splitstr(argv[i], p_parse, "=");
     strcpy(tmp1, p_parse[1]);
     splitstr(tmp1, p_parse, "[");
     strcpy(tmp1,p_parse[0]);
     splitstr(tmp1, p_parse, "]");
     strcpy(tmp1,p_parse[0]);
     splitstr(tmp1, p_parse, ",");
     strcpy(tmp1,p_parse[0]);
     strcpy(tmp2,p_parse[1]);
     splitstr(tmp1, p_parse, ":");
     section[0] = atoi(p_parse[0]);
     section[1] = atoi(p_parse[1]);
     splitstr(tmp2, p_parse, ":");
     section[2] = atoi(p_parse[0]);
     section[3] = atoi(p_parse[1]);
     free_svector(p_parse);
   }
   else if (!strncmp("--pedestal",argv[i],10)) {
     p_parse = svector(2,MAXLENGTH);
     splitstr(argv[i], p_parse, "=");
     pedestal = atof(p_parse[1]);
     free_svector(p_parse);
   }
   else if (!strncmp("--cut",argv[i],5)) {
     p_parse = svector(2,MAXLENGTH);
     splitstr(argv[i], p_parse, "=");
     cutlevel = atof(p_parse[1]);
     free_svector(p_parse);
   }
   else {
     imgpos[nimages++] = i;
   }
   i++;
 }

 /* Get the image names */
 pimages = svector(nimages,MAXLENGTH);
 for (i=0; i<nimages; i++) {
   strcpy(pimages[i],argv[imgpos[i]]);
 }
 
 /* Make box a odd number */
 if ( !(box % 2) ) { box++; }

 /* Allocate memory for the output data */
 pmap = dmatrix(box*box,6);
 
 /* Allocate memory for the stats section */
 naxes_sec[0]=section[1]-section[0]+1;
 naxes_sec[1]=section[3]-section[2]+1;
 pmult = dmatrixc(naxes_sec[1],naxes_sec[0]);

 /* Read in the reference image */
 if ( status = read_image( pimages[0], &pref, naxes ) ) {
 	if (pref != NULL) { free_dmatrix(pref); }
	free_svector( pimages );
	free_dmatrix( pmap );
	free_dmatrix( pmult );
	printerror( status );
 } 
 /* cut all values below cutlevel */
 if (cutlevel > NOCUT+1) {
   for (jj=0;jj<naxes[1];jj++) {
     for (ii=0;ii<naxes[0];ii++) {
       if ( *(*(pref+jj)+ii) < cutlevel ) *(*(pref+jj)+ii) = 0;
     }
   }
 }

 /* Loop over all the other images */
 for (i=1; i<nimages; i++) {

   /* read in the image */
   if ( status = read_image( pimages[i], &pimg, naxes_img) ) {
   	if (pimg != NULL) { free_dmatrix(pimg); }
	free_dmatrix( pref );
	free_svector( pimages );
	free_dmatrix( pmap );
	free_dmatrix( pmult );
	printerror( status );
   }
   if ((naxes_img[0] != naxes[0]) || (naxes_img[1] != naxes[1])) {
   	printf("ERROR: Image %s not the same size as reference (%s)\n",
			pimages[i], pimages[0]);
	free_dmatrix( pref );
	free_dmatrix( pimg );
	free_svector( pimages );
	free_dmatrix( pmap );
	free_dmatrix( pmult );
	exit(1);
   }
   /* add pedestral level */
   if (pedestal != 0.) {
     for (jj=0;jj<naxes[1];jj++) {
       for (ii=0;ii<naxes[0];ii++) {
         *(*(pimg+jj)+ii) += pedestal;
       }
     }
   }
   /* cut all values below cutlevel */
   if (cutlevel > NOCUT+1) {
     for (jj=0;jj<naxes[1];jj++) {
       for (ii=0;ii<naxes[0];ii++) {
         if ( *(*(pimg+jj)+ii) < cutlevel ) *(*(pimg+jj)+ii) = 0;
       }
     }
   }
   
   /* Make the shift doesn't bring the section out of bound */
   /* If it does, reduce the size of the section.  It is important that */
   /* all the points in the map be calculated with the same number of  */
   /* good pixels since the map is made of the sums */
   
   CHANGE_FLAG=0;
   /* Check low x */
   if ( (pad = section[0] - (firstdx+(long)(box/2))) < 1 ) {
   		section[0] -= pad;		/* pad is negative or zero */
		CHANGE_FLAG=1;
   }
   /* Check high x */
   if ( (pad = section[1] - (firstdx-(long)(box/2))) > naxes[0] ) {
   		section[1] -= pad-naxes[0];
		CHANGE_FLAG=1;
   }
   /* Check low y */
   if ( (pad = section[2] - (firstdy+(long)(box/2))) < 1 ) {
   		section[2] -= pad;		/* pad is negative or zero */
		CHANGE_FLAG=1;
   }
   /* Check high y */
   if ( (pad = section[3] - (firstdy-(long)(box/2))) > naxes[1] ) {
   		section[3] -= pad-naxes[1];
		CHANGE_FLAG=1;
   }
   
   /* Consistency check */
   if ( (long)section[0] > (long)section[1] ) {
     printf("ERROR: Section could not be readjusted in X.\n");
     exit(1);
   }
   if ( (long)section[2] > (long)section[3] ) {
     printf("ERROR: Section could not be readjusted in Y.\n");
     exit(1);
   }
   
   if (CHANGE_FLAG) {
     printf("\tWARNING: Section has been readjusted to [%d:%d,%d:%d]\n",
     		section[0],section[1],section[2],section[3]);
   }
   
   /* Scan the search box */
   sh=0;	/* shift number sh (max=box*box) */
   for (shifty=firstdy-(long)(box/2); shifty<=firstdy+(long)(box/2); shifty++) {
     for (shiftx=firstdx-(long)(box/2); shiftx<=firstdx+(long)(box/2); shiftx++) {

       /* Multiply one image section by the other's section */
	/* (No need to multiply the whole image since only interested */
	/*  in stats section */
	if ( status = mul2Images(pref,pimg,pmult,naxes,section,shiftx,shifty) ) {
		free_dmatrix(pref);
		free_dmatrix(pimg);
		free_svector(pimages);
		free_dmatrix(pmult);
		free_dmatrix(pmap);
		printf("ERROR: mul2Images (%d)\n",status);
		exit(status);
	}

       /* Calculate the stats for current shift */
	if ( status = stddevbox(pmult, naxes_sec, &stats, NULL) ) {
		free_dmatrix(pref);
		free_dmatrix(pimg);
		free_svector(pimages);
		free_dmatrix(pmult);
		free_dmatrix(pmap);
		printf("ERROR: stddevbox (%d)\n",status);
		exit(status);
	}
		
	
	/* Add to output (shiftx, shifty, sum, sumsq, ave, sdev) */
	*(*(pmap+sh)+0) = (double)shiftx;
	*(*(pmap+sh)+1) = (double)shifty;
	*(*(pmap+sh)+2) = stats.stats_sum;
	*(*(pmap+sh)+3) = stats.stats_sumsq;
	*(*(pmap+sh)+4) = stats.stats_ave;
	*(*(pmap+sh)+5) = stats.stats_sdev;
	sh++;
     } 
   }
   
   /* Output the results for this image */
   /* 1- change the suffix */
   strcpy(tmp1, pimages[i]);
   pdot = strrchr( tmp1, '.');
   if (pdot == NULL) { length = strlen(tmp1); }
   else { length = pdot - tmp1; }
   strcpy(tmp1,"");
   strncat(tmp1, pimages[i], length);

   /* 2- Insert the prefix (might be the full path) */
   pdot = strrchr( tmp1, '/');
   if (pdot == NULL) { 	/* just the file name */
   	strcpy(output, root);
	strcat(output, tmp1);
	strcat(output, ".reg");
   } else {			/* full path (suffix has been striped) */
       length = pdot - tmp1;
	strcpy(output,"");
	strncat(output,tmp1,length);
	strcat(output,"/");
	strcat(output,root);
	strcat(output,pdot+1);
	strcat(output,".reg");
   }

   ostream = fopen(output, "w");
   for (sh=0; sh<box*box; sh++) {
     fprintf(ostream, "%4d  %4d  %.5g  %.5g  %.5f  %.3f\n", 
     		(int)*(*(pmap+sh)+0),
		(int)*(*(pmap+sh)+1),
		*(*(pmap+sh)+2),
		*(*(pmap+sh)+3),
		*(*(pmap+sh)+4),
		*(*(pmap+sh)+5));
   }
   fclose(ostream);
 }
 
 /* Desallocate all memory */
 free_dmatrix(pref);
 free_dmatrix(pimg);
 free_svector(pimages);
 free_dmatrix(pmap);
 free_dmatrix(pmult);

 exit(0);
}
