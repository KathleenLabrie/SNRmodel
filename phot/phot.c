/* plot.c */

#include "./phot.h"
#include <KLutil.h>

void alloc_results( RESULTS *strct )
/* allocate memory for the various arrays in a RESULTS structure */
{
 strct->res_serr = ivector(strct->res_ncoo);
 strct->res_nskypix = ivectorc(strct->res_ncoo);
 strct->res_skyperpix = dvector(strct->res_ncoo);
 strct->res_skystddev = dvector(strct->res_ncoo);
 strct->res_npix =imatrixc(strct->res_nradii,strct->res_ncoo);
 strct->res_totalflux = dmatrixc(strct->res_nradii,strct->res_ncoo);
 strct->res_error = dmatrix(strct->res_nradii,strct->res_ncoo);
 strct->res_radii = dvector(strct->res_nradii);
 strct->res_fluxsec = dmatrix(strct->res_nradii,strct->res_ncoo);
 strct->res_mag = dmatrix(strct->res_nradii,strct->res_ncoo);
 strct->res_merr = dmatrix(strct->res_nradii,strct->res_ncoo);
 return;
}

void free_results( RESULTS *strct )
/* free a RESULTS struct allocated with alloc_results() */
{
 free_ivector(strct->res_serr);
 free_ivector(strct->res_nskypix);
 free_dvector(strct->res_skyperpix);
 free_dvector(strct->res_skystddev);
 free_imatrix(strct->res_npix);
 free_dmatrix(strct->res_totalflux);
 free_dmatrix(strct->res_error);
 free_dvector(strct->res_radii);
 free_dmatrix(strct->res_fluxsec);
 free_dmatrix(strct->res_mag);
 free_dmatrix(strct->res_merr);
 return;
}
