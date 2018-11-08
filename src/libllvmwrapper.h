/** \file
 * Header file for LLVM wrapper library.
 * This file helps testing the wrapper library in a C++ context.
 *
 * \author Richard Nusser
 * \copyright 2018 Richard Nusser
 * \license GPLv3 (see http://www.gnu.org/licenses/)
 * \sa https://github.com/rinusser/PimpMyCompiler
 */

#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"

using namespace llvm;


extern "C" {


Module *llvmw_create_module();
LLVMContext *llvmw_get_module_context(Module *module);
IRBuilder<> *llvmw_create_builder(BasicBlock *block);
Function *llvmw_create_function(FunctionType *ft, GlobalValue::LinkageTypes linkage, const char *name, Module *module);


} //extern "C"
