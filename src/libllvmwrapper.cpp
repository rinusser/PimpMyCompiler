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

//make sure all exported functions are included in .h, otherwise they'll compiled with C++ naming
#include "libllvmwrapper.h"


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


/**
 * Wrapper around llvm::Function::Create().
 * This is currently used to get around the fact that libLLVM-4.0 doesn't actually export the Function::Create method, but instead
 * replaces those function calls with "new Function(...)". Object instantiation isn't straightforward in LLVM, so this method makes
 * the C++ compiler perform this operation for us.
 *
 * \param ft      LLVM function type for the function declaration
 * \param linkage LLVM linkage type, e.g. ExternalLinkage
 * \param name    function name to declare
 * \param module  LLVM module to add the function to
 * \return pointer to the declared LLVM function
 */
Function *llvmw_create_function(FunctionType *ft, GlobalValue::LinkageTypes linkage, const char *name, Module *module)
{
  return Function::Create(ft,linkage,name,module);
}
