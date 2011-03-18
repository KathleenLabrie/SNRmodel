/* Function : filterID */
/* Version: 0.1.0 */
/*   Convert filter code name to filter code number.
 *
 * int filterID(char name, unsigned short int *code);
 *
 * Calling filter ID
 *   status = filterID(name, &code);
 *
 * Needs:
 *   %%%libKLutil%%%
 */

#include "./redcor.h"
#include <mydef.h>	/* for error codes */
#include <stdio.h>	/* for stderr */
#include <string.h>	/* for strncmp(), strlen() */

int filterID(char name[], unsigned short int *code)
{
 int status=0;

 if (!strncmp(CFHT_REDEYEW_J_ID,name,strlen(name))) { *code = CFH5103; }
 else if (!strncmp(CFHT_REDEYEW_H_ID,name,strlen(name))) { *code = CFH5201; }
 else {
 	fprintf(stderr,"ERROR: Unknown filter (%s)\n",name);
	return(ERRNO_INPUT_ERROR);	
 }

 return(status);
}
