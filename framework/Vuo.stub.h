﻿/**
 * @file
 * Vuo prefix header.
 *
 * @copyright Copyright © 2012–2014 Kosada Incorporated.
 * This interface description may be modified and distributed under the terms of the GNU Lesser General Public License (LGPL) version 2 or later.
 * For more information, see http://vuo.org/license.
 */

#ifndef VUO_H
#define VUO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdocumentation"
#include <json/json.h>
#pragma clang diagnostic pop

#include <zmq/zmq.h>

#ifdef __cplusplus

#include <string>
#include <vector>
#include <map>
#include <list>

using namespace std;

#define __STDC_CONSTANT_MACROS
#define __STDC_FORMAT_MACROS
#define __STDC_LIMIT_MACROS

#ifdef NO
	#define VUO_NO_ALREADY_DEFINED
	#undef NO
#endif
#define NO VUO_LLVM_NO_RENAMED

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Wunused-private-field"
#pragma clang diagnostic ignored "-Wshorten-64-to-32"
#include <clang/Basic/Version.h>
#include <clang/CodeGen/CodeGenAction.h>
#include <clang/Driver/ArgList.h>
#include <clang/Driver/Compilation.h>
#include <clang/Driver/Driver.h>
#include <clang/Driver/ToolChain.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/CompilerInvocation.h>
#include <clang/Frontend/FrontendDiagnostic.h>
#include <clang/Frontend/TextDiagnosticPrinter.h>
#include <llvm/Support/DynamicLibrary.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/Program.h>
#include <llvm/Bitcode/Archive.h>
#include <llvm/LLVMContext.h>
#include <llvm/Module.h>
#include <llvm/DerivedTypes.h>
#include <llvm/Constants.h>
#include <llvm/Instructions.h>
#include <llvm/LinkAllPasses.h>
#include <llvm/Linker.h>
#include <llvm/PassManager.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/Analysis/ValueTracking.h>
#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/ExecutionEngine/Interpreter.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/system_error.h>
#include <llvm/Support/IRReader.h>
#include <llvm/Support/PathV1.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Transforms/Utils/Cloning.h>
#include <llvm/ValueSymbolTable.h>
#pragma clang diagnostic pop

#undef NO
#ifdef VUO_NO_ALREADY_DEFINED
	#undef VUO_NO_ALREADY_DEFINED
	// From /usr/include/objc/objc.h line 58
	#define NO __objc_no
#endif

using namespace llvm;

@INCLUDE_VUO_CXX_HEADERS@

#endif

#ifdef __cplusplus
extern "C"
{
#endif

@INCLUDE_VUO_C_HEADERS@

#ifdef __cplusplus
}
#endif

#endif
