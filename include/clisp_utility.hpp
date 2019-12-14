//
// Created by cre-chan on 19-12-14.
//

#ifndef SEMANTICS_CLISP_UTILITY_HPP
#define SEMANTICS_CLISP_UTILITY_HPP

#include <iostream>
#include <memory>

template <class T>
std::ostream& operator<<(std::ostream& out,std::unique_ptr<T>& a){
    return out<<*a;
}



#endif //SEMANTICS_CLISP_UTILITY_HPP
