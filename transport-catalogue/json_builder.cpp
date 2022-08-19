#include "json_builder.h"

using namespace std;

namespace json {
    
Node& Builder::Build() {
    if(!nodes_stack_.empty()) {
        throw  logic_error{"Build: " + to_string(__LINE__)};
    }
    if(root_.IsNull()) {
        throw  logic_error{"Build: " + to_string(__LINE__)};
    }
    return root_;
}    

KeyItemContext Builder::Key(std::string key) {
    if (!nodes_stack_.size() || !nodes_stack_.back()->IsMap() ) {
        throw  logic_error{"Build: " + to_string(__LINE__)};
    }
    Dict& cur = const_cast<Dict&>(nodes_stack_.back()->AsMap());
    cur.emplace(key, key);
    nodes_stack_.push_back(&cur.at(key));
    KeyItemContext a(*this);
    return a;
}

Builder& Builder::Value(Node value) {
    InsertNode(value);
    return *this;
}

ArrayItemContext Builder::StartArray() {
    nodes_stack_.push_back(InsertNode(Array{}));
    ArrayItemContext a(*this);
    return a;
}

Builder& Builder::EndArray() {
    if (!nodes_stack_.size() || !nodes_stack_.back()->IsArray() ) {
        throw  logic_error{"Build: " + to_string(__LINE__)};
    }
    nodes_stack_.pop_back();
    return *this;
}

DictItemContext Builder::StartDict() {
    nodes_stack_.push_back(InsertNode(Dict{}));
    DictItemContext a(*this);
    return a;
}

Builder& Builder::EndDict() {
    if (!nodes_stack_.size() || !nodes_stack_.back()->IsMap() ) {
        throw  logic_error{"Build: " + to_string(__LINE__)};
    }
    nodes_stack_.pop_back();
    return *this;
}
    
Node* Builder::InsertNode(Node node) {
    if (nodes_stack_.size() == 0 && root_.IsNull()) {
        root_ = Node(node);
        return &root_;
    }
    else if (nodes_stack_.size() && nodes_stack_.back()->IsArray()) {
        Array& current = const_cast<Array&>(nodes_stack_.back()->AsArray());
        current.push_back(node);
        return &current.back();
    }
    else if (nodes_stack_.size() && nodes_stack_.back()->IsString()) {
        const std::string key = nodes_stack_.back()->AsString();
        nodes_stack_.pop_back();
        if (nodes_stack_.back()->IsMap()) {
            Dict& current = const_cast<Dict&>(nodes_stack_.back()->AsMap());
            current[key] = node;
            return &current[key];
        }
    }
    throw  logic_error{"Build: " + to_string(__LINE__)};
    return {};
}
//--------------------------------------------------------------    
KeyItemContext DictItemContext::Key(std::string st) {
    return builder_.Key(st);
} 
Builder& DictItemContext::EndDict() {
    return builder_.EndDict();    
}    
//-------------------------------------------------------------- 
ValueAfterKeyContext KeyItemContext::Value(Node node) {
    ValueAfterKeyContext a(builder_.Value(node));
    return a;
}
DictItemContext KeyItemContext::StartDict() {
    return builder_.StartDict();
} 
ArrayItemContext KeyItemContext::StartArray() {
    ArrayItemContext a(builder_.StartArray());
    return a;
}
//--------------------------------------------------------------     
KeyItemContext ValueAfterKeyContext::Key(std::string st) {
    return builder_.Key(st);
}
Builder& ValueAfterKeyContext::EndDict() {
    return builder_.EndDict();
}  
//-------------------------------------------------------------- 
ValueAfterArrayContext ArrayItemContext::Value(Node node) {
    ValueAfterArrayContext a(builder_.Value(node));
    return a;
}
DictItemContext ArrayItemContext::StartDict() {
    DictItemContext a(builder_.StartDict());
    return a;
}
ArrayItemContext ArrayItemContext::StartArray() {
    ArrayItemContext a(builder_.StartArray());
    return a;
}
Builder&  ArrayItemContext::EndArray() {
    return builder_.EndArray();
}  

//--------------------------------------------------------------     
ValueAfterArrayContext ValueAfterArrayContext::Value(Node node) {
    ValueAfterArrayContext a(builder_.Value(node));
    return a;
}
DictItemContext ValueAfterArrayContext::StartDict() {
    DictItemContext a(builder_.StartDict());
    return a;    
}
ArrayItemContext ValueAfterArrayContext::StartArray() {
    ArrayItemContext a(builder_.StartArray());
    return a;    
}
Builder& ValueAfterArrayContext::EndArray() {
    return builder_.EndArray();    
}
}//namespace json 