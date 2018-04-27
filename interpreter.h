// launch interpreter with given code
// kill it after 2 seconds
// if killed, return 1
// else 0
// change *standard_output and *standard_error (allocate memory) if not NULL
#ifndef __EMSCRIPTEN__
int launch(char *code, char **standard_output, char **standard_error);
#else
void launch_emscripten(char *code, char **standard_output, char **standard_error, signed char *result);
#endif
