/* Function : rd_coords */
/* Version: 0.1.0 */
/*   Read coordinate file
 *
 * int rd_coords( FILE *istream, double ***ptr_pcoords )
 *
 * Usage: status = rd_coords(istream, &pcoords);
 */

#include "./phot.h"
#include <KLutil.h>
#include <stdlib.h>	/* for atof() */

int rd_coords( FILE *istream, double ***ptr_pcoords, int *ncoords )
{
 int n,i,status=0;
 double **p_coords;
 char line[MAXLENGTH],**p_parse;

 p_coords = dmatrix(2,MAX_N_COORDS);
 n=0;
 while (fgets( line, MAXLENGTH, istream ) != NULL) {
   if (n>=MAX_N_COORDS) {
   	fprintf(stderr,"ERROR: Too many sources. Max=%d\n",MAX_N_COORDS);
	free_dmatrix(p_coords);
	return(ERRNO_INPUT_ERROR);
   }
   p_parse = svector(2,MAXLENGTH);
   splitstr(line,p_parse,SPACES);
   *(*(p_coords+0)+n) = atof(p_parse[0]);
   *(*(p_coords+1)+n) = atof(p_parse[1]);
   n++;
   free_svector(p_parse);
 }
 *ncoords = n;

 *ptr_pcoords = dmatrix(2,*ncoords);
 for (i=0;i<*ncoords;i++) {
   *(*(*ptr_pcoords + 0) + i) = *(*(p_coords+0)+i);
   *(*(*ptr_pcoords + 1) + i) = *(*(p_coords+1)+i);
 }
 free_dmatrix(p_coords);

 return(status);
}
