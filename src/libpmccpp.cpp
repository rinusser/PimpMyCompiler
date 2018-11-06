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
  virtual const char *getInstancePrefix();

public:
  static CppClass *getInstance();
  static void staticNoArgs();
  void printMsg(const char *msg);
};


const char *CppClass::prefix="greetings from C++!";

const char *CppClass::getInstancePrefix()
{
  return "CppClass";
}


CppClass *CppClass::getInstance()
{
  return new CppClass();
}

void CppClass::staticNoArgs()
{
  printf("%s CppClass::staticNoArgs() says hi.\n",prefix);
}

void CppClass::printMsg(const char *msg)
{
  printf("%s %s::printMsg() caller says '%s'.\n",prefix,getInstancePrefix(),msg);
}



class ExtendedClass: public CppClass
{
protected:
  const char *getInstancePrefix();

public:
  static ExtendedClass *getInstance();
  static void staticNoArgs();
  static void staticPrintMsg(const char *msg);
};


const char *ExtendedClass::getInstancePrefix()
{
  return "ExtendedClass";
}


ExtendedClass *ExtendedClass::getInstance()
{
  return new ExtendedClass();
}

void ExtendedClass::staticNoArgs()
{
  printf("%s ExtendedClass::staticNoArgs() says hi.\n",prefix);
}

void ExtendedClass::staticPrintMsg(const char *msg)
{
  printf("%s ExtendedClass::staticPrintMsg() caller says '%s'.\n",prefix,msg);
}
