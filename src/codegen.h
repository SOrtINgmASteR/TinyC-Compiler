#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdio.h>
#include "tac.h"

void codegen_generate(const TacInstr *head, FILE *out);

#endif
