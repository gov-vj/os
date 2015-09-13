#ifndef __myerror_
#define __myerror_
#define MYERROR(A) errno=0;\
    if(A<0) fprintf(stderr,"%s\n",strerror(errno))
#define MYERRORP(A) errno=0;\
    if(A==NULL) fprintf(stderr,"%s\n",strerror(errno))
#endif
