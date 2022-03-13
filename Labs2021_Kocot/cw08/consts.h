#define true 1
#define false 0
#define bool short

#define mode_t short
#define NUMBERS 1
#define BLOCK 2

#define MAXVAL 255

#define timeptr struct timeval *
#define usec_t unsigned long

typedef struct {
    int *data_in;
    int *data_out;
    int width;
    int height;
    int minval;
    int maxval;
    usec_t* retptr;
} numinp;

