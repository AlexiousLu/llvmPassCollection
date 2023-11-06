#include <StructAnalyzer.hh>
#include <sqlite3.h>


int main(){
    ModuleLoader ml;
    // map<string, Module*>* modules = ml.loadModules("./Data/linux_5.18bcs/bc.list", false);
    Module* module = ml.loadModule("temp.ll");
    
    StructExtracter se;
    map<string, Module*>* fake_modules = new map<string, Module*>();
    (*fake_modules)["fake_module"] = module;
    RootNode* rn = se.extractModules(fake_modules);
    cout << "finish" << endl;


    // cout << (rn == (*rn)["fake_module"].getParent()) << endl;
    // Node* n = (*rn)["fake_module"]["s1"].getParent();
    // cout << (&(*rn)["fake_module"] == (*rn)["fake_module"]["s1"].getParent()) << endl;
    // cout << (&rn["fake_module"]["s1"] == rn["fake_module"]["s1"]["a"].getParent()) << endl;
    // cout << (&rn["fake_module"]["s2"] == rn["fake_module"]["s2"]["a"].getParent()) << endl;
    // json* data = se.extractModule(module);
    // se.saveTo("./temp.json", data);
    
    // for (auto model_pair : *modules){
    //     model_pair.second->print(llvm::errs(), nullptr);
    // }


    

}