/* Header only C++ Tree */
#ifndef LLVM_MODULE_TREE_HH
#define LLVM_MODULE_TREE_HH
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
    Node(unsigned char type): node_type(type){}

    Node* getParent(){ return this->parent; }
    void setParent(Node* node) { this->parent = node; }

    bool virtual isLeafNode();

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
        for(pair<K, V> kv : this->children){
            if(kv.first == key){
                return kv.second;
            }
        }
        return this->children.end();
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
    
    bool isLeafNode() {
        return true;
    }
};

class StructNode: public OrderedMappingNode<string, MemberNode> {
private:
    string struct_name;
    vector<DIVariable> from_variable; //TODO: pending
public:
    StructNode(): OrderedMappingNode<string, MemberNode>(STRUCT_NODE){}
    StructNode(ModuleNode* m): StructNode(){ this->setParent((Node*)m); }
    
};


class ModuleNode: public MappingNode<string, StructNode> {
public:
    ModuleNode(): MappingNode<string, StructNode>(STRUCT_NODE){}
    ModuleNode(RootNode* r): ModuleNode(){ this->setParent((Node*)r); }
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