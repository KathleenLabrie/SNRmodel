/* Function : artimg_seeing */
/* Version : 0.1.0 */
/*    Convolve with PSF.
 *
 * int artimg_seeing(ARTIMGPARS pars, double **ppix);
 *
 * Calling artimg_seeing
 *   status = artimg_seeing(pars, ppix);
 */

#include "./artdata.h"
#include <KLutil.h>
#include <KLfunc.h>	/* for moffat */
#include <KLfft.h>	/* for drlft3 */
#include <stddef.h>	/* for NULL */
#include <math.h>

static void pre_return(void);

/* File Scope variables */
static double **ppsf=NULL;
static double **points=NULL,*values=NULL,*coeff=NULL;

int artimg_seeing(ARTIMGPARS pars, double **ppix)
{
 int status=0;
 long int ii,jj,kk,ll,j;
 long int img_naxes[2], psf_naxes[2], fft_naxes[2], npoints=0;
 double norm;
 double ***datap=NULL,***datar=NULL,**speqp=NULL,**speqr=NULL;
 double fac,*sp1,*sp2,r,i;
 void (*pfunc)();

 img_naxes[0]=pars.artimg_naxes[0];
 img_naxes[1]=pars.artimg_naxes[1];
 psf_naxes[0]=psf_naxes[1]= (long int)(pars.artimg_psfwidth*
 				(pars.artimg_seeing/pars.artimg_pixscale));
 if (psf_naxes[0]%2 == 0) { psf_naxes[0]++; } /* psf_naxes must be odd */
 if (psf_naxes[1]%2 == 0) { psf_naxes[1]++; }
 fft_naxes[0]=fft_naxes[1]=
 	pow(2.,ceil(log10(img_naxes[0]+(psf_naxes[0]/2))/log10(2.)));

 /* Get response function, i.e. PSF */
 ppsf = dmatrixc(psf_naxes[1],psf_naxes[0]);
 npoints = psf_naxes[1]*psf_naxes[0];
 points = dmatrix(2,npoints);
 values = dvectorc(npoints);
 for (j=0,jj=0;jj<psf_naxes[1];jj++)
   for (ii=0;ii<psf_naxes[0];ii++) {
     *(*(points+0)+j) = (double)ii;
     *(*(points+1)+j) = (double)jj;
     j++;
   }
 if ( !strncmp(pars.artimg_psf,"moffat",6) ) {
    pfunc = fmoffatS;
    coeff = dvector(NCOEFF_MOFFAT_SURFACE);
    coeff[0]=0.;
    coeff[1]=1.;
    coeff[2]=psf_naxes[0]/2;
    coeff[3]=psf_naxes[1]/2;
    coeff[4]=-2.5;
    coeff[5]=pars.artimg_seeing/pars.artimg_pixscale;
    coeff[6]=pars.artimg_seeing/pars.artimg_pixscale;
    if (status = evalfunc(points,values,2,npoints,coeff,NCOEFF_MOFFAT_SURFACE,
 		   pfunc)) {
 	   fprintf(stderr,ERRMSG_INTERNAL_ERROR);
	   pre_return();
	   return(ERRNO_INTERNAL_ERROR);
    }
 } else {
    fprintf(stderr,"ERROR: unrecognized PSF\n");
    fprintf(stderr,ERRMSG_INPUT_ERROR,pars.artimg_psf);
    pre_return();
    return(ERRNO_INPUT_ERROR);
 }
 for (norm=0,j=0;j<npoints;j++) norm += *(values+j);
 for (j=0;j<npoints;j++)
   *(*(ppsf+(long int)points[1][j])+(long int)points[0][j]) = *(values+j)/norm;

 /* Free some memory */
 free_dmatrix(points);
 free_dvector(values);
 free_dvector(coeff);
 points=NULL;
 values=coeff=NULL;
  
 /* Copy image and response function to dcubes, and add zero-padding 
  * Arrange responce function in wrap-around format */
 datap = dcubec(1,fft_naxes[1],fft_naxes[0]);
 datar = dcubec(1,fft_naxes[1],fft_naxes[0]);
 speqp = dmatrix(1,2*fft_naxes[1]);
 speqr = dmatrix(1,2*fft_naxes[1]);
 
 for (jj=0;jj<img_naxes[1];jj++)	/* image */
   for (ii=0;ii<img_naxes[0];ii++)
     *(*(*(datap+0)+jj)+ii) = *(*(ppix+jj)+ii);

 for (jj=0,ll=-(int)(psf_naxes[1]/2);jj<psf_naxes[1];jj++,ll++)  /* response */
   for (ii=0,kk=-(int)(psf_naxes[0]/2);ii<psf_naxes[0];ii++,kk++) {
     if ((ll<0) && (kk<0)) 
     	*(*(*(datar+0)+fft_naxes[1]+ll)+fft_naxes[0]+kk) = *(*(ppsf+jj)+ii);
     else if (ll<0)
     	*(*(*(datar+0)+fft_naxes[1]+ll)+kk) = *(*(ppsf+jj)+ii);
     else if (kk<0)
     	*(*(*(datar+0)+ll)+fft_naxes[0]+kk) = *(*(ppsf+jj)+ii);
     else
     	*(*(*(datar+0)+ll)+kk) = *(*(ppsf+jj)+ii);
   }
   
 /* Free some memory */
 free_dmatrix(ppsf);
 ppsf=NULL;
 
 /* Calculate Fourier transforms of image and response */
 drlft3(datap,speqp,1,fft_naxes[1],fft_naxes[0],1);
 drlft3(datar,speqr,1,fft_naxes[1],fft_naxes[0],1);
  
 /* Convolve, i.e. multiply Fourier transforms */
 fac=2.0/(1.*fft_naxes[1]*fft_naxes[0]);
 sp1 = &datap[0][0][0];
 sp2 = &datar[0][0][0];
 for (j=0; j<1*fft_naxes[1]*fft_naxes[0]/2; j++) {
   r = sp1[0]*sp2[0] - sp1[1]*sp2[1];
   i = sp1[0]*sp2[1] + sp1[1]*sp2[0];
   sp1[0] = fac*r;
   sp1[1] = fac*i;
   sp1 += 2;
   sp2 += 2;
 }
 sp1 = &speqp[0][0];
 sp2 = &speqr[0][0];
 for (j=0; j<1*fft_naxes[1]; j++) {
   r = sp1[0]*sp2[0] - sp1[1]*sp2[1];
   i = sp1[0]*sp2[1] + sp1[1]*sp2[0];
   sp1[0] = fac*r;
   sp1[1] = fac*i;
   sp1 += 2;
   sp2 += 2;
 }
 
 /* Inverse Fourier transform of convolved image */
 drlft3(datap,speqp,1,fft_naxes[1],fft_naxes[0],-1);
 
 /* Copy convolved image to original array and remove zero-padding */
 for (jj=0;jj<img_naxes[1];jj++)
   for (ii=0;ii<img_naxes[0];ii++)
     *(*(ppix+jj)+ii) = *(*(*(datap+0)+jj)+ii);

 free_dcube(datap);
 free_dcube(datar);
 free_dmatrix(speqp);
 free_dmatrix(speqr);
 
 pre_return();
 return(status);
}

void pre_return(void)
{
 if (ppsf != NULL)	{ free_dmatrix(ppsf); }
 if (points != NULL)	{ free_dmatrix(points); }
 if (values != NULL)	{ free_dvector(values); }
 if (coeff != NULL)	{ free_dvector(coeff); }
 
 return;
}
