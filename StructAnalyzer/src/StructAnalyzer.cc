#include <StructAnalyzer.hh>

int main(){
    ModuleLoader ml;
    map<string, Module*>* modules = ml.loadModules("./Data/linux_5.18bcs/bc.list", false);
    cout << 1 << endl;
}