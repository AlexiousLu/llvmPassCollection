#include <StructAnalyzer.hh>
#include <sqlite3.h>
#include <json.hpp>


int main(){
    ModuleLoader ml;
    // map<string, Module*>* modules = ml.loadModules("./Data/linux_5.18bcs/bc.list", false);
    Module* module = ml.loadModule("temp.ll");
    
    StructExtracter se;
    map<string, Module*>* fake_modules = new map<string, Module*>();
    (*fake_modules)["fake_module"] = module;
    RootNode* rn = se.extractModules(fake_modules);
    se.saveToSqlite3("./struct_info.sqlite");

    cout << (rn == (*rn)["fake_module"]->getParent()) << endl;
    // cout << ((*rn)["fake_module"] == (*(*rn)["fake_module"])["s1"]->getParent()) << endl;
    // cout << ((*(*rn)["fake_module"])["s1"] == (*(*(*rn)["fake_module"])["s1"])["a"]->getParent()) << endl;
    
    cout << "finish" << endl;

    json j = json::parse("{\"derived_pointer\":\"nullptr\",\"is_basetype\":true,\"is_derived\":false,\"member_name\":\"a\",\"offset\":0,\"size\":4,\"type_str\":\"int\"}");
    cout << j.size() << endl;
    MemberNode mn;
    mn.from_json(j, mn);
    cout << mn.toString() << endl;


}