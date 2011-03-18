#include <KLutil.h>
#include <KLcfitsio.h>
#include <stddef.h>		/* for NULL */
#include <string.h>		/* for gcc4.0, strcpy() */
#include <math.h>
#include <float.h>		/* for DBL_EPSILON */

main( argc, argv )
int argc;
char *argv[];
{
 int status=0;
 long int ii,jj;
 long int naxes[2];
 double **ppix=NULL;
 double x0,y0,r,e,fpp,fpbkg;
 double dr2,dx,dy,theta;
 double r2,a2,b2,pa,top,alpha;
 char imname[80];
 KLFITS_HEADER *imheader;
 int nheader=2,npix=0;

 naxes[0]=naxes[1]=50;
 ppix = dmatrixc(naxes[1],naxes[0]);
 imheader = klfits_header_vector(nheader);
 
 strcpy(imname,"!fakeimg.fits");
 strcpy(imheader[0].keyword, "TITLE" );
 strcpy(imheader[0].value_str, "Fake image");
 strcpy(imheader[0].comment,"");
 imheader[0].datatype = TSTRING;
 x0=y0=23.5;
 pa = 125.;
 r = 10.;
 e = 0.5;
 fpp = 2.;
 fpbkg = 1.;
 
 a2 = r * r;
 b2 = a2 * (1 - e);
 top = a2 * b2;
 for (jj=0; jj<naxes[1]; jj++) {
   for (ii=0; ii<naxes[0]; ii++) {
     dx = ii-x0;
     dy = jj-y0;
     theta = atan2(dy,dx);
     dr2 = dx*dx + dy*dy;
     alpha = pa*PI/180.-theta;
     r2 = top / (a2*cos(alpha)*cos(alpha) + b2*sin(alpha)*sin(alpha));
     if (dr2<=r2+DBL_EPSILON) {
       *(*(ppix+jj)+ii) = fpp;
       npix++;
     }
     else {
       *(*(ppix+jj)+ii) = fpbkg;
     }
   }
 }
 strcpy(imheader[1].keyword, "NPIX" );
 imheader[1].value_lint=npix;
 strcpy(imheader[1].comment,"");
 imheader[1].datatype = TINT;
 
 status = create_image( imname, ppix, naxes, imheader, nheader );

 exit(0);
}
