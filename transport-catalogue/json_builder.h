#pragma once
#include "json.h"
#include <vector>

namespace json {
    
class DictItemContext;   
class KeyItemContext;
class ValueAfterKeyContext;    
class ArrayItemContext;
class ValueAfterArrayContext;       
    
class Builder {
private:
    Node root_;
    std::vector<Node*> nodes_stack_;
public:
    Builder() = default;
    DictItemContext StartDict();
    Builder& EndDict();
    ArrayItemContext StartArray();
    Builder& EndArray();
    KeyItemContext Key(std::string);
    Builder& Value(Node);
    Node& Build();
    Node* InsertNode(Node node);
};
    
class DictItemContext {
public:
    DictItemContext(Builder& builder) : builder_(builder) {}
    KeyItemContext Key(std::string); 
    Builder& EndDict();
private:
    Builder& builder_;    
};
    
class KeyItemContext {
public:
    KeyItemContext(Builder& builder) : builder_(builder) {}
    ValueAfterKeyContext Value(Node);
    DictItemContext StartDict(); 
    ArrayItemContext StartArray();
private:
    Builder& builder_;    
};
    
class ValueAfterKeyContext {
public:
    ValueAfterKeyContext(Builder& builder) : builder_(builder) {}
    KeyItemContext Key(std::string); 
    Builder& EndDict();
private:
    Builder& builder_;    
};  
    
class ArrayItemContext {
public:
    ArrayItemContext(Builder& builder) : builder_(builder) {}
    ValueAfterArrayContext Value(Node);
    DictItemContext StartDict();
    ArrayItemContext StartArray();
    Builder& EndArray();
private:
    Builder& builder_;    
};
 
class ValueAfterArrayContext {
public:
    ValueAfterArrayContext(Builder& builder) : builder_(builder) {}
    ValueAfterArrayContext Value(Node); 
    DictItemContext StartDict();
    ArrayItemContext StartArray();
    Builder& EndArray();
private:
    Builder& builder_;    
};    

}// namespace json 