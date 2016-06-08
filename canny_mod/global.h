
#ifndef _GLOBAL_H
#define _GLOBAL_H

#define X86_PRESC 1
#define RC_PRESC  1000/25
#ifdef RC
  #define PRESC   RC_PRESC
#else
  #define PRESC   X86_PRESC
#endif 

#define C1 (float) (0.01 * 255 * 0.01  * 255)
#define C2 (float) (0.03 * 255 * 0.03  * 255)

#endif