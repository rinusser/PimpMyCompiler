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

#include "libllvmwrapper.h" //use wrapper lib here too to make sure it'll work from within generated code


using namespace llvm;


LLVMContext *context;
Module *module;


Function *findFunction(const char *name)
{
  Function *callee=module->getFunction(name);
  if(!callee)
    fprintf(stderr,"error: could not find function '%s'\n",name);
  return callee;
}


void addMainFunction()
{
  IntegerType *int32=Type::getInt32Ty(*context);

  Constant *c=module->getOrInsertFunction("main",int32,NULL);
  Function *func=cast<Function>(c);
  func->setCallingConv(CallingConv::C);

  BasicBlock *block=BasicBlock::Create(*context,"entry",func);
  IRBuilder<> *builder=llvmw_create_builder(block);

  Function *callee=findFunction("test_calls");
  builder->CreateCall(callee);

  callee=findFunction("compiler_demo");
  builder->CreateCall(callee);

  builder->CreateRet(ConstantInt::get(Type::getInt32Ty(*context),0));

  verifyFunction(*func);
}


void addCompilerDemo()
{
  Constant *c=module->getOrInsertFunction("compiler_demo",Type::getVoidTy(*context),NULL);
  Function *func=cast<Function>(c);
  func->setCallingConv(CallingConv::C);

  BasicBlock *block=BasicBlock::Create(*context,"entry",func);
  IRBuilder<> *builder=llvmw_create_builder(block);

  //helper types/values
  PointerType *type_i8ptr=PointerType::get(Type::getInt8Ty(*context),0);
  Type *type_i1=Type::getInt1Ty(*context);
  Type *type_void=Type::getVoidTy(*context);

  FunctionType *ft_i8ptr=FunctionType::get(type_i8ptr,false);

  Value *val_null=ConstantPointerNull::get(type_i8ptr);
  Value *val_false=ConstantInt::getFalse(*context);


  //register external llvmw_create_module()
  Function::Create(ft_i8ptr,Function::ExternalLinkage,"llvmw_create_module",module);

  //Module *module=llvmw_create_module()
  Value *var_module=builder->CreateCall(findFunction("llvmw_create_module"),None,"module");

  //LLVMContext *context=llvmw_get_module_context(module)

  //verifyModule(*module,NULL,NULL);

  //register errs()
  Function::Create(ft_i8ptr,Function::ExternalLinkage,"_ZN4llvm4errsEv",module);

  //stderr=errs()
  Value *var_stderr=builder->CreateCall(findFunction("_ZN4llvm4errsEv"));

  //register Module::print()
  std::vector<Type*> args=std::vector<Type*>{type_i8ptr,type_i8ptr,type_i8ptr,type_i1,type_i1};
  FunctionType *ft=FunctionType::get(type_void,args,false);
  Function::Create(ft,Function::ExternalLinkage,"_ZNK4llvm6Module5printERNS_11raw_ostreamEPNS_24AssemblyAnnotationWriterEbb",module);

  //module->print(errs(),NULL,false,false);
  Function *callee=findFunction("_ZNK4llvm6Module5printERNS_11raw_ostreamEPNS_24AssemblyAnnotationWriterEbb");
  std::vector<Value*> args2=std::vector<Value*>{var_module,var_stderr,val_null,val_false,val_false};
  builder->CreateCall(callee,args2);


  //end: return
  builder->CreateRet(NULL);
  verifyFunction(*func);
}


void addTestCallsFunction()
{
  Constant *c=module->getOrInsertFunction("test_calls",Type::getVoidTy(*context),NULL);
  Function *func=cast<Function>(c);
  func->setCallingConv(CallingConv::C);

  BasicBlock *block=BasicBlock::Create(*context,"entry",func);
  IRBuilder<> *builder=llvmw_create_builder(block);


  //XXX the findFunction() calls below don't do NULL checks for brevity's sake

  //call to c_noargs()
  Function *callee=findFunction("c_noargs");
  builder->CreateCall(callee);

  //call to c_printmsg()
  callee=findFunction("c_printstr");
  Value *str=builder->CreateGlobalStringPtr("I'm a compiler!");
  std::vector<Value*> args=std::vector<Value*>{str};
  builder->CreateCall(callee,args);

  //call to CppClass::staticNoArgs()
  builder->CreateCall(findFunction("_ZN8CppClass12staticNoArgsEv"));

  //call to ExtendedClass::staticNoArgs()
  builder->CreateCall(findFunction("_ZN13ExtendedClass12staticNoArgsEv"));

  //call to ExtendedClass::staticPrintMsg()
  callee=findFunction("_ZN13ExtendedClass14staticPrintMsgEPKc");
  builder->CreateCall(callee,args);


  //call to CppClass::printMsg()
  Value *baseInst=builder->CreateCall(findFunction("_ZN8CppClass11getInstanceEv"),None,"baseInst");
  str=builder->CreateGlobalStringPtr("I'm calling CppClass::printMsg() on a CppClass instance!");
  args=std::vector<Value*>{baseInst,str};
  builder->CreateCall(findFunction("_ZN8CppClass8printMsgEPKc"),args);

  //call to (inherited) ExtendedClass::printMsg()
  Value *extInst=builder->CreateCall(findFunction("_ZN13ExtendedClass11getInstanceEv"),None,"extInst");
  str=builder->CreateGlobalStringPtr("I'm calling CppClass::printMsg() on an ExtendedClass instance!");
  args=std::vector<Value*>{extInst,str};
  builder->CreateCall(findFunction("_ZN8CppClass8printMsgEPKc"),args);


  //end: return
  builder->CreateRet(NULL);

  verifyFunction(*func);
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


int main(int argc, char **argv)
{
  module=llvmw_create_module();
  context=llvmw_get_module_context(module);

  addCLibraryWrappers();
  addCppLibraryWrappers();
  addTestCallsFunction();
  addCompilerDemo();
  addMainFunction();

  verifyModule(*module);
  module->print(errs(),NULL);

  return 0;
}
