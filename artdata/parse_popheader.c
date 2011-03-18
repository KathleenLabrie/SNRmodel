/* Function : parse_popheader */
/* Version : 0.1.0 */
/*   Put pop header info in a KLFITS_HEADER structure
 *
 * int parse_popheader(char line[], KLFITS_HEADER *h);
 *
 * Calling parse_popheader
 *    KLFITS_HEADER *h;
 *    status = parse_popheader(line, h);
 *
 * Needs:
 *   %%%libKLutil%%%
 *   %%%libKLcfitsio%%%
 */

#include <KLutil.h>
#include <KLcfitsio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>

int parse_popheader(char line[], KLFITS_HEADER *h)
{
 int n=0, status=0;
 char **p_parse,**p_tmp,*pstr1,*pstr2;
 
 p_parse=svector(2,MAXLENGTH);
 n=splitstr(line,p_parse,"=");
 if (n != 2) { 
    h->datatype=0;
    return(status);
 } 
 pstr1 = *(p_parse+0) + 1;
 pstr2 = *(p_parse+1) + 1;
 
 p_tmp=svector(2,MAXLENGTH);
 splitstr(pstr1,p_tmp,SPACES);
 strcpy(h->keyword,p_tmp[0]);
 free_svector(p_tmp);
 
 p_tmp=svector(2,MAXLENGTH);
 splitstr(pstr2,p_tmp,SPACES);
 if (( *(pstr2+0) >= 48 ) && ( *(pstr2+0) <= 57 )) { /* Numerical */
    h->datatype=TFLOAT;
    h->value_double=atof(p_tmp[0]);
    strcpy(h->comment,"");
 } else {	/* string */
    h->datatype=TSTRING;
    strcpy(h->value_str,p_tmp[0]);
    strcpy(h->comment,"");
 }
 free_svector(p_tmp);
 
 free_svector(p_parse);
 return(status);
}
