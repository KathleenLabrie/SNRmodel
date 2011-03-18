/* Main : snrpopbfit */
/* Version : 0.1.0 */
/*    Average the param space chisq and find chisqmin.
 *
 * Usage: snrpopbfit [-vh] input_root output [--version --help]
 *
 *	input_root	: Root name of the chisq input files
 *	output		: Name of the output file
 *	-h|--help	: Print help page
 *	-v		: Toggle verbose on
 *	--version	: Print version
 *
 * Needs:
 *   %%%libKLutil%%%
 *   %%%libKLfile%%%
 */

#include "./snrpopfit.h"
#include <KLutil.h>
#include <KLfile.h>	/* for wrcube(), rdcube() */
#include <stdio.h>
#include <stddef.h>	/* for NULL */
#include <string.h>	/* for strcpy() */
#include <stdlib.h>	/* for system() */
#include <math.h>   /* for gcc4.0, sqrt() */

#define VERSION "0.1.0"

#define DEBUGMODE 0

#define MAXARG 20
#define TMPFILELIST "tmpfilelist"
#define TMPFILELISTAP "tmpfilelistap"
#define AP 1
#define HELP_SNRPOPBFIT "Help!!!"

static void pre_exit(void);

/* File Scope Variables */
static double ***incube=NULL,***outcube=NULL,***outstdcube=NULL;
static double ***sumcube=NULL,***sumsqcube=NULL;
static double *axis1=NULL,*axis2=NULL,*axis3=NULL;
static double *taxis1=NULL,*taxis2=NULL,*taxis3=NULL;
static char **filelist=NULL;

main(argc,argv)
int argc;
char *argv[];
{
 int FLAGS,TYPE=0,status=0;
 int i,j,k,n;
 long int n1,n2,n3,tn1,tn2,tn3;
 unsigned long int nfit;
 double var;
 char inroot[MAXLENGTH],output[MAXLENGTH],listname[MAXLENGTH];
 char **otherargs=NULL,command[MAXLENGTH],line[MAXLENGTH],**p_parse=NULL;
 FILE *istream,*ostream;

 /* Initialize */
 FLAGS = 0;
 if (DEBUGMODE) FLAGS |= 1 << DEBUG;
 strcpy(inroot,"");
 strcpy(output,"");
 
 /* Read command line */
 otherargs = svector(MAXARG,MAXLENGTH);
 n=0;
 i=1;
 while (i < argc) {
   if (!strncmp("--",argv[i],2)) {
     if (!strncmp("--help",argv[i],6)) {
     	fprintf(stdout,HELP_SNRPOPBFIT);
	exit(status);
     }
     else if (!strncmp("--ap",argv[i],4)) { TYPE |= 1 << AP; }
     else if (!strncmp("--version",argv[i],9)) {
     	fprintf(stdout,"snrpopbfit v%s\n",VERSION);
	exit(status);
     }
     else {
	fprintf(stderr, ERRMSG_INPUT_ERROR, argv[i]);
	fprintf(stderr,HELP_SNRPOPFIT);
	exit(ERRNO_INPUT_ERROR);
     }
   }
   else if (!strncmp("-",argv[i],1)) {
     if (!strncmp("-h",argv[i],2)) {
     	fprintf(stdout,HELP_SNRPOPBFIT);
	exit(status);
     }
     else if (!strncmp("-v",argv[i],2)) { FLAGS |= 1 << VERBOSE; }
     else {
     	fprintf(stderr,ERRMSG_INPUT_ERROR,argv[i]);
        fprintf(stderr,HELP_SNRPOPFIT);
        exit(ERRNO_INPUT_ERROR);
     }
   }
   else {
     strcpy(otherargs[n++],argv[i]);
   }
   i++;
 }
 if (n != 2) {
 	free_svector(otherargs);
	fprintf(stderr,ERRMSG_INPUT_ERROR,"Too many arguments");
	exit(ERRNO_INPUT_ERROR);
 }
 strcpy(inroot,otherargs[0]);
 strcpy(output,otherargs[1]);
 free_svector(otherargs);

 if (TYPE & 1 << AP) {
    sprintf(command,"ls %schisqap[1-9]*.dat | wc > %s",inroot,TMPFILELISTAP);
    system(command);
    sprintf(command,"ls %schisqap[1-9]*.dat >> %s",inroot,TMPFILELISTAP);
    system(command);
    strcpy(listname,TMPFILELISTAP);
 } else {
    sprintf(command,"ls %schisq[1-9]*.dat | wc > %s",inroot,TMPFILELIST);
    system(command);
    sprintf(command,"ls %schisq[1-9]*.dat >> %s",inroot,TMPFILELIST);
    system(command);
    strcpy(listname,TMPFILELIST);
 }
 
 if ( (istream = fopen(listname,"r")) == NULL ) {
 	fprintf(stderr,ERRMSG_IO_ERROR,listname);
	exit(ERRNO_IO_ERROR);
 }
 fscanf(istream,"%d %*d %*d\n",&nfit);
 if (FLAGS & 1 << VERBOSE) fprintf(stdout,"Found %d fit files\n",nfit);
 filelist = svector(nfit,MAXLENGTH);
 p_parse = svector(1,MAXLENGTH);
 n=0;
 while (fgets( line, MAXLENGTH, istream ) != NULL) {
   splitstr(line,p_parse,SPACES);	/* gets rid of \n, if it's there */
   strcpy(filelist[n++],p_parse[0]);
 }
 fclose(istream);
 istream=NULL;
 if (n != nfit) {
 	fprintf(stderr,ERRMSG_INTERNAL_ERROR);
	if (FLAGS & 1 << DEBUG)
	  fprintf(stderr,"Number of fit files and number in list not equal.\n");
	pre_exit();
	exit(status);
 }
 if (!(FLAGS & 1 << DEBUG)) remove(listname);
 
 /* Open first cube to get axes length */
 if (status = rdcube(filelist[0],NULL,NULL, &incube, &axis1, &axis2, &axis3,
 		&n1,&n2,&n3) ) {
	fprintf(stderr,ERRMSG_IO_ERROR,filelist[0]);
	pre_exit();
	exit(status);
 }
 /* Clear incube */
 free_dcube(incube);
 incube=NULL;
 
 /* Allocate memory for sumcube and sumsqcube */
 sumcube = dcubec(n3,n2,n1);
 sumsqcube = dcubec(n3,n2,n1);

 /* Loop through the chisq files */
 for (n=0;n<nfit;n++) {
   if (status = rdcube(filelist[n],NULL,NULL, &incube, &taxis1, &taxis2,
   		&taxis3,&tn1,&tn2,&tn3) ) {
	fprintf(stderr,ERRMSG_IO_ERROR,filelist[n]);
	pre_exit();
	exit(status);
   }
   for (k=0;k<n3;k++)
   for (j=0;j<n2;j++)
   for (i=0;i<n1;i++) {
     *(*(*(sumcube+k)+j)+i) += *(*(*(incube+k)+j)+i);
     *(*(*(sumsqcube+k)+j)+i) += *(*(*(incube+k)+j)+i) * *(*(*(incube+k)+j)+i);
   }
   free_dcube(incube); incube=NULL;
   free_dvector(taxis1); taxis1=NULL;
   free_dvector(taxis2); taxis2=NULL;
   free_dvector(taxis3); taxis3=NULL;   
 }
 
 outcube = dcube(n3,n2,n1);
 outstdcube = dcube(n3,n2,n1);
 for (k=0;k<n3;k++)
 for (j=0;j<n2;j++)
 for (i=0;i<n1;i++) {
   *(*(*(outcube+k)+j)+i) = *(*(*(sumcube+k)+j)+i) / (double)nfit;
   var = ( *(*(*(sumsqcube+k)+j)+i) -
   	 (1./(double)nfit) * *(*(*(sumcube+k)+j)+i) * *(*(*(sumcube+k)+j)+i) )/
	 ( (double)nfit - 1.);
   *(*(*(outstdcube+k)+j)+i) = sqrt(var);
 }
 free_dcube(sumcube); sumcube=NULL;
 free_dcube(sumsqcube); sumsqcube=NULL;
 
 if (status = wrcube(output,NULL,NULL,outcube,axis1,axis2,axis3,n1,n2,n3)) {
 	pre_exit();
	exit(status);
 }
 sprintf(line,"%s.std",output);
 if (status = wrcube(line,NULL,NULL,outstdcube,axis1,axis2,axis3,n1,n2,n3)) {
 	pre_exit();
	exit(status);
 }
 
 pre_exit();
 exit(status);
}

void pre_exit(void)
{
 if (filelist != NULL) free_svector(filelist);
 if (incube != NULL) free_dcube(incube);
 if (outcube != NULL) free_dcube(outcube);
 if (outstdcube != NULL) free_dcube(outstdcube);
 if (sumcube != NULL) free_dcube(sumcube);
 if (sumsqcube != NULL) free_dcube(sumsqcube);
 if (axis1 != NULL) free_dvector(axis1);
 if (axis2 != NULL) free_dvector(axis2);
 if (axis3 != NULL) free_dvector(axis3);
 if (taxis1 != NULL) free_dvector(taxis1);
 if (taxis2 != NULL) free_dvector(taxis2);
 if (taxis3 != NULL) free_dvector(taxis3);

 return;
}
