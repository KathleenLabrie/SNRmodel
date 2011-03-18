/* Main : plotsurf */

#include <KLutil.h>
#include <nrutil.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>	/* for NULL */
#include <stdio.h>
#include <cpgplot.h>

#define PGPLOTPKG 0
#define PGXTALPKG 1
#define GGOBI 2

void pgplot2d_(float *arraybuf, int *kx, int *ny, float *size, float *angle);
void plot_(float *x, int *nx, float *y, int *ny, float *zbuf, int *n1, int *n2,
	float *w, float *size, int *iwidth, char xlbl[], char ylbl[],
	char title[]);
int ggobixmlfmt4(char filename[], double *v1, double *v2, double *v3, 
	double ***v4, char **vnames, int n1, int n2, int n3);

void testf_(float *a, int *nx, int *ny);

main(argc,argv)
int argc;
char *argv[];
{
 int PLOTPKG=0,status=0;
 long int i,j,k;
 long int n1,n2,n3;
 int nx,ny,ndat;
 long int slct_axis=0,slct_slice;
 double ***data=NULL,*axis1=NULL,*axis2=NULL,*axis3=NULL;
 float *x=NULL,*y=NULL,**z=NULL,*w=NULL;
 float *zbuf=NULL;
 float size,angle;
 int iwidth;
 char filename[MAXLENGTH];
 char *xlbl,*ylbl,*title;
 char **axis_names;
 
 int pgID_main;

 strcpy(filename,argv[1]);
 if (argc == 2) { PLOTPKG = GGOBI; }
 else { PLOTPKG = atoi(argv[2]); }
 if (PLOTPKG != GGOBI) {
   slct_axis = atoi(argv[3]);
   slct_slice = atoi(argv[4]);
   slct_slice--;
 }
 if (status = rdcube(filename,NULL, NULL, &data, &axis1, &axis2, &axis3, 
 		&n1, &n2, &n3) ) {
	fprintf(stderr,ERRMSG_IO_ERROR,filename);
	exit(status);
 }

 switch (slct_axis)
 {
   case  0:
   	break;
   case  1: 
   	nx=(int)n2;
	ny=(int)n3;
	ndat=nx;
	x = vector(nx);
	y = vector(ny);
	z = matrix(ndat,ndat);
	w = vector(nx);
	for (k=0;k<n3;k++) {
           *(y+k) = (float)*(axis3+k);
           for (j=0;j<n2;j++) *(*(z+k)+j) = (float)*(*(*(data+k)+j)+slct_slice);
        }	    
	for (j=0;j<n2;j++) *(x+j) = (float)*(axis2+j);
	break;
   case  2: 
   	nx=(int)n1;
	ny=(int)n3;
	ndat=nx;
	x = vector(nx);
	y = vector(ny);
	z = matrix(ndat,ndat);
	w = vector(nx);
   	for (k=0;k<n3;k++) {
           *(y+k) = (float)*(axis3+k);
           for (i=0;i<n1;i++) *(*(z+k)+i) = (float)*(*(*(data+k)+slct_slice)+i);
        }	    
	for (i=0;i<n1;i++) *(x+i) = (float)*(axis1+i);
	break;
   case  3: 
   	nx=(int)n1;
	ny=(int)n2;
	ndat=nx;
	x = vector(nx);
	y = vector(ny);
	z = matrix(ndat,ndat);
	w = vector(nx);
   	for (j=0;j<n2;j++) {
           *(y+j) = (float)*(axis2+j);
           for (i=0;i<n1;i++) *(*(z+j)+i) = (float)*(*(*(data+slct_slice)+j)+i);
        }
	for (i=0;i<n1;i++) *(x+i) = (float)*(axis1+i);
	break;
   default: fprintf(stderr,ERRMSG_INPUT_ERROR,slct_axis);
            fprintf(stderr,"Unknown axis\n");
	    exit(ERRNO_INPUT_ERROR);
 }

 switch (PLOTPKG)
 {
   case PGPLOTPKG :
	zbuf=vector(nx*ny);
	for (j=0;j<(long int)ny;j++) 
	  for (i=0;i<(long int)nx;i++) *(zbuf+(j*nx+i)) = *(*(z+j)+i); 
 	size=1.0;
 	angle=25.0;
 	if (( pgID_main = cpgopen("/xwin") ) < 1) {
 		fprintf(stderr,ERRMSG_INTERNAL_ERROR);
 		fprintf(stderr,"Unable to open plotting device.\n");
		exit(ERRNO_INTERNAL_ERROR);
 	}
 	cpgask(1);
 	cpgslct(pgID_main);
 	cpgenv(0.,size,0.,size,1,-2);
 	freddy_(zbuf,&ny,&nx,&size,&angle);
 	cpgclos();
	break;
   case PGXTALPKG :
	zbuf=vector(nx*ny);
	for (j=0;j<(long int)ny;j++) 
	  for (i=0;i<(long int)nx;i++) *(zbuf+(j*nx+i)) = *(*(z+j)+i); 
   	size=1.5;
	iwidth = 1;
	xlbl = (char *) malloc((size_t)(8*sizeof (char)));
	ylbl = (char *) malloc((size_t)(8*sizeof (char)));
	title = (char *) malloc((size_t)(6*sizeof (char)));
	strcpy(xlbl,"X-label");
	strcpy(ylbl,"Y-label");
	strcpy(title,"title");
   	plot_(x,&nx,y,&ny,zbuf,&ndat,&ndat,w,&size,&iwidth,xlbl,ylbl,title);
	break;
   case GGOBI :
   	axis_names = svector(4,MAXLENGTH);
	strcpy(axis_names[0],"NSNR");
	strcpy(axis_names[1],"snrate");
	strcpy(axis_names[2],"nambient");
	strcpy(axis_names[3],"ChiSq");
	status = ggobixmlfmt4("ggobidata.xml",axis1,axis2,axis3,data,
	  axis_names,n1,n2,n3);
	system("/usr/local/src/ggobi/ggobi/bin/ggobi ggobidata &");
	break;
   default :
   	fprintf(stderr,ERRMSG_INPUT_ERROR,"Unknown plot package");
	exit(ERRNO_INPUT_ERROR);
 }

 free_vector(w);

 exit(status);
}
