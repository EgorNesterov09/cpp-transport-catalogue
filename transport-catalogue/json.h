#pragma once
/*
 * Место для вашей JSON-библиотеки
 */

#include "transport_catalogue.h"
#include <string>
#include <deque>
#include <iostream>
#include <string_view>
#include <map>
#include <vector>
#include <variant>

namespace json {

class Node;
// Сохраните объявления Dict и Array без изменения
using Dict = std::map<std::string, Node>;
using Array = std::vector<Node>;
using Value = std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>;
// Эта ошибка должна выбрасываться при ошибках парсинга JSON
class ParsingError : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

class Node {
public:
    Node();
    Node (const Dict& val);
    Node (const Array& val);
    Node(std::nullptr_t val);
    Node(bool val);
    Node(int val);
    Node(double val);
    Node(const std::string& val);
    const Value& GetValue() const; 
    //Следующие методы Node сообщают, хранится ли внутри значение некоторого типа
    bool IsInt() const;
    bool IsDouble() const; //Возвращает true, если в Node хранится int либо double.
    bool IsPureDouble() const; //Возвращает true, если в Node хранится double.
    bool IsBool() const;
    bool IsString() const;
    bool IsNull() const;
    bool IsArray() const;
    bool IsMap() const;
    //Ниже перечислены методы, которые возвращают хранящееся внутри Node значение заданного типа. Если внутри содержится значение другого типа, должно выбрасываться исключение std::logic_error.
    int AsInt() const;
    bool AsBool() const;
    double AsDouble() const; //Возвращает значение типа double, если внутри хранится double либо int. В последнем случае возвращается приведённое в double значение.
    const std::string& AsString() const;
    const Array& AsArray() const;
    const Dict& AsMap() const;
    
private:
    Value value_ ;
};

class Document {
public:
    Document();
    Document(Node root);

    const Node& GetRoot() const;

private:
    Node root_;
};

Document Load(std::istream& input);
    
void PrintValue(int i, std::ostream& out);
void PrintValue(double d, std::ostream& out);
void PrintValue(std::nullptr_t, std::ostream& out);
void PrintValue(const Array& array, std::ostream& out);
void PrintValue(const Dict& dict, std::ostream& out);
void PrintValue(const std::string& str, std::ostream& out);
void PrintValue(const bool bl, std::ostream& out);
void PrintNode(const Node& node, std::ostream& out);

void Print(const Document& doc, std::ostream& output);

Node LoadNumber(std::istream& input);
Node LoadString(std::istream& input);
    
bool operator==(const Node lhs, const Node rhs);
bool operator!=(const Node lhs, const Node rhs);  
bool operator==(const Document lhs, const Document rhs);
bool operator!=(const Document lhs, const Document rhs); 
}  // namespace json