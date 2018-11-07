/** \file
 * An LLVM wrapper handling all the otherwise difficult to implement compiler requirements.
 * Ideally, all features of this library should be replaced with LLVM implementations.
 *
 * \author Richard Nusser
 * \copyright 2018 Richard Nusser
 * \license GPLv3 (see http://www.gnu.org/licenses/)
 * \sa https://github.com/rinusser/PimpMyCompiler
 */

#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"

#include "libllvmwrapper.h" //include header file to make sure function definitions match the declarations


using namespace llvm;


Module *llvmw_create_module()
{
  LLVMContext *context=new LLVMContext();
  return new Module("generated",*context); //XXX maybe accept module name string in function parameter
}

LLVMContext *llvmw_get_module_context(Module *module)
{
  return &module->getContext();
}

IRBuilder<> *llvmw_create_builder(BasicBlock *block)
{
  return new IRBuilder<>(block);
}
