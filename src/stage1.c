/** \file
 * A simple LLVM-based compiler / code generator using LLVM's C API.
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


//stage 1 LLVM basics
LLVMModuleRef module;
LLVMContextRef context;
LLVMBuilderRef builder;
LLVMBool false=0;


//stage 1 data types
LLVMTypeRef void_type;
LLVMTypeRef i1_type;
LLVMTypeRef i8_type;
LLVMTypeRef i32_type;
LLVMTypeRef i8ptr_type;
//LLVMTypeRef i8ptrptr_type=LLVMPointerType(i8ptr_type,0);


//stage 1 function types
LLVMTypeRef ft_i32;
LLVMTypeRef ft_void;
LLVMTypeRef ft_i8ptr;
LLVMTypeRef ft_i8ptr_i8ptr;
LLVMTypeRef ft_void_i8ptr;


//stage 1 constants
LLVMValueRef i32_0;
LLVMValueRef i32_1;
LLVMValueRef i8_i;
LLVMValueRef i8_p;
LLVMValueRef i8_q;
LLVMValueRef i8_pipe;
LLVMValueRef i8ptr_null;
LLVMValueRef i1_false;



void prepareStage1Globals(void)
{
  //LLVM basics
  module=LLVMModuleCreateWithName("stage2");
  context=LLVMGetModuleContext(module);
  builder=LLVMCreateBuilder();

  //simple data types
  void_type=LLVMVoidType();
  i1_type=LLVMInt1Type();
  i8_type=LLVMInt8Type();
  i32_type=LLVMInt32Type();
  i8ptr_type=LLVMPointerType(i8_type,0);

  //function types
  ft_i32=LLVMFunctionType(i32_type,NULL,0,false);
  ft_void=LLVMFunctionType(void_type,NULL,0,false);
  ft_i8ptr=LLVMFunctionType(i8ptr_type,NULL,0,false);
  ft_i8ptr_i8ptr=LLVMFunctionType(i8ptr_type,&i8ptr_type,1,false);
  ft_void_i8ptr=LLVMFunctionType(void_type,&i8ptr_type,1,false);

  //constants
  i32_0=LLVMConstInt(i32_type,0,false);
  i32_1=LLVMConstInt(i32_type,1,false);
  i8_i=LLVMConstInt(i8_type,'i',false);
  i8_p=LLVMConstInt(i8_type,'p',false);
  i8_q=LLVMConstInt(i8_type,'q',false);
  i8_pipe=LLVMConstInt(i8_type,'|',false);
  i8ptr_null=LLVMConstNull(i8ptr_type);
  i1_false=LLVMConstInt(i1_type,0,false);
}


LLVMValueRef findFunction(const char *name)
{
  LLVMValueRef func=LLVMGetNamedFunction(module,name);
  if(!func)
    fprintf(stderr,"error: could not find function '%s'\n",name);
  return func;
}


void addMainFunction()
{
  LLVMValueRef func=LLVMAddFunction(module,"main",ft_i32);

  LLVMBasicBlockRef block=LLVMAppendBasicBlock(func,"entry");
  LLVMPositionBuilderAtEnd(builder,block);

  LLVMBuildCall(builder,findFunction("compile"),NULL,0,"");

  LLVMBuildRet(builder,i32_0);
}


void addSourceHandlerFragment(LLVMValueRef func, LLVMBasicBlockRef end_block)
{
  LLVMBasicBlockRef instr_parser=LLVMAppendBasicBlock(func,"instr_parser");
  LLVMBasicBlockRef instr_advance_1=LLVMAppendBasicBlock(func,"instr_advance_1");
  LLVMBasicBlockRef instr_advance_block=LLVMAppendBasicBlock(func,"instr_advancer");
  LLVMBasicBlockRef ignore_instr_block=LLVMAppendBasicBlock(func,"ignore_instr");
  LLVMBasicBlockRef print_instr_block=LLVMAppendBasicBlock(func,"print_instr");
  LLVMBasicBlockRef quit_instr_block=LLVMAppendBasicBlock(func,"quit_instr");
  LLVMBasicBlockRef err_block=LLVMAppendBasicBlock(func,"err_block");

  //declare reference to char *source_code and retrieve it
  LLVMValueRef source_code_ref=LLVMAddGlobal(module,i8ptr_type,"source_code");
  LLVMValueRef source_code_value=LLVMBuildLoad(builder,source_code_ref,"source_code_value");

  LLVMValueRef start=LLVMBuildGEP(builder,source_code_value,&i32_0,1,"start");
  LLVMValueRef cur=LLVMBuildAlloca(builder,i8ptr_type,"cur");
  LLVMBuildStore(builder,start,cur);
  LLVMBuildBr(builder,instr_parser);

  //instruction parser
  LLVMPositionBuilderAtEnd(builder,instr_parser);
  LLVMValueRef instr_pos=LLVMBuildLoad(builder,cur,"instr_pos");
  LLVMValueRef instruction=LLVMBuildLoad(builder,instr_pos,"instruction");
  LLVMValueRef sw=LLVMBuildSwitch(builder,instruction,err_block,3);
  LLVMAddCase(sw,i8_p,print_instr_block);
  LLVMAddCase(sw,i8_i,ignore_instr_block);
  LLVMAddCase(sw,i8_q,quit_instr_block);

  //instruction pointer advancement after instruction call
  LLVMPositionBuilderAtEnd(builder,instr_advance_block);
  LLVMValueRef pos=LLVMBuildLoad(builder,cur,"pos");
  LLVMValueRef next=LLVMBuildGEP(builder,pos,&i32_1,1,"next");
  LLVMBuildStore(builder,next,cur);
  LLVMValueRef ch=LLVMBuildLoad(builder,next,"ch");
  LLVMValueRef switch_char=LLVMBuildSwitch(builder,ch,instr_advance_block,1);
  LLVMAddCase(switch_char,i8_pipe,instr_advance_1);

  //advance instruction pointer by 1 (to skip found "|" delimiter before handling next instruction)
  LLVMPositionBuilderAtEnd(builder,instr_advance_1);
  LLVMValueRef orig=LLVMBuildLoad(builder,cur,"orig");
  LLVMValueRef inc=LLVMBuildGEP(builder,orig,&i32_1,1,"inc");
  LLVMBuildStore(builder,inc,cur);
  LLVMBuildBr(builder,instr_parser);

  //"ignore" instruction handler
  LLVMPositionBuilderAtEnd(builder,ignore_instr_block);
  LLVMValueRef ignore_func=LLVMAddFunction(module,"print_ignore",ft_void);
  LLVMBuildCall(builder,ignore_func,NULL,0,"");
  LLVMBuildBr(builder,instr_advance_block);

  //"print" instruction handler
  LLVMPositionBuilderAtEnd(builder,print_instr_block);
  LLVMValueRef msg=LLVMBuildGEP(builder,instr_pos,&i32_1,1,"msg");
  LLVMValueRef printer_func=LLVMAddFunction(module,"printer",ft_void_i8ptr);
  LLVMBuildCall(builder,printer_func,&msg,1,"");
  LLVMBuildBr(builder,instr_advance_block);

  //"quit" instruction handler
  LLVMPositionBuilderAtEnd(builder,quit_instr_block);
  LLVMValueRef quit_func=LLVMAddFunction(module,"print_quit",ft_void);
  LLVMBuildCall(builder,quit_func,NULL,0,"");
  LLVMBuildBr(builder,end_block);

  //error handler
  LLVMPositionBuilderAtEnd(builder,err_block);
  LLVMValueRef print_instruction_error_func=LLVMAddFunction(module,"print_instruction_error",ft_void_i8ptr);
  LLVMBuildCall(builder,print_instruction_error_func,&instr_pos,1,"");
  LLVMBuildBr(builder,end_block);
}


void addCompileFunction()
{
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
  LLVMValueRef func=LLVMAddFunction(module,"compile",ft_void);

  LLVMBasicBlockRef block=LLVMAppendBasicBlock(func,"entry");
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
  LLVMValueRef args4[]={stage2_i8_type,i32_0};
  LLVMValueRef stage2_i8ptr_type=LLVMBuildCall(builder,callee,args4,2,"stage2_i8ptr_type");
//  args4[0]=stage2_i8ptr_type;
//  LLVMValueRef stage2_i8ptrptr_type=LLVMBuildCall(builder,callee,args4,2,"stage2_i8ptrptr_type");

  //stage2 constants
  callee=LLVMAddFunction(module,"LLVMConstNull",ft_i8ptr_i8ptr);
  LLVMValueRef stage2_null_val=LLVMBuildCall(builder,callee,&stage2_i8ptr_type,1,"stage2_null_val");
//  LLVMValueRef stage2_nullp_val=LLVMBuildCall(builder,callee,&stage2_i8ptrptr_type,1,"stage2_nullp_val");

  callee=LLVMAddFunction(module,"LLVMConstInt",ft_i8ptr_i8ptr_i32_i1);
  LLVMValueRef args3[]={stage2_i32_type,i32_0,i1_false};
  LLVMValueRef stage2_i32_0_val=LLVMBuildCall(builder,callee,args3,3,"stage2_i32_0_val");
//  args3[0]=stage2_i1_type;
//  LLVMValueRef stage2_false_val=LLVMBuildCall(builder,callee,args3,3,"stage2_false_val");

  //stage2 function types
  callee=LLVMAddFunction(module,"LLVMFunctionType",ft_i8ptr_i8ptr_i8ptr_i32_i1);
  LLVMValueRef args5[]={stage2_i32_type,i8ptr_null,i32_0,i1_false};
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


  //Parse and handle source from libsource.
  //This will eventually replace all stage 2 calls to LLVM functions
  LLVMBasicBlockRef handler_end_block=LLVMAppendBasicBlock(func,"handler_end");
  addSourceHandlerFragment(func,handler_end_block);
  LLVMPositionBuilderAtEnd(builder,handler_end_block);


  //stage2: call c_noargs()
  callee=LLVMAddFunction(module,"LLVMBuildCall",ft_i8ptr_i8ptr_i8ptr_i8ptr_i32_i8ptr);  //XXX args should be i8ptrptr, but null pointer literal below isn't typed correctly
  LLVMValueRef stage2_str_=LLVMBuildGlobalStringPtr(builder,"","stage2_str_");
  LLVMValueRef args10[]={stage2_builder,stage2_c_noargs,stage2_null_val,i32_0,stage2_str_};
  LLVMBuildCall(builder,callee,args10,5,"");

  //stage2: return 0
  callee=LLVMAddFunction(module,"LLVMBuildRet",ft_i8ptr_i8ptr_i8ptr);
  LLVMValueRef args9[]={stage2_builder,stage2_i32_0_val};
  LLVMBuildCall(builder,callee,args9,2,"");


  //finish stage2 module

  //LLVMVerifyModule(module,LLVMPrintMessageAction,NULL)
  callee=LLVMAddFunction(module,"LLVMVerifyModule",ft_void_i8ptr_i32_i8ptr);
  LLVMValueRef pars[]={stage2_module,i32_0,i8ptr_null};
  LLVMBuildCall(builder,callee,pars,3,"");

  callee=LLVMAddFunction(module,"LLVMDumpModule",ft_void_i8ptr);
  LLVMBuildCall(builder,callee,&stage2_module,1,"");

  //finish stage1 function

  LLVMBuildRetVoid(builder);
}


int main(int argc, char **argv)
{
  prepareStage1Globals();

  addCompileFunction();
  addMainFunction();

  printf("%s",LLVMPrintModuleToString(module));
  if(LLVMVerifyModule(module,LLVMPrintMessageAction,NULL))
    printf("\n");

  return 0;
}
