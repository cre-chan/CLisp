//
// Created by cre-chan on 19-12-14.
//

#ifndef SEMANTICS_CLISP_UTILITY_HPP
#define SEMANTICS_CLISP_UTILITY_HPP

#include <iostream>
#include <memory>
#include <forward_list>

template <class T>
std::ostream& operator<<(std::ostream& out,std::unique_ptr<T>& a){
    return out<<*a;
}

template <class T>
std::forward_list<T> operator+(std::forward_list<T> l1,std::forward_list<T> l2){
    while(!l1.empty()){
        l2.push_front(std::move(l1.front()));
        l1.pop_front();
    }

    return std::move(l2);
}




#endif //SEMANTICS_CLISP_UTILITY_HPP
