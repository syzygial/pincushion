# pincushion
A lightweight instrumentation tool for highly configurable debugger breakpoint control 

## Usage

Pincushion can be used to add breakpoints to any application/library code compiled with ``-finstrument-functions``, which are specified in a ``.breakpoints.yaml`` file.  This can easily be done by prepending
``CFLAGS=-finstrument-functions`` to most build-system invocations, e.g.

```bash
CFLAGS=-finstrument-functions cmake ..
CFLAGS=-finstrument-functions make
```
etc.

The ``pincushion`` executable preloads the ``pincushion_hooks`` library which contains an implementation of ``__cyg_profile_func_enter`` that determines 
whether or not to create a breakpoint.  A program is run with pincushion
via ``pincushion my_program``, ``gdb --args pincushion my_program``, etc.
``libpincushion`` is an optional library that can be used to control the 
behavior of pincushion at runtime.
## Examples



```c
// example.c
void B() {
}

void A() {
  B();
}

int main() {
  A();
}
```

```yaml
# .breakpoints.yaml
example:
  - Function: main
  - Function: B

```

The above example will break on both main() and B() when run under
a debugger, e.g.

```bash
$ gcc -finstrument-functions -g -o example example.c
$ gdb --args pincushion ./example
```




### Checking if hooks are present with ``libpincushion``

```c
// example.c
#include <stdio.h>

#include <pincushion/pincushion_api.h>

int main() {
  printf("Hooks found: %s\n", pincushion_hooks_present() ? "true" : "false");
  return 0;
}
```

```bash
$ ./example.c
Hooks found: false

$ pincushion ./example.c
Hooks found: true
```

## Dependencies
  - Boost.Stacktrace
  - Yaml-Cpp

## Roadmap
  - [ ] Windows/MacOS/BSD support
  - [ ] Callstack breakpoint filter, e.g. break on A()->B()->C() but not A()->C()
  - [ ] Breakpoint conditions, e.g. only break at B() if A() has been called previously
  - [ ] CLI options for controlling behavior