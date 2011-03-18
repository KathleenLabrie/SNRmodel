/* Function: dobx */
/* Version: 0.1.1 */
/*   Do the boxcar filtering
 *
 *Needs:
 *   %%%libKLcfitsio%%%
 *   %%%libKLimgutil%%%
 *   %%%libKLutil%%%
 */

#include "./imfilter.h"
#include <KLutil.h>
#include <KLimgutil.h>
#include <KLcfitsio.h>
#include <stdio.h>
#include <string.h>

int dobx(char image[], char new_image[], TBOXCAR *param, char logfile[])
{
 int status,ii,jj;
 long naxes[2];
 unsigned long npts;
 double **ppoints;
 double **ppix=NULL, **pmask=NULL, **pnew=NULL;
 char message[MAXLENGTH];

 printf("Boxcar filter on %s -> %s ... ",image, new_image);
 if (strlen(logfile) != 0) {
   sprintf(message, "Boxcar filter on %s -> ", image);
   if (status = wrlog(logfile,message)) {
     printwarning( status );
     return(1);
   }
 }

 /* Read image */
 if (status = read_image( image, &ppix, naxes )) {
   if (ppix!=NULL) free_dmatrix(ppix);
   printf("\n\t");
   if (strlen(logfile) != 0) {
     sprintf(message, "ERROR WHILE READING IMAGE\n");
     if (status = wrlog(logfile,message)) {
       printwarning( status );
	return(1);
     }
   }
   printwarning( status );
   return(0);
 }

 /* Read a list of coordinate and create a 2D mask */
 if (strncmp(param->bx_mask, "NULL",strlen(param->bx_mask))) {
   if (status = read_points( param->bx_mask, &ppoints, &npts, 0, 0)) {
     if (ppoints!=NULL) free_dmatrix(ppoints);
     free_dmatrix(ppix);
     printf("\n\t");
     if (status==111) { printf("ERROR: Reading points - Too many points\n"); }
     if (strlen(logfile) != 0) {
	sprintf(message, "ERROR WHILE READING MASK\n");
	if (status = wrlog(logfile,message)) {
         printwarning( status );
	  return(1);
	}
     }
     printwarning( status );
     return(0);
   }
   if (status = make_mask(ppoints, npts, naxes, &pmask)) {
     if (pmask != NULL) free_dmatrix(pmask);
     free_dmatrix(ppoints);
     free_dmatrix(ppix);
     printf("\n\t");
     if (strlen(logfile) != 0) {
       sprintf(message, "ERROR WHILE CREATING THE 2D MASK\n");
	if (status = wrlog(logfile,message)) {
	  printwarning( status );
	  return(1);
	}
     }
     printwarning( status );
     return(0);
   }
   free_dmatrix(ppoints);
 }

 /* Boxcar */
 if (!strcmp(param->bx_doclip,"no")) {
    status = boxcar( ppix, pmask, naxes, &param->bx_box, NULL, &pnew);
 } else {
    status = boxcar( ppix, pmask, naxes, &param->bx_box, &param->bx_clip, &pnew);
 }
 if ( status ) {
   if (pnew!=NULL) free_dmatrix(pnew);
   if (pmask!=NULL) free_dmatrix(pmask);
   free_dmatrix(ppix);
   if (strlen(logfile) != 0) {
     sprintf(message, "ERROR WHILE APPLYING BOXCAR FILTER TO IMAGE\n");
     if (status = wrlog(logfile,message)) {
       printwarning( status );
	return(1);
     }
   }
   printf("\n\t");
   printwarning( status );
   return(0);
 }

 /* Write image */
 if (edit_image( image, new_image, pnew )) {
   if (pmask!=NULL) free_dmatrix(pmask);
   free_dmatrix(pnew);
   free_dmatrix(ppix);
   printf("\n\t");
   if (strlen(logfile) != 0) {
     sprintf(message, "ERROR WHILE WRITING IMAGE\n");
     if (status = wrlog(logfile,message)) {
       printwarning( status );
	return(1);
     }
   }
   printwarning( status );
   return(0);
 }

 printf("done\n");
 if (strlen(logfile) != 0) {
   sprintf(message, "%s\n", new_image);
   if (status = wrlog(logfile,message)) {
     printwarning( status );
     return(1);
   }
 }

 return(0);
}
