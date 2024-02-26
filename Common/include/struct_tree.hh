/* Header only C++ Tree */
#ifndef LLVM_MODULE_STRUCT_TREE_HH
#define LLVM_MODULE_STRUCT_TREE_HH
#include <map>
#include <string>
#include <iostream>
#include <iterator>
#include <json.hpp>
#include "include_llvm.hh"


// pending: iterator.
namespace struct_tree{
using namespace std;
using namespace llvm;
using namespace nlohmann;

class Node {
protected:
    Node* parent;
    unsigned char node_type;
    enum {
        ROOT_NODE=1,
        MODULE_NODE,
        STRUCT_NODE,
        MEMBER_NODE,
    };
public:
    Node(unsigned char type): node_type(type), parent(nullptr){}

    Node* getParent(){ return this->parent; }
    void setParent(Node* node) { this->parent = node; }

    bool virtual isLeafNode() = 0;

    unsigned char getNodeType(){ return this->node_type; }
    bool isRootNode() { return this->node_type == ROOT_NODE; }
    bool isModuleNode() { return this->node_type == MODULE_NODE; }
    bool isStructNode() { return this->node_type == STRUCT_NODE; }
};

template <typename K, typename V>
class MappingNode: public Node{
private:
    map<K, V> children;
protected:
    MappingNode(unsigned char t): Node(t) { children = map<K, V>(); }
public:
    V& operator[](const K& key){
        return this->children[key];
    }
    V& getChild(const K& key){
        return (*this)[key];
    }
    bool isLeafNode() {
        return this->children.empty();
    }

    bool has(const K& key) {
        return this->children.find(key) != this->children.end();
    }

    const map<K, V>& getChildren(){
        return this->children;
    }
};

template <typename K, typename V>
class OrderedMappingNode: public Node{
private:
    vector<pair<K, V>> children;
protected:
    OrderedMappingNode(unsigned char t): Node(t) { children = vector<pair<K, V>>(); }
public:
    V& operator[](const K& key){
        if (!this->children.empty()) {
            for(auto it = this->children.begin(); it != this->children.end(); it++){
                if(it->first == key){
                    return it->second;
                }
            }
        }
        this->children.push_back({key, V{}});
        return std::prev(this->children.end())->second;
    }
    V& getChild(const K& key){
        return (*this)[key];
    }

    bool isLeafNode() {
        return this->children.empty();
    }

    bool has(const K& key) {
        return std::find(this->children.begin(), this->children.end(), key) != this->children.end();
    }

    const vector<pair<K, V>>& getChildren(){
        return this->children;
    }
};

class RootNode;
class ModuleNode;
class StructNode;
class MemberNode;


class StructNode: public OrderedMappingNode<string, MemberNode*> {
private:
    string struct_name;
public:
    StructNode(): OrderedMappingNode<string, MemberNode*>(STRUCT_NODE){}
    StructNode(ModuleNode* m): StructNode(){ this->setParent((Node*)m); }

    string getStructName(){ return struct_name; }
    void setStructName(string struct_name) { this->struct_name = struct_name; }

    // ModuleNode* getParentModule(){ return this->parent_module; }
    // void setParentModule(ModuleNode* parent_module){ this->parent_module = parent_module; }

};

class MemberNode: public Node{
private:
    string member_name;
    string type_str;
    uint64_t size;
    uint64_t offset;
    bool is_derived_struct;
    bool is_basetype;
    StructNode* derived_struct_pointer;
    string derived_struct_name; // for json recover;
public:
    MemberNode(): Node(MEMBER_NODE){
        this->member_name = "";
        this->type_str = "";
        this->is_derived_struct = false;
        this->is_basetype = false;
        this->derived_struct_pointer=nullptr; 
    }
    MemberNode(StructNode* s): MemberNode(){ this->setParent((Node*)s); } 
    MemberNode(string member_name, string type_str, unsigned char der_base=0): Node(MEMBER_NODE) {
        this->member_name = member_name;
        this->type_str = type_str;
        this->is_derived_struct = der_base == 0 ? false : true;
        this->is_basetype = der_base == 0 ? true : false;
    }
    MemberNode(json j): Node(MEMBER_NODE) {
        from_json(j, *this);
    }
    
    bool isLeafNode() { return true; }

    bool isDerivedStruct(){ return this->is_derived_struct; }
    void setDerivedStruct(bool is_derived_struct) { this->is_derived_struct = is_derived_struct; }

    bool isBaseType(){ return this->is_basetype; }
    void setBaseType(bool is_basetype) { this->is_basetype = is_basetype; }

    string getMemberName(){ return this->member_name; }
    void setMemberName(string member_name){ this->member_name = member_name; }

    string getTypeStr() { return this->type_str; }
    void setTypeStr(string type_str) { this->type_str = type_str; }

    uint64_t getSize() { return this->size; }
    void setSize(uint64_t size) { this->size = size; }

    uint64_t getOffset() { return this->offset; }
    void setOffset(uint64_t offset) { this->offset = offset; }

    StructNode* getDerivedStructPointer() { return this->derived_struct_pointer; }
    void setDerivedStructPointer(StructNode* derived_pointer) { this->derived_struct_pointer = derived_pointer; }

    string getDerivedStructName() { return this->derived_struct_name; }
    void setDerivedStructName(string derived_struct_name) { this->derived_struct_name = derived_struct_name; }

    string toString() {
        json res;
        this->to_json(res, *this);
        return res.dump();
    }

    void to_json(json& j, MemberNode& mn) {
        j["member_name"] = this->member_name;
        j["type_str"] = this->type_str;
        j["size"] = this->size;
        j["offset"] = this->offset;
        j["is_derived"] = this->is_derived_struct;
        j["is_basetype"] = this->is_basetype;
        j["derived_pointer"] = this->derived_struct_pointer ? this->derived_struct_pointer->getStructName() : "nullptr";
    }

    // standard `from_json` in json.hpp can't be used in class object now. 
    // so here is a user-defined from_json to load from sqlite.
    void from_json(const json& j, MemberNode& mn) {
        mn.member_name = j["member_name"];
        mn.type_str = j["type_str"];
        mn.size = j["size"];
        mn.offset = j["offset"];
        mn.is_derived_struct = j["is_derived"];
        mn.is_basetype = j["is_basetype"];
        mn.derived_struct_name = j["derived_pointer"];
    }
};



class ModuleNode: public MappingNode<string, StructNode*> {
private:
    string module_name; // using full path as module_name
public:
    ModuleNode(): MappingNode<string, StructNode*>(MODULE_NODE){}
    ModuleNode(RootNode* r): ModuleNode(){ this->setParent((Node*)r); }

    string getModuleName(){ return module_name; }
    void setModuleName(string module_name) { this->module_name = module_name; }
};

class RootNode: public MappingNode<string, ModuleNode*> {
public:
    RootNode(): MappingNode<string, ModuleNode*>(ROOT_NODE){}
};


class Tree{

};

}

// test
// int main(){
//     struct_tree::RootNode rt;
//     struct_tree::ModuleNode md;
//     struct_tree::StructNode st;
//     rt["123"] = md;
//     md["456"] = st;
//     std::cout << (int)rt["123"]["456"].getNodeType() << std::endl;
// }

#endif