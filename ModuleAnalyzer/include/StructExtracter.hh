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

using namespace std;
using namespace llvm;
using namespace struct_tree;

//extract all structures from modules, only the newest extract would be valid.
class StructExtracter {
private:
//  module_name: struct_name : field_name: {"type": xx, "otherinfo...."}
    RootNode* root;

    pair<string, DIType*> recurDerefPointer(DIDerivedType* pointer_def);
    variant<MemberNode*, StructNode*> recurExtractType(DIType* struct_def, vector<string>& recursived_Ty_name, ModuleNode* curr_module_node);
    vector<string> member_to_record(MemberNode* member_node);
public:
    StructExtracter() { this->root = new RootNode(); }
    StructExtracter(RootNode* r): root(r) {}

    RootNode* getTree(){ return this->root; };

    ModuleNode* extractModule(Module* module);
    optional<StructNode*> extractVariableDeclear(CallInst* CI, vector<string>& handled_Ty_name, ModuleNode* curr_module_node);
    RootNode* extractModules(map<string, Module*>* modules);

    RootNode* loadFromSqlite3(string sqlite3_path);

    bool saveToSqlite3(string sqlite3_path, RootNode* root=nullptr);

};


#endif