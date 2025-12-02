#ifndef PINCUSHION_H
#define PINCUSHION_H

#ifdef __cplusplus
extern "C" {
#else
#include <stdbool.h>
#endif

bool pincushion_hooks_present();  
int pincushion_dump_filters();


#ifdef __cplusplus
}
#endif

#endif