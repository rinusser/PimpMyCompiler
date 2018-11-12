/** \file
 * In-memory source code and helper functions for stage2+ code generators.
 * The data in here will be used by the generated LLVM code to produce even more LLVM code, e.g. by the stage 2 binary
 * to generate LLVM IR for stage 3.
 *
 * In real-world applications the sources would be passed in human-readable form from a file or stdin instead.
 *
 * The helper functions in here only provide user-friendly information about what's happening in the LLVM code, they
 * especially do not participate in code generation.
 *
 *
 * Syntax (temporary):
 *
 *   | ... command separator
 *   i ... ignore (does nothing)
 *   p ... print a string literal (string starts after 'p' character, ends with \0
 *   q ... quit (stops parsing)
 *
 *
 * \author Richard Nusser
 * \copyright 2018 Richard Nusser
 * \license GPLv3 (see http://www.gnu.org/licenses/)
 * \sa https://github.com/rinusser/PimpMyCompiler
 */

#include "stdio.h"


const char *source_code="pHi, I'm the first string!\0|"
                        "i|"
                        "pthe next string will be empty\0|"
                        "p\0|"
                        "i|"
                        "i|"
                        "i|"
                        "pthe end\0|"
                        "q";


void print_instruction_error(const char *input)
{
  printf("ERROR parsing source: %s\n"
         "                      ^\n",input);
}

void print_ignore()
{
  printf("got command IGNORE\n");
}

void printer(const char *input)
{
  printf("got command PRINT: '%s'\n",input);
}

void print_quit()
{
  printf("got command QUIT\n");
}
