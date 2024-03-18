#include <StructAnalyzer.hh>
#include <sqlite3.h>
#include <json.hpp>


int main(){
    ModuleLoader ml;
    // Module* module = ml.loadModule("test.ll");
    // Module* module = ml.loadModule("/data/linux-stable-with-debug/net/wireless/.nl80211.o.ll");
    // Module* module = ml.loadModule("/data/linux-stable-with-debug/drivers/mtd/nand/raw/.nand_onfi.o.ll");
    // map<string, Module*>* fake_modules = new map<string, Module*>();
    // (*fake_modules)["fake_module"] = module;

    map<string, Module*>* modules = ml.loadModules("./Data/linux_5.18_debug/ll.list", false);
    
    StructExtracter se;
    // RootNode* rn = se.exModules(fake_modules);
    RootNode* rn = se.exModules(modules);
    se.saveToMysql("./Config/mysql.conf", "struct_info");
    
    cout << "finish" << endl;

    // se.loadFromSqlite3("./struct_info_large.sqlite");
    // rn = se.getTree();
    // cout << (rn == (*rn)["test.c"]->getParent()) << endl;
    // cout << "finish" << endl;

}