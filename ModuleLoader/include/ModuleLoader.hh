#include <iostream>
#include <llvm/IR/LLVMContext.h>
#include <map>
#include <include_llvm.hh>


using namespace std;
using namespace llvm;

class ModuleLoader{    
private:
    LLVMContext* ctx;
    LLVMContext* parse_LLVMContext(LLVMContext* context);
    SMDiagnostic* parse_SMDiagnostic(SMDiagnostic* err);

    map<string, Module*>* loadModule(vector<string> filelist, LLVMContext* context=nullptr, SMDiagnostic* err=nullptr, bool slience=false);

public:
    ModuleLoader() {
        this->ctx = new LLVMContext();
    }
    ModuleLoader(LLVMContext* ctx): ctx(ctx) {}

    LLVMContext* getContext() {
        return this->ctx;
    }

    Module* loadModule(string filename, LLVMContext* context=nullptr, SMDiagnostic* err=nullptr);

    map<string, Module*>* loadModules(string bclistFile, LLVMContext* context=nullptr, SMDiagnostic* err=nullptr, bool slience=false);
    map<string, Module*>* loadModules(string bclistFile, bool slience);
    
};