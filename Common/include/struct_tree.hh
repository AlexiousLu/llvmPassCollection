/* Header only C++ Tree */
#ifndef LLVM_MODULE_STRUCT_TREE_HH
#define LLVM_MODULE_STRUCT_TREE_HH
#include <map>
#include <string>
#include <iostream>
#include <iterator>
#include "include_llvm.hh"


//TODO: iterator.
namespace struct_tree{
using namespace std;
using namespace llvm;

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

    //TODO: iterator<typename Category, typename Tp> traverse {};
};

template <typename K, typename V>
class MappingNode: public Node{
private:
    map<K, V> children;
protected:
    MappingNode(unsigned char t): Node(t){}
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
};

template <typename K, typename V>
class OrderedMappingNode: public Node{
private:
    vector<pair<K, V>> children;
protected:
    OrderedMappingNode(unsigned char t): Node(t){}
public:
    V& operator[](const K& key){
        for(auto it = this->children.begin(); it != this->children.end(); it++){
            if(it->first == key){
                return it->second;
            }
        }
        this->children.push_back({key, V{}});
        // if(is_base_of<V, Node>::value){
            
        // }
        return std::prev(this->children.end())->second;
    }
    V& getChild(const K& key){
        return (*this)[key];
    }

    bool isLeafNode() {
        return this->children.empty();
    }
};

class RootNode;
class ModuleNode;
class StructNode;
class MemberNode;

class MemberNode: public Node{
private:
    string member_name;
    string type_str;
    uint64_t size;
    uint64_t offset;
    bool is_derived;
    bool is_basetype;
    StructNode* derived_pointer;
public:
    MemberNode(): Node(MEMBER_NODE){
        this->member_name = "";
        this->type_str = "";
        this->is_derived = false;
        this->is_basetype = false;
        this->derived_pointer=nullptr; 
    }
    MemberNode(StructNode* s): MemberNode(){ this->setParent((Node*)s); } 
    MemberNode(string member_name, string type_str, unsigned char der_base=0): Node(MEMBER_NODE) {
        this->member_name = member_name;
        this->type_str = type_str;
        this->is_derived = der_base == 0 ? false : true;
        this->is_basetype = der_base == 0 ? true : false;
    }
    
    bool isLeafNode() { return true; }

    bool isDerived(){ return this->is_derived; }
    void setDerived(bool is_derived) { this->is_derived = is_derived; }

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

    StructNode* getDerivedPointer() { return this->derived_pointer; }
    void setDerivedPointer(StructNode* derived_pointer) { this->derived_pointer = derived_pointer; }

};

class StructNode: public OrderedMappingNode<string, MemberNode> {
private:
    string struct_name;
    // vector<DIVariable> from_variable; //TODO: pending
    // ModuleNode* parent_module; // ? TODO: why do I set this field?
public:
    StructNode(): OrderedMappingNode<string, MemberNode>(STRUCT_NODE){}
    StructNode(ModuleNode* m): StructNode(){ this->setParent((Node*)m); }

    string getStructName(){ return struct_name; }
    void setStructName(string struct_name) { this->struct_name = struct_name; }

    // ModuleNode* getParentModule(){ return this->parent_module; }
    // void setParentModule(ModuleNode* parent_module){ this->parent_module = parent_module; }
};


class ModuleNode: public MappingNode<string, StructNode> {
private:
    string module_name; // using full path as module_name
public:
    ModuleNode(): MappingNode<string, StructNode>(MODULE_NODE){}
    ModuleNode(RootNode* r): ModuleNode(){ this->setParent((Node*)r); }

    string getModuleName(){ return module_name; }
    void setModuleName(string module_name) { this->module_name = module_name; }
};

class RootNode: public MappingNode<string, ModuleNode> {
public:
    RootNode(): MappingNode<string, ModuleNode>(ROOT_NODE){}
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