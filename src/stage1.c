/** \file
 * A simple LLVM-based compiler / code generator using the LLVM's C API.
 *
 * TODO: the C calling convention is default, but maybe set it explicitly anyways
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

  LLVMBasicBlockRef block=LLVMAppendBasicBlock(func,"entry");
  LLVMBuilderRef builder=LLVMCreateBuilder();
  LLVMPositionBuilderAtEnd(builder,block);

  LLVMValueRef callee=findFunction("compiler_demo");
  LLVMBuildCall(builder,callee,NULL,0,"");

  LLVMValueRef int32_0=LLVMConstInt(int32,0,false);
  LLVMBuildRet(builder,int32_0);

  LLVMVerifyFunction(func,LLVMPrintMessageAction);
}


void addCompilerDemo()
{
  //stage1 data types
  LLVMTypeRef void_type=LLVMVoidType();
  LLVMTypeRef i1_type=LLVMInt1Type();
  LLVMTypeRef i8_type=LLVMInt8Type();
  LLVMTypeRef i32_type=LLVMInt32Type();
  LLVMTypeRef i8ptr_type=LLVMPointerType(i8_type,0);
//  LLVMTypeRef i8ptrptr_type=LLVMPointerType(i8ptr_type,0);

  //stage1 constants
  LLVMValueRef i32_0_val=LLVMConstInt(i32_type,0,false);
//  LLVMValueRef i32_5_val=LLVMConstInt(i32_type,5,false);
  LLVMValueRef false_val=LLVMConstInt(i1_type,0,false);
  LLVMValueRef null_val=LLVMConstNull(i8ptr_type);

  //stage1 function types
  LLVMTypeRef ft_void=LLVMFunctionType(void_type,NULL,0,false);
  LLVMTypeRef ft_i8ptr=LLVMFunctionType(i8ptr_type,NULL,0,false);
  LLVMTypeRef ft_i8ptr_i8ptr=LLVMFunctionType(i8ptr_type,&i8ptr_type,1,false);
  LLVMTypeRef ft_void_i8ptr=LLVMFunctionType(void_type,&i8ptr_type,1,false);


  LLVMTypeRef types[]={i8ptr_type,i32_type,i8ptr_type};
  LLVMTypeRef ft_void_i8ptr_i32_i8ptr=LLVMFunctionType(void_type,types,3,false);

  LLVMTypeRef types2[]={i8ptr_type,i8ptr_type,i32_type,i1_type};
  LLVMTypeRef ft_i8ptr_i8ptr_i8ptr_i32_i1=LLVMFunctionType(i8ptr_type,types2,4,false);

  LLVMTypeRef types3[]={i8ptr_type,i32_type};
  LLVMTypeRef ft_i8ptr_i8ptr_i32=LLVMFunctionType(i8ptr_type,types3,2,false);

  LLVMTypeRef types4[]={i8ptr_type,i32_type,i1_type};
  LLVMTypeRef ft_i8ptr_i8ptr_i32_i1=LLVMFunctionType(i8ptr_type,types4,3,false);

  LLVMTypeRef types5[]={i8ptr_type,i8ptr_type,i8ptr_type};
  LLVMTypeRef ft_i8ptr_i8ptr_i8ptr_i8ptr=LLVMFunctionType(i8ptr_type,types5,3,false);

  LLVMTypeRef types6[]={i8ptr_type,i8ptr_type};
  LLVMTypeRef ft_i8ptr_i8ptr_i8ptr=LLVMFunctionType(i8ptr_type,types6,2,false);
  LLVMTypeRef ft_void_i8ptr_i8ptr=LLVMFunctionType(i8ptr_type,types6,2,false);

//  LLVMTypeRef types7[]={i8ptr_type,i8ptr_type,i8ptrptr_type,i32_type,i8ptr_type};
//  LLVMTypeRef ft_i8ptr_i8ptr_i8ptr_i8ptrptr_i32_i8ptr=LLVMFunctionType(i8ptr_type,types7,5,false);

  LLVMTypeRef types8[]={i8ptr_type,i8ptr_type,i8ptr_type,i32_type,i8ptr_type};
  LLVMTypeRef ft_i8ptr_i8ptr_i8ptr_i8ptr_i32_i8ptr=LLVMFunctionType(i8ptr_type,types8,5,false);

  //stage1 entry
  LLVMValueRef func=LLVMAddFunction(module,"compiler_demo",ft_void);

  LLVMBasicBlockRef block=LLVMAppendBasicBlock(func,"entry");
  LLVMBuilderRef builder=LLVMCreateBuilder();
  LLVMPositionBuilderAtEnd(builder,block);


  //stage2 module creation

  //module=LLVMModuleCreateWithName("stage2")
  LLVMValueRef callee=LLVMAddFunction(module,"LLVMModuleCreateWithName",ft_i8ptr_i8ptr);
  LLVMValueRef modulename=LLVMBuildGlobalStringPtr(builder,"stage3","stage2_targetmodulename");
  LLVMValueRef stage2_module=LLVMBuildCall(builder,callee,&modulename,1,"stage3_module");

  //context=LLVMGetModuleContext(module)
//  callee=LLVMAddFunction(module,"LLVMGetModuleContext",ft_i8ptr_i8ptr);
//  LLVMValueRef stage2_context=LLVMBuildCall(builder,callee,&stage2_module,1,"stage2_context");

  //stage2 data types
  callee=LLVMAddFunction(module,"LLVMVoidType",ft_i8ptr);
  LLVMValueRef stage2_void_type=LLVMBuildCall(builder,callee,NULL,0,"stage2_void_type");
//  callee=LLVMAddFunction(module,"LLVMInt1Type",ft_i8ptr);
//  LLVMValueRef stage2_i1_type=LLVMBuildCall(builder,callee,NULL,0,"stage2_i1_type");
  callee=LLVMAddFunction(module,"LLVMInt8Type",ft_i8ptr);
  LLVMValueRef stage2_i8_type=LLVMBuildCall(builder,callee,NULL,0,"stage2_i8_type");
  callee=LLVMAddFunction(module,"LLVMInt32Type",ft_i8ptr);
  LLVMValueRef stage2_i32_type=LLVMBuildCall(builder,callee,NULL,0,"stage2_i32_type");
  callee=LLVMAddFunction(module,"LLVMPointerType",ft_i8ptr_i8ptr_i32);
  LLVMValueRef args4[]={stage2_i8_type,i32_0_val};
  LLVMValueRef stage2_i8ptr_type=LLVMBuildCall(builder,callee,args4,2,"stage2_i8ptr_type");
//  args4[0]=stage2_i8ptr_type;
//  LLVMValueRef stage2_i8ptrptr_type=LLVMBuildCall(builder,callee,args4,2,"stage2_i8ptrptr_type");

  //stage2 constants
  callee=LLVMAddFunction(module,"LLVMConstNull",ft_i8ptr_i8ptr);
  LLVMValueRef stage2_null_val=LLVMBuildCall(builder,callee,&stage2_i8ptr_type,1,"stage2_null_val");
//  LLVMValueRef stage2_nullp_val=LLVMBuildCall(builder,callee,&stage2_i8ptrptr_type,1,"stage2_nullp_val");

  callee=LLVMAddFunction(module,"LLVMConstInt",ft_i8ptr_i8ptr_i32_i1);
  LLVMValueRef args3[]={stage2_i32_type,i32_0_val,false_val};
  LLVMValueRef stage2_i32_0_val=LLVMBuildCall(builder,callee,args3,3,"stage2_i32_0_val");
//  args3[0]=stage2_i1_type;
//  LLVMValueRef stage2_false_val=LLVMBuildCall(builder,callee,args3,3,"stage2_false_val");

  //stage2 function types
  callee=LLVMAddFunction(module,"LLVMFunctionType",ft_i8ptr_i8ptr_i8ptr_i32_i1);
  LLVMValueRef args5[]={stage2_i32_type,null_val,i32_0_val,false_val};
  LLVMValueRef stage2_ft_i32=LLVMBuildCall(builder,callee,args5,4,"stage2_ft_i32");
  args5[0]=stage2_void_type;
  LLVMValueRef stage2_ft_void=LLVMBuildCall(builder,callee,args5,4,"stage2_ft_void");

  //stage2: declare int main()
  callee=LLVMAddFunction(module,"LLVMAddFunction",ft_i8ptr_i8ptr_i8ptr_i8ptr);
  LLVMValueRef stage2_str_main=LLVMBuildGlobalStringPtr(builder,"main","stage2_str_main");
  LLVMValueRef args6[]={stage2_module,stage2_str_main,stage2_ft_i32};
  LLVMValueRef stage2_main=LLVMBuildCall(builder,callee,args6,3,"stage2_main");

  LLVMValueRef stage2_str_c_noargs=LLVMBuildGlobalStringPtr(builder,"c_noargs","stage2_str_c_noargs");
  args6[1]=stage2_str_c_noargs;
  args6[2]=stage2_ft_void;
  LLVMValueRef stage2_c_noargs=LLVMBuildCall(builder,callee,args6,3,"stage2_c_noargs");

  //stage2: LLVMBasicBlockRef block=LLVMAppendBasicBlock(func,"entry");
  callee=LLVMAddFunction(module,"LLVMAppendBasicBlock",ft_i8ptr_i8ptr_i8ptr);
  LLVMValueRef stage2_str_entry=LLVMBuildGlobalStringPtr(builder,"entry","stage2_str_entry");
  LLVMValueRef args7[]={stage2_main,stage2_str_entry};
  LLVMValueRef stage2_block=LLVMBuildCall(builder,callee,args7,2,"stage2_block");

  //stage2: LLVMBuilderRef builder=LLVMCreateBuilder();
  callee=LLVMAddFunction(module,"LLVMCreateBuilder",ft_i8ptr);
  LLVMValueRef stage2_builder=LLVMBuildCall(builder,callee,NULL,0,"stage2_builder");

  //stage2: LLVMPositionBuilderAtEnd(builder,block);
  callee=LLVMAddFunction(module,"LLVMPositionBuilderAtEnd",ft_void_i8ptr_i8ptr);
  LLVMValueRef args8[]={stage2_builder,stage2_block};
  LLVMBuildCall(builder,callee,args8,2,"");

  //stage2: call c_noargs()
  callee=LLVMAddFunction(module,"LLVMBuildCall",ft_i8ptr_i8ptr_i8ptr_i8ptr_i32_i8ptr);  //XXX args should be i8ptrptr, but null pointer literal below isn't typed correctly
  LLVMValueRef stage2_str_=LLVMBuildGlobalStringPtr(builder,"","stage2_str_");
  LLVMValueRef args10[]={stage2_builder,stage2_c_noargs,stage2_null_val,i32_0_val,stage2_str_};
  LLVMBuildCall(builder,callee,args10,5,"");

  //stage2: return 0
  callee=LLVMAddFunction(module,"LLVMBuildRet",ft_i8ptr_i8ptr_i8ptr);
  LLVMValueRef args9[]={stage2_builder,stage2_i32_0_val};
  LLVMBuildCall(builder,callee,args9,2,"");


  //finish stage2 module

  //LLVMVerifyModule(module,LLVMPrintMessageAction,NULL)
  callee=LLVMAddFunction(module,"LLVMVerifyModule",ft_void_i8ptr_i32_i8ptr);
  LLVMValueRef pars[]={stage2_module,i32_0_val,null_val};
  LLVMBuildCall(builder,callee,pars,3,"");

  callee=LLVMAddFunction(module,"LLVMDumpModule",ft_void_i8ptr);
  LLVMBuildCall(builder,callee,&stage2_module,1,"");

  //finish stage1 function

  LLVMBuildRetVoid(builder);
  LLVMVerifyFunction(func,LLVMPrintMessageAction);
}


int main(int argc, char **argv)
{
  module=LLVMModuleCreateWithName("stage2");
  context=LLVMGetModuleContext(module);

  addCompilerDemo();
  addMainFunction();

  LLVMVerifyModule(module,LLVMPrintMessageAction,NULL);
  printf("%s",LLVMPrintModuleToString(module));

  return 0;
}
