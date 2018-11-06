/** \file
 * A basic C shared library.
 * This library is used to test C function calls from within LLVM.
 *
 * \author Richard Nusser
 * \copyright 2018 Richard Nusser
 * \license GPLv3 (see http://www.gnu.org/licenses/)
 * \sa https://github.com/rinusser/PimpMyCompiler
 */

#include <stdio.h>


static const char *prefix="greetings from C!";


void c_noargs()
{
  printf("%s c_noargs() says hi.\n",prefix);
}

void c_printstr(const char *str)
{
  printf("%s c_printstr() caller says '%s'\n",prefix,str);
}

int c_print_and_inc_int(const int value)
{
  printf("%s c_print_and_inc_int() got '%d' and returns %d\n",prefix,value,value+1);
  return value+1;
}
