/** \file
 * A simple LLVM-based compiler / code generator.
 *
 * \author Richard Nusser
 * \copyright 2018 Richard Nusser
 * \license GPLv3 (see http://www.gnu.org/licenses/)
 * \sa https://github.com/rinusser/PimpMyCompiler
 */

#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Verifier.h"
#include "llvm/IR/IRBuilder.h"

using namespace llvm;


Module *createLLVMModule();
LLVMContext *context;
Module *module;


int main(int argc, char **argv)
{
  module=createLLVMModule();
  verifyModule(*module);
  module->print(errs(),NULL);

  return 0;
}


Function *findFunction(const char *name)
{
  Function *callee=module->getFunction(name);
  if(!callee)
    fprintf(stderr,"error: could not find function '%s'\n",name);
  return callee;
}


Function *addMainFunction()
{
  IntegerType *int32=Type::getInt32Ty(*context);

  Constant *c=module->getOrInsertFunction("main",int32,NULL);
  Function *func=cast<Function>(c);
  func->setCallingConv(CallingConv::C);

  BasicBlock *block=BasicBlock::Create(*context,"entry",func);
  IRBuilder<> builder(block);

  //call to c_noargs()
  Function *callee=findFunction("c_noargs");
  if(!callee)
    return NULL;
  builder.CreateCall(callee);

  //call to c_printmsg()
  callee=findFunction("c_printstr");
  if(!callee)
    return NULL;
  Value *str=builder.CreateGlobalStringPtr("I'm a compiler!");
  std::vector<Value*> args=std::vector<Value*>{str};
  builder.CreateCall(callee,args);

  builder.CreateRet(ConstantInt::get(Type::getInt32Ty(*context),0));

  verifyFunction(*func);

  return func;
}


void addCLibraryWrappers()
{
  FunctionType *ft=FunctionType::get(Type::getVoidTy(*context),false);
  Function::Create(ft,Function::ExternalLinkage,"c_noargs",module);

  std::vector<Type*> args(1,PointerType::get(Type::getInt8Ty(*context),0));
  ft=FunctionType::get(Type::getVoidTy(*context),args,false);
  Function::Create(ft,Function::ExternalLinkage,"c_printstr",module);
}


Module *createLLVMModule()
{
  context=new LLVMContext();
  module=new Module("generated",*context);
  addCLibraryWrappers();
  addMainFunction();

  return module;
}
