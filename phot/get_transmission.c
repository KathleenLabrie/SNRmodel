/* Function : get_transmission */
/* Version: 0.1.0 */
/*   Get transmission curve of a filter.
 *
 * int get_transmission(unsigned short int filter, double **ptr2lambda,
 *		double **ptr2trans, long int *n)
 *
 * Calling get_transmission:
 *  double *lambda=NULL, *trans=NULL;
 *  status = get_transmission(filter,&lambda,&trans,&n);
 *
 * Needs
 *   %%%libKLutil%%%
 *   %%%libKLfile%%%
 */

#include "./redcor.h"
#include "./phot.h"	/* for location of filter files */
#include <KLutil.h>
#include <KLfile.h>
#include <stdio.h>

#define ANGSTROM 1.e-10

int get_transmission(unsigned short int filter, double **ptr2lambda,
 		double **ptr2trans, long int *n)
{
 int status=0;
 long int i;
 double units;
 char filename[MAXLENGTH];
 
 switch (filter)
 {
   case CFH5103:
   	sprintf(filename,"%s%s%s",DEFAULT_DIR,DEFAULT_FILTER_DIR,
				  CFHT_REDEYEW_J_FILE);
	units=1.e-9;
	break;
   case CFH5201:
   	sprintf(filename,"%s%s%s",DEFAULT_DIR,DEFAULT_FILTER_DIR,
				  CFHT_REDEYEW_H_FILE);
	units=1.e-9;
	break;
   default:
   	fprintf(stderr,ERRMSG_INTERNAL_ERROR);
	return(ERRNO_INTERNAL_ERROR);
 }
 if ( status = rd_set_double(filename,ptr2lambda,ptr2trans,n) ) return(status);
 for (i=0;i<*n;i++) { *(*ptr2lambda+i) *= units/ANGSTROM; }
 
 return(status);
}
