/* register.h */

#ifndef _REGISTER_H_
#define _REGISTER_H_

#define BINDIR "/home/klabrie/prgc/"

#define MAXARG 50

#define HELP_COMBMOSAIC \
"\nUsage: combmosaic [-vh] img1 img2 .. imgN outimg oob\n\
        combmosaic [-vh] -l imglist outimg oob\n\
  imgN          : Name of the input images\n\
  outimg        : Name of the output image\n\
  -b oob        : Maximum out of bound value.  Any pixels below that\n\
                  value will be considered out of bound.\n\
  -h|--help     : Print help page\n\
  -l imglist    : Specify the name of the file with the list of images\n\
                  [one per line]\n\
  -v|--verbose  : Toggle verbose on\n\
  --version     : Print version\n"

#endif /* _REGISTER_H_ */
