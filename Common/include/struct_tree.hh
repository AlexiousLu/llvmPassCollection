/* Header only C++ Tree */
#ifndef LLVM_MODULE_TREE_HH
#define LLVM_MODULE_TREE_HH
#include <map>
#include <string>
#include <iostream>
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

    };
public:
    Node(unsigned char type): node_type(type){}

    Node* getParent(){ return this->parent; }
    void setParent(Node* node) { this->parent = node; }

    unsigned char getNodeType(){ return this->node_type; }

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
};

class TypeNode: public Node{
public:
    TypeNode(): Node(4){}
};

class StructNode: public OrderedMappingNode<string, Node> {
public:
    StructNode(): OrderedMappingNode<string, Node>(3){}
};


class ModuleNode: public MappingNode<string, StructNode> {
public:
    ModuleNode(): MappingNode<string, StructNode>(2){}
};

class RootNode: public MappingNode<string, ModuleNode> {
public:
    RootNode(): MappingNode<string, ModuleNode>(1){}
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