#ifndef LLVM_STRUCT_EXTRACTER_HH
#define LLVM_STRUCT_EXTRACTER_HH
#include <include_llvm.hh>
#include <struct_tree.hh>
#include <iostream>
#include <fstream>
#include <optional>
#include <variant>
#include <json.hpp>
#include <sqlite3.h>
#include <DatabaseManager.hh>

using namespace std;
using namespace llvm;
using namespace struct_tree;

class ExtractInfo { // an info for a MEMBER of struct;
public:
    ModuleNode* curr_module_node = nullptr;
    StructNode* curr_struct_node = nullptr;
    MemberNode* curr_member_node = nullptr;
    ExtractInfo() {};
    ExtractInfo(ModuleNode* mn) { curr_module_node = mn; };
};

//extract all structures from modules, only the newest extract would be valid.
class StructExtracter {
private:
//  module_name: struct_name : field_name: {"type": xx, "otherinfo...."}
    RootNode* root;

    void exDerivedType(DIDerivedType* derived_type, ExtractInfo& info);
    void exCompositeType(DICompositeType* composite_type, ExtractInfo& info);
    void exBasicType(DIBasicType* base_type, ExtractInfo& info);
    void exType(DIType* type, ExtractInfo& info);
public:
    StructExtracter() { this->root = new RootNode(); }
    StructExtracter(RootNode* r): root(r) {}

    RootNode* getTree(){ return this->root; };

    ModuleNode* exModule(Module* module);
    optional<StructNode*> exVarDeclear(CallInst* CI, ModuleNode* curr_module_node);
    RootNode* exModules(map<string, Module*>* modules);

    RootNode* loadFromSqlite3(string sqlite3_path);

    bool saveToSqlite3(string sqlite3_path, RootNode* root=nullptr);
    
    RootNode* loadFromMysql(string config_file, string table_name);
    
    bool saveToMysql(string config_file, string table_name, RootNode* root=nullptr);

};


#endif