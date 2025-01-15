#include "ish.h"

void error_handling(const char *programName, const char *message) {
    fprintf(stderr, "%s: %s\n", programName, message);
}