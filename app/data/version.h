#ifndef VERSION_H
#define VERSION_H

#include "build_env.h"

#define CAT4_NX(A, B, C, D)         A ## . ## B ## . ## C ## . ## D
#define VER_CAT4(A, B, C, D)        CAT4_NX(A, B, C, D)

#define STRINGIZE_NX(A)             #A
#define STRINGIZE(A)                STRINGIZE_NX(A)

#define VER_FILE_STRING_EXTRACT(A,B,C,D)    STRINGIZE(VER_CAT4(A,B,C,D))

#define VER_FILEVERSION                     1,2,0,0
#define VER_FILE_STRING                     VER_FILE_STRING_EXTRACT(VER_FILEVERSION)

#define VER_PRODUCTVERSION                  1,2,1,VER_GIT_COMMITID_STR
#define VER_PRODUCTVERSION_STR              VER_FILE_STRING_EXTRACT(VER_PRODUCTVERSION)


#endif // VERSION_H
