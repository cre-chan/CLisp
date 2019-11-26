//
// Created by cre-chan on 19-11-25.
//

#include "stdafx.h"
#include "SymbolTable.hpp"

int main() {


    string cmd;
    auto tbl = SymbolTable<string>::NIL();

    do {
        cout << ">>>";
        cin >> cmd;

        if (cmd == "ADD") {
            string key, val;
            cin >> key;
            cin >> val;
            tbl = tbl->insert(key, make_shared<string>(val));
        } else if (cmd == "SHOW") {
            cout << *tbl << endl;
        } else if (cmd=="FIND"){
            string key;

            cin>>key;

            cout<<*(tbl->find(key))<<endl;
        }


    } while (cmd != "EXIT");


    return 0;
}