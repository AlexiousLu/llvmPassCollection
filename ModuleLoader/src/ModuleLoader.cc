#include <ModuleLoader.hh>
#include <cstddef>
#include <fstream>
#include <llvm/IR/LLVMContext.h>
#include <vector>
#include <omp.h>


LLVMContext* ModuleLoader::parse_LLVMContext(LLVMContext* context){
    return context == nullptr ? this->ctx : context;
}

SMDiagnostic* ModuleLoader::parse_SMDiagnostic(SMDiagnostic* err){
    return err == nullptr ? new SMDiagnostic() : err;
}


Module* ModuleLoader::loadModule(string filename, LLVMContext* context, SMDiagnostic* err){
    context = this->parse_LLVMContext(context);
    err = this->parse_SMDiagnostic(err);
    unique_ptr<Module> M = parseIRFile(filename, *err, *context);

    if (M == NULL) {
        cerr << "error loading file: " << filename 
             << ",because " << err->getMessage().str() 
             << " at "<< err->getLineNo() << ": " << err->getLineContents().str() << "'\n";
        return nullptr;
    }

    Module *Module = M.release();
    return Module;
}


map<string, Module*>* ModuleLoader::loadModule(vector<string> filelist, LLVMContext* context, SMDiagnostic* err, bool slience){
    context = this->parse_LLVMContext(context);
    err = this->parse_SMDiagnostic(err);
    map<string, Module*> *ret = new map<string, Module*>();
    if(!slience) {
        cout << "Total " << filelist.size() << " file(s)" << endl;
    }

    for(string filename : filelist){
        (*ret)[filename] = (Module*)0;
    }

    int i = 0;

    #pragma omp parallel for num_threads(10) private(err) // NOTE: all variable should be DEFINED in the for block, or it will be used as shared variable
    for (i = 0; i < filelist.size(); i++){
        string filename = filelist[i];
        LLVMContext* context1 = new LLVMContext(); 
        (*ret)[filename] = this->loadModule(filename, context1, err);
        if (!slience && i % 500 == 0){
            cout << "." << flush;
        }
    }
    
    if(!slience) {
        cout << endl << "Module(s) Loaded succfully." << endl;
    } 
    return ret;
}


map<string, Module*>* ModuleLoader::loadModules(string bclistFile, LLVMContext* context, SMDiagnostic* err, bool slience){
    context = this->parse_LLVMContext(context);
    err = this->parse_SMDiagnostic(err);
    ifstream inf(bclistFile);
    if(inf.fail()){
        cerr << "Cannot open bclistFile: " << bclistFile << endl;
        return nullptr;
    }
    vector<string>* paths = new vector<string>();
    string buf;
    while(!inf.eof()) {
        inf >> buf;
        paths -> push_back(buf);
    }
    return this->loadModule(*paths, context, err, slience);
}

map<string, Module*>* ModuleLoader::loadModules(string bclistFile, bool slience){
    return this->loadModules(bclistFile, nullptr, nullptr, slience);
}

// // test
// int main(){
//     ModuleLoader ml;
//     ml.loadModules("./bc.list");
// }