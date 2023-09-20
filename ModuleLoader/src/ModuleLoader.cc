#include <ModuleLoader.hh>
#include <cstddef>
#include <fstream>
#include <vector>


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
        cerr << "error loading file: " << filename << "'\n";
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
    int i = 0;
    for (string filename : filelist){
        unique_ptr<Module> M = parseIRFile(filename, *err, *context);
        if (M == NULL) {
            cerr << "error loading file: '" << filename << "'\n";
            (*ret)[filename] = nullptr;
            // ret->insert(pair<string, Module*>(filename, nullptr));
        } else {
            (*ret)[filename] = M.release();
            i++;
            if (!slience && i % 500 == 0){
                cout << "." << flush;
            }
        }
    }
    if(!slience) {
        cout << endl << i << "Module(s) Loaded succfully." << endl;
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
        cout << buf << endl;
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