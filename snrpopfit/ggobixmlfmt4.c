/* Function : ggobixmlfmt4 */
/* Version : 0.1.0 */
/*   Format data in XML for GGobi plotting tool.
 *
 */

#include <stdio.h>
#include <KLutil.h>
#include <string.h>
#include <stddef.h>

int ggobixmlfmt4(char filename[], double *v1, double *v2, double *v3, 
	double ***v4, char **vnames, int n1, int n2, int n3)
{
 int i,j,k,n,status=0;
 int datacount=2,varcount=4,npoints;
 int color=2,edgescount;
 char dataname[MAXLENGTH],glyph[MAXLENGTH],edgesglyph[MAXLENGTH];
 FILE *ostream=NULL;
 
 strcpy(dataname,"data cube");
 strcpy(glyph,"plus 0");
 npoints = n1*n2*n3;
 strcpy(edgesglyph,"fr 0");
 edgescount = 3*n1*n2*n3 - n2*n3 - n1*n3 - n1*n2;
 if ( (ostream = fopen(filename, "w")) == NULL ) {
 	fprintf(stderr,ERRMSG_IO_ERROR,"");
	fprintf(stderr,"\tUnable to open %s for writing.\n",filename);
	return(ERRNO_IO_ERROR);
 }
 fprintf(ostream,"<?xml version=\"1.0\"?>\n");
 fprintf(ostream,"<!DOCTYPE ggobidata SYSTEM \"ggobi.dtd\">\n\n");
 fprintf(ostream,"<ggobidata count=\"%d\">\n",datacount);
 fprintf(ostream,"<data name=\"%s\">\n",dataname);
 fprintf(ostream,"<description>\n");
 fprintf(ostream,"</description>\n");
 fprintf(ostream,"<variables count=\"%d\">\n",varcount);
 fprintf(ostream,"  <variable name=\"%s\" />\n",vnames[0]);
 fprintf(ostream,"  <variable name=\"%s\" />\n",vnames[1]);
 fprintf(ostream,"  <variable name=\"%s\" />\n",vnames[2]);
 fprintf(ostream,"  <variable name=\"%s\" />\n",vnames[3]);
 fprintf(ostream,"</variables>\n");
 fprintf(ostream,"<records count=\"%d\" color=\"%d\" glyph=\"%s\">\n",
 			npoints,color,glyph);
			
 for (n=1,k=0;k<n3;k++)
 for (j=0;j<n2;j++)
 for (i=0;i<n1;i++)
       fprintf(ostream,"<record id=\"%d\"> %g %g %g %g </record>\n",
       		n++, *(v1+i), *(v2+j), *(v3+k), *(*(*(v4+k)+j)+i));

 fprintf(ostream,"</records>\n");
 fprintf(ostream,"</data>\n");
 
 fprintf(ostream,"\n<data name=\"edges\">\n");
 fprintf(ostream,"<records count=\"%d\" glyph=\"%s\">\n",edgescount,edgesglyph);

 for (n=1,k=0;k<n3;k++)
 for (j=0;j<n2;j++)
 for (i=0;i<n1;i++) {
   if (i != n1-1) fprintf(ostream,
   	"<record source=\"%d\" destination=\"%d\" color=\"%d\"> </record>\n",
 	n,n+1,color);
   if (j != n2-1) fprintf(ostream,
   	"<record source=\"%d\" destination=\"%d\" color=\"%d\"> </record>\n",
 	n,n+(n1),color);
   if (k != n3-1) fprintf(ostream,
   	"<record source=\"%d\" destination=\"%d\" color=\"%d\"> </record>\n",
 	n,n+(n1*n2),color);
   n++;  
 }
  
 fprintf(ostream,"</records>\n");
 fprintf(ostream,"</data>\n\n");
 
 fprintf(ostream,"</ggobidata>\n");
 
 fclose(ostream);

 return(status);
}
