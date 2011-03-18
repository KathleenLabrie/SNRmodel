/* Function: initbx */
/* Version: 0.1.0 */
/*   Initialize boxcar parameters */

#include "./imfilter.h"
#include <KLutil.h>
#include <string.h>
#include <stdlib.h>	/* for atof() */
#include <stdio.h>

int initbx(char file[],TBOXCAR *param)
{
 int FOUND;
 int nother=0,i,j;
 char **other_param, **other_value;
 char line[MAXLENGTH], **p_parse;
 FILE *istream;

 other_param = svector(MAXPARAM,MAXLENGTH);
 other_value = svector(MAXPARAM,MAXLENGTH);

 istream = fopen(file,"r");
 while (fgets(line, MAXLENGTH, istream) != NULL) {
   delspace(line);
   if (!strncmp( line, "boxcar.", 7)) {
     p_parse = svector(2,MAXLENGTH);
     splitstr( line, p_parse, "=" );		/* also gets rid of newline character */
     if (!strncmp( p_parse[0], "boxcar.mask", strlen(p_parse[0]))) {
       strcpy(param->bx_mask,p_parse[1]);
     }
     else if (!strncmp( p_parse[0], "boxcar.xboxmin", strlen(p_parse[0]))) {
       param->bx_box.vbox_xmin = atof(p_parse[1]);
     }
     else if (!strncmp( p_parse[0], "boxcar.xboxmax", strlen(p_parse[0]))) {
       param->bx_box.vbox_xmax = atof(p_parse[1]);
     }
     else if (!strncmp( p_parse[0], "boxcar.yboxmin", strlen(p_parse[0]))) {
       param->bx_box.vbox_ymin = atof(p_parse[1]);
     }
     else if (!strncmp( p_parse[0], "boxcar.yboxmax", strlen(p_parse[0]))) {
       param->bx_box.vbox_ymax = atof(p_parse[1]);
     }
     else if (!strncmp( p_parse[0], "boxcar.clip", strlen(p_parse[0]))) {
       strcpy(param->bx_doclip,p_parse[1]);
     }
     else {
       strcpy(other_param[nother],p_parse[0]);
       strcpy(other_value[nother++],p_parse[1]);
     }
     free_svector(p_parse);
   }
 }
 fclose(istream);

 if (!strncmp(param->bx_doclip,"yes",strlen(param->bx_doclip))) {
   i=0;
   FOUND=FALSE;
   while (!FOUND && i < nother) {
     if (!strncmp(other_param[i], "boxcar.niter", strlen(other_param[i]))) {
       clipalloc(atoi(other_value[i]), &param->bx_clip);
       param->bx_clip.clip_niter = atoi(other_value[i]);
	FOUND = TRUE;
     }
     i++;
   }
   if (!FOUND) {
     printf("ERROR: 'boxcar.niter' has not been defined.\n");
     free_clip(&param->bx_clip);
     return(1);
   }
   for (i=0;i<nother;i++) {
     if (!strncmp( other_param[i], "boxcar.niter", strlen(other_param[i]))) {
       continue;
     }
     else if (!strncmp( other_param[i], "boxcar.tclip", strlen(other_param[i]))) {
       p_parse=svector(param->bx_clip.clip_niter,MAXLENGTH);
	splitstr(other_value[i], p_parse, ",");
	/*translate type to int*/
	if (gettclip(p_parse, param->bx_clip.clip_tclip, param->bx_clip.clip_niter)) {
	  free_clip(&param->bx_clip);
	  free_svector(p_parse);
	  return(1);
	}
	free_svector(p_parse);
     }
     else if (!strncmp( other_param[i], "boxcar.xsigbox", strlen(other_param[i]))) {
	p_parse=svector(param->bx_clip.clip_niter,MAXLENGTH);
	splitstr(other_value[i],p_parse, ",");
	for (j=0; j<param->bx_clip.clip_niter; j++) {
	  param->bx_clip.clip_sigbox[j][0] = atoi(p_parse[j]);
	}
	free_svector(p_parse);
     }
     else if (!strncmp( other_param[i], "boxcar.ysigbox", strlen(other_param[i]))) {
	p_parse=svector(param->bx_clip.clip_niter,MAXLENGTH);
	splitstr(other_value[i],p_parse, ",");
	for (j=0; j<param->bx_clip.clip_niter; j++) {
	  param->bx_clip.clip_sigbox[j][1] = atoi(p_parse[j]);
	}
	free_svector(p_parse);
     }
     else if (!strncmp( other_param[i], "boxcar.lsigma", strlen(other_param[i]))) {
	p_parse=svector(param->bx_clip.clip_niter,MAXLENGTH);
	splitstr(other_value[i],p_parse, ",");
	for (j=0; j<param->bx_clip.clip_niter; j++) {
	  param->bx_clip.clip_lsigma[j] = atof(p_parse[j]);
	}
	free_svector(p_parse);
     }
     else if (!strncmp( other_param[i], "boxcar.hsigma", strlen(other_param[i]))) {
	p_parse=svector(param->bx_clip.clip_niter,MAXLENGTH);
	splitstr(other_value[i],p_parse, ",");
	for (j=0; j<param->bx_clip.clip_niter; j++) {
	  param->bx_clip.clip_hsigma[j] = atof(p_parse[j]);
	}
	free_svector(p_parse);
     }
     else if (!strncmp( other_param[i], "boxcar.rnoise", strlen(other_param[i]))) {
       param->bx_clip.clip_chip.ccd_rnoise = atof(other_value[i]);
     }
     else if (!strncmp( other_param[i], "boxcar.gain", strlen(other_param[i]))) {
       param->bx_clip.clip_chip.ccd_gain = atof(other_value[i]);
     }
     else if (!strncmp( other_param[i], "boxcar.snoise", strlen(other_param[i]))) {
       param->bx_clip.clip_chip.ccd_snoise = atof(other_value[i]);
     }
     else {
       printf("ERROR: Unknown parameter (%s).\n",other_param[i]);
	free_clip(&param->bx_clip);
	return(1);
     }
   }
 }
 return(0);
}
