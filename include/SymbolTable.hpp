#include <utility>

//
// Created by cre-chan on 19-11-26.
//

#ifndef SYMANTICS_SYMBOLTABLE_H
#define SYMANTICS_SYMBOLTABLE_H

#include "stdafx.h"


template<class V>
class SymbolTable {
    string symbol_name;
    shared_ptr<V> value;
    shared_ptr<SymbolTable> left, right;

    SymbolTable() :
            symbol_name(), value(nullptr), left(nullptr), right(nullptr) {

    }//构造空表，有且仅有一个实例

    SymbolTable(string name, shared_ptr<V> val, shared_ptr<SymbolTable> l, shared_ptr<SymbolTable> r) :
            symbol_name(std::move(name)), value(val), left(l), right(r) {

    }//一般构造函数

    bool is_nil() const {
        return this->symbol_name.empty();
    }//比较当前对象的地址和空表的地址

public:

    static shared_ptr<SymbolTable> NIL() {
        static auto nil = shared_ptr<SymbolTable>(new SymbolTable());
        return nil;
    }

    static shared_ptr<SymbolTable>
    INTERNAL(const string &name, shared_ptr<V> val, shared_ptr<SymbolTable> l, shared_ptr<SymbolTable> r) {
        return shared_ptr<SymbolTable>(new SymbolTable(name, val, l, r));
    }

    static shared_ptr<SymbolTable> LEAF(const string &name, shared_ptr<V> val) {
        return shared_ptr<SymbolTable>(new SymbolTable(name, val, NIL(), NIL()));
    }

    shared_ptr<SymbolTable> insert(const string &name, shared_ptr<V> val) const {
        if (this->is_nil())
            return LEAF(name, std::move(val));

        //当名字已经存在时，shadow该名字
        if (name == this->symbol_name)
            return INTERNAL(name, std::move(val), this->left, this->right);
        else if (name < this->symbol_name)
            return INTERNAL(this->symbol_name, this->value, this->left->insert(name, std::move(val)), this->right);
        else
            return INTERNAL(this->symbol_name, this->value, this->left, this->right->insert(name, std::move(val)));

    }

    shared_ptr<V> find(const string &keyword) const {
        if (this->is_nil())
            throw runtime_error("Symbol not found");

        if (keyword == this->symbol_name)
            return this->value;
        else if (keyword < this->symbol_name)
            return this->left->find(keyword);
        else
            return this->right->find(keyword);
    }


    ostream &output_key_val_pair(ostream &out) const {
        if (this->is_nil())
            return out;

        out << '(' << this->symbol_name << ", " << *(this->value) << ')';

        this->left->output_key_val_pair(out);

        this->right->output_key_val_pair(out);

        return out;
    }

    SymbolTable<V> operator+(const SymbolTable<V>& other) {
        if (this->is_nil())
            return other;

        return (*this->left)+(*this->right)+(*other.insert(this->symbol_name,this->value));
    }
};

template <class T>
shared_ptr<T> operator+(shared_ptr<T> op1,shared_ptr<T> op2){
    return make_shared<T>(*op1+*op2);
}

template<class V>
ostream &operator<<(ostream &out, SymbolTable<V> &tbl) {

    out << '{';
    tbl.output_key_val_pair(out);
    out << '}';

    return out;
}

#endif //SYMANTICS_SYMBOLTABLE_H
