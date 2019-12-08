//
// Created by cre-chan on 19-11-16.
//

#include <stdafx.h>
#include <syntax.h>


int main(){
//    vector<unique_ptr<Expr>> opers;
//    opers.push_back(Expr::IdExpr("well"));
//    opers.push_back(Expr::IntExpr(-200));
//    auto simple_program=Expr::IfExpr(
//            Expr::BooleanExpr(true),
//            Expr::PlusExpr(std::move(opers)),
//            Expr::IdExpr("a")
//            );
//
//    cout<<(*simple_program);

        while (true) {
            Syntaxer temp;
            cout << ">>>";
            cout << *temp.parseExpr() << endl;
        }

    return 0;
}