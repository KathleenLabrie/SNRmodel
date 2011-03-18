/* Function: mag */
/* Version: 0.1.0 */
/*   Calculate magnitude and flux per seconds.
 *
 * int mag( double **pcoo, int idcoo, PHOTPARS ppars, DATAPARS dpars, RESULTS *res )
 *
 * Usage: status = mag( pcoords, idcoo, photpars, datapars, &results );
 *
 * Needs:
 */

#include "./phot.h"
#include <math.h>

int mag( double **pcoo, int idcoo, PHOTPARS ppars, DATAPARS dpars, RESULTS *res )
{
 int i,status=0;

 /* loop through radii */
 for (i=0;i<res->res_nradii;i++) {
   res->res_fluxsec[i][idcoo] = res->res_totalflux[i][idcoo] / 
   				dpars.data_itime;
   res->res_mag[i][idcoo] = ppars.phot_zmag - 
   			    2.5*log10(res->res_fluxsec[i][idcoo]);
   res->res_merr[i][idcoo] = (2.5/log(10.)) * res->res_error[i][idcoo] /
   				res->res_totalflux[i][idcoo];
 }

 return(status);
}
