/** \file
 * A simple LLVM-based compiler / code generator using the LLVM's C API.
 *
 * \author Richard Nusser
 * \copyright 2018 Richard Nusser
 * \license GPLv3 (see http://www.gnu.org/licenses/)
 * \sa https://github.com/rinusser/PimpMyCompiler
 */

#include "stdio.h"

#include "llvm-c/Core.h"
#include "llvm-c/Analysis.h"


LLVMModuleRef module;
LLVMContextRef context;
LLVMBool false=0;


LLVMValueRef findFunction(const char *name)
{
  LLVMValueRef func=LLVMGetNamedFunction(module,name);
  if(!func)
    fprintf(stderr,"error: could not find function '%s'\n",name);
  return func;
}


void addMainFunction()
{
  LLVMTypeRef int32=LLVMInt32Type();
  LLVMTypeRef ft=LLVMFunctionType(int32,NULL,0,false);
  LLVMValueRef func=LLVMAddFunction(module,"main",ft);
//  func->setCallingConv(CallingConv::C);

  LLVMBasicBlockRef block=LLVMAppendBasicBlock(func,"entry");
  LLVMBuilderRef builder=LLVMCreateBuilder();
  LLVMPositionBuilderAtEnd(builder,block);

//  Function *callee=findFunction("test_calls");
//  builder->CreateCall(callee);

  LLVMValueRef callee=findFunction("compiler_demo");
  LLVMBuildCall(builder,callee,NULL,0,"");

  LLVMValueRef int32_0=LLVMConstInt(int32,0,false);
  LLVMBuildRet(builder,int32_0);

  LLVMVerifyFunction(func,LLVMPrintMessageAction);
}


void addCompilerDemo()
{
  LLVMTypeRef void_type=LLVMVoidType();
  LLVMTypeRef i8_type=LLVMInt8Type();
  LLVMTypeRef i8ptr_type=LLVMPointerType(i8_type,0);
  LLVMTypeRef ft=LLVMFunctionType(void_type,NULL,0,false);
  LLVMValueRef func=LLVMAddFunction(module,"compiler_demo",ft);
//  func->setCallingConv(CallingConv::C);

  LLVMBasicBlockRef block=LLVMAppendBasicBlock(func,"entry");
  LLVMBuilderRef builder=LLVMCreateBuilder();
  LLVMPositionBuilderAtEnd(builder,block);

  ft=LLVMFunctionType(i8ptr_type,&i8ptr_type,1,false);
  LLVMValueRef callee=LLVMAddFunction(module,"LLVMModuleCreateWithName",ft);
  LLVMValueRef modulename=LLVMBuildGlobalStringPtr(builder,"stage2","modulename");
  LLVMValueRef stage2_module=LLVMBuildCall(builder,callee,&modulename,1,"module");

  //LLVMContext *context=llvmw_get_module_context(module)

  //verifyModule(*module,NULL,NULL);

  ft=LLVMFunctionType(void_type,&i8ptr_type,1,false);
  callee=LLVMAddFunction(module,"LLVMDumpModule",ft);
  LLVMBuildCall(builder,callee,&stage2_module,1,"");

  //end: return
  LLVMBuildRetVoid(builder);
  LLVMVerifyFunction(func,LLVMPrintMessageAction);
}


int main(int argc, char **argv)
{
  module=LLVMModuleCreateWithName("stage1");
  context=LLVMGetModuleContext(module);

  addCompilerDemo();
  addMainFunction();

  LLVMVerifyModule(module,LLVMPrintMessageAction,NULL);
  printf("%s",LLVMPrintModuleToString(module));

  return 0;
}
