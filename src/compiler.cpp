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


  //XXX the findFunction() calls below don't do NULL checks for brevity's sake

  //call to c_noargs()
  Function *callee=findFunction("c_noargs");
  builder.CreateCall(callee);

  //call to c_printmsg()
  callee=findFunction("c_printstr");
  Value *str=builder.CreateGlobalStringPtr("I'm a compiler!");
  std::vector<Value*> args=std::vector<Value*>{str};
  builder.CreateCall(callee,args);

  //call to CppClass::staticNoArgs()
  builder.CreateCall(findFunction("_ZN8CppClass12staticNoArgsEv"));

  //call to ExtendedClass::staticNoArgs()
  builder.CreateCall(findFunction("_ZN13ExtendedClass12staticNoArgsEv"));

  //call to ExtendedClass::staticPrintMsg()
  callee=findFunction("_ZN13ExtendedClass14staticPrintMsgEPKc");
  builder.CreateCall(callee,args);


  //call to CppClass::printMsg()
  Value *baseInst=builder.CreateCall(findFunction("_ZN8CppClass11getInstanceEv"),None,"baseInst");
  str=builder.CreateGlobalStringPtr("I'm calling CppClass::printMsg() on a CppClass instance!");
  args=std::vector<Value*>{baseInst,str};
  builder.CreateCall(findFunction("_ZN8CppClass8printMsgEPKc"),args);

  //call to (inherited) ExtendedClass::printMsg()
  Value *extInst=builder.CreateCall(findFunction("_ZN13ExtendedClass11getInstanceEv"),None,"extInst");
  str=builder.CreateGlobalStringPtr("I'm calling CppClass::printMsg() on an ExtendedClass instance!");
  args=std::vector<Value*>{extInst,str};
  builder.CreateCall(findFunction("_ZN8CppClass8printMsgEPKc"),args);


  //end: return int 0
  builder.CreateRet(ConstantInt::get(Type::getInt32Ty(*context),0));

  verifyFunction(*func);

  return func;
}


FunctionType *getVoidCharPtrFT()
{
  std::vector<Type*> args(1,PointerType::get(Type::getInt8Ty(*context),0));
  return FunctionType::get(Type::getVoidTy(*context),args,false);
}


void addCLibraryWrappers()
{
  FunctionType *ft=FunctionType::get(Type::getVoidTy(*context),false);
  Function::Create(ft,Function::ExternalLinkage,"c_noargs",module);

  ft=getVoidCharPtrFT();
  Function::Create(ft,Function::ExternalLinkage,"c_printstr",module);
}


void addCppLibraryWrappers()
{
  FunctionType *ft=FunctionType::get(Type::getVoidTy(*context),false);
  Function::Create(ft,Function::ExternalLinkage,"_ZN8CppClass12staticNoArgsEv",module);

  Function::Create(ft,Function::ExternalLinkage,"_ZN13ExtendedClass12staticNoArgsEv",module);

  ft=getVoidCharPtrFT();
  Function::Create(ft,Function::ExternalLinkage,"_ZN13ExtendedClass14staticPrintMsgEPKc",module);

  ft=FunctionType::get(PointerType::get(Type::getInt8Ty(*context),0),false);
  Function::Create(ft,Function::ExternalLinkage,"_ZN8CppClass11getInstanceEv",module);
  Function::Create(ft,Function::ExternalLinkage,"_ZN13ExtendedClass11getInstanceEv",module);

  Type *ptr=PointerType::get(Type::getInt8Ty(*context),0);
  std::vector<Type*> args(2,ptr);
  ft=FunctionType::get(Type::getVoidTy(*context),args,false);
  Function::Create(ft,Function::ExternalLinkage,"_ZN8CppClass8printMsgEPKc",module);
}

Module *createLLVMModule()
{
  context=new LLVMContext();
  module=new Module("generated",*context);
  addCLibraryWrappers();
  addCppLibraryWrappers();
  addMainFunction();

  return module;
}
