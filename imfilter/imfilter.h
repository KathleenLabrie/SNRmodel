/* imfilter.h */

#ifndef _IMFILTER_H_
#define _IMFILTER_H_

#include <KLutil.h>		/* for CLIP */

#define BOXCAR 1	/* 1 << 0 */

#define MAXPARAM 20

typedef struct
{
 double vbox_xmin, vbox_xmax;
 double vbox_ymin, vbox_ymax;
} VBOX;

typedef struct
{
 VBOX bx_box;
 CLIP bx_clip;
 char bx_doclip[MAXLENGTH];
 char bx_mask[MAXLENGTH];
} TBOXCAR;


int boxcar( double **ppix, double **pmask, unsigned long naxes[], 
                 VBOX *vbpar, CLIP *cpar, double ***pnew );
int dobx(char image[], char new_image[], TBOXCAR *param, char logfile[]);
int initbx( char file[], TBOXCAR *param );

#endif		/* _IMFILTER_H_ */
