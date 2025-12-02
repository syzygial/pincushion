#include "intern.h"

#include <pincushion/pincushion_api.h>

int hooks_present = 0;

bool pincushion_hooks_present() {
  return hooks_present != 0;
}