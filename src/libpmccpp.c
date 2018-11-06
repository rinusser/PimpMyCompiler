/** \file
 * A basic C++ shared library.
 * This library is used to test C++ function calls from within LLVM. In particular this should help figuring out how to
 * deal with C++ name mangling, class/instance properties and methods.
 *
 * \author Richard Nusser
 * \copyright 2018 Richard Nusser
 * \license GPLv3 (see http://www.gnu.org/licenses/)
 * \sa https://github.com/rinusser/PimpMyCompiler
 */

#include <stdio.h>


class CppClass
{
protected:
  static const char *prefix;

public:
  static void staticNoArgs();
};


const char *CppClass::prefix="greetings from C++!";


void CppClass::staticNoArgs()
{
  printf("%s CppClass::staticNoArgs() says hi.\n",prefix);
}



class ExtendedClass: public CppClass
{
public:
  static void staticNoArgs();
  static void staticPrintMsg(const char *msg);
};


void ExtendedClass::staticNoArgs()
{
  printf("%s ExtendedClass::staticNoArgs() says hi.\n",prefix);
}

void ExtendedClass::staticPrintMsg(const char *msg)
{
  printf("%s ExtendedClass::staticPrintMsg() caller says '%s'.\n",prefix,msg);
}
