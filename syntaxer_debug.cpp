//
// Created by cre-chan on 19-11-16.
//

#include <stdafx.h>
#include <syntax.h>


int main() {
    unique_ptr<Expr> ast;
    while (cout<<">>>",cin>>ast)
        cout<<*ast<<endl;

    return 0;
}