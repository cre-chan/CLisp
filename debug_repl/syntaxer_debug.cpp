//
// Created by cre-chan on 19-11-16.
//

#include <stdafx.h>
#include <clisp_execption.h>
#include <syntax.h>


int main() {
    unique_ptr<Expr> ast;
    while (cout << ">>>") {
        try{
            cin>>ast;
            cout << ast << endl;
        }catch(const ClispError& e){
            cerr<<e.info()<<endl;
        }catch(NormalExit& err){
            cout<<err.what()<<endl;
            break;
        }


    }

    return 0;
}