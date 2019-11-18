#include <utility>

//
// Created by cre-chan on 19-11-16.
//

#include <stdafx.h>
#include "syntax.h"

Expr::expr_type Expr::getTag() const {
    switch (content.index()) {
        case 0:
            return cond;
        case 1:
            return neg;
        case 2:
            return conj;
        case 3:
            return disj;
        case 4:
            return id;
        case 5:
            return boolean;
        case 6:
            return interger;
        case 7:
            return apply;
        case 8:
            return def;
        case 9:
            switch (std::get<arith_expr>(content).tag) {
                case op_type::times:
                    return times;
                case op_type::plus:
                    return plus;
                case op_type::minus:
                    return minus;
                case op_type::div:
                    return div;
            }
        case 10:
            return def_with_expr;
        case 11:
            switch (std::get<binary_relation>(content).tag){
                case binary_relation::rel_type ::eq:
                    return eq;
                case binary_relation::rel_type ::le:
                    return le;
                case binary_relation::rel_type ::lt:
                    return lt;
                case binary_relation::rel_type ::ge:
                    return ge;
                case binary_relation::rel_type ::gt:
                    return gt;
            }
        default:
            throw exception();//这种情况出现了我也不知道怎么办，就让它证明失败好了
    }
}


Expr::Expr(Expr::expr_type t, bool val) : content(bool_expr{val}) {}

Expr::Expr(Expr::expr_type t, int val) : content(int_expr{val}) {}

Expr::Expr(Expr::expr_type t, const string &s) : content(id_expr{s}) {}

Expr::Expr(Expr::expr_type t, vector<unique_ptr<Expr>> list) {
    switch (t) {
        case apply:
            content = apply_expr{std::move(list)};
            break;
        case plus:
            content = variant_t(arith_expr{arith_expr::op_type::plus, std::move(list)});
            break;
        case minus:
            content = arith_expr{arith_expr::op_type::minus, std::move(list)};
            break;
        case times:
            content = arith_expr{arith_expr::op_type::times, std::move(list)};
            break;
        case div:
            content = arith_expr{arith_expr::op_type::div, std::move(list)};
            break;
        default:
            throw exception();//以防万一
    }
}

Expr::Expr(Expr::expr_type t, unique_ptr<Expr> expr) : content(not_expr{std::move(expr)}) {}

Expr::Expr(Expr::expr_type t, unique_ptr<Expr> expr1, unique_ptr<Expr> expr2) {
    switch (t) {
        case disj:
            content.emplace<or_expr>(or_expr{std::move(expr1), std::move(expr2)});
            break;
        case conj:
            content.emplace<and_expr>(and_expr{std::move(expr1), std::move(expr2)});
            break;
        case eq:
            content.emplace<binary_relation>(
                    binary_relation{binary_relation::rel_type::eq, std::move(expr1), std::move(expr2)});
            break;
        case le:
            content.emplace<binary_relation>(
                    binary_relation{binary_relation::rel_type::le, std::move(expr1), std::move(expr2)});
            break;
        case lt:
            content.emplace<binary_relation>(
                    binary_relation{binary_relation::rel_type::lt, std::move(expr1), std::move(expr2)});
            break;
        case ge:
            content.emplace<binary_relation>(
                    binary_relation{binary_relation::rel_type::ge, std::move(expr1), std::move(expr2)});
            break;
        case gt:
            content.emplace<binary_relation>(
                    binary_relation{binary_relation::rel_type::gt, std::move(expr1), std::move(expr2)});
            break;
        default:
            throw exception();
    }
}

Expr::Expr(Expr::expr_type t, vector<Expr::id_expr> arglist, unique_ptr<Expr> body) :
        content(define_expr{std::move(arglist), std::move(body)}) {}

Expr::Expr(Expr::expr_type t, unique_ptr<Expr> expr1, unique_ptr<Expr> expr2, unique_ptr<Expr> expr3) :
        content(if_caluse{std::move(expr1), std::move(expr2), std::move(expr3)}) {}

Expr::Expr(Expr::expr_type t, Expr::define_expr definePart, unique_ptr<Expr> expr) :
        content(define_with_expr{std::move(definePart), std::move(expr)}) {

}







/*
 * 快速构建包裹在unique_ptr里的表达式类型
 * */
unique_ptr<Expr> Expr::EqExpr(unique_ptr<Expr> expr1, unique_ptr<Expr> expr2) {
    return unique_ptr<Expr>(new Expr(expr_type::eq,std::move(expr1),std::move(expr2)));
}

unique_ptr<Expr> Expr::LeExpr(unique_ptr<Expr> expr1, unique_ptr<Expr> expr2) {
    return unique_ptr<Expr>(new Expr(expr_type::le,std::move(expr1),std::move(expr2)));
}

unique_ptr<Expr> Expr::LtExpr(unique_ptr<Expr> expr1, unique_ptr<Expr> expr2) {
    return unique_ptr<Expr>(new Expr(expr_type::lt,std::move(expr1),std::move(expr2)));
}

unique_ptr<Expr> Expr::GeExpr(unique_ptr<Expr> expr1, unique_ptr<Expr> expr2) {
    return unique_ptr<Expr>(new Expr(expr_type::ge,std::move(expr1),std::move(expr2)));
}

unique_ptr<Expr> Expr::GtExpr(unique_ptr<Expr> expr1, unique_ptr<Expr> expr2) {
    return unique_ptr<Expr>(new Expr(expr_type::gt,std::move(expr1),std::move(expr2)));
}

unique_ptr<Expr> Expr::IfExpr(unique_ptr<Expr> condition, unique_ptr<Expr> expr1, unique_ptr<Expr> expr2) {
    return unique_ptr<Expr>(new Expr(cond, std::move(condition), std::move(expr1), std::move(expr2)));
}

unique_ptr<Expr> Expr::NegExpr(unique_ptr<Expr> expr) {
    return unique_ptr<Expr>(new Expr(neg, std::move(expr)));
}

unique_ptr<Expr> Expr::AndExpr(unique_ptr<Expr> expr1, unique_ptr<Expr> expr2) {
    return unique_ptr<Expr>(new Expr(conj, std::move(expr1), std::move(expr2)));
}

unique_ptr<Expr> Expr::OrExpr(unique_ptr<Expr> expr1, unique_ptr<Expr> expr2) {
    return unique_ptr<Expr>(new Expr(disj, std::move(expr1), std::move(expr2)));
}

unique_ptr<Expr> Expr::IdExpr(const string &s) {
    return unique_ptr<Expr>(new Expr(id, s));
}

unique_ptr<Expr> Expr::BooleanExpr(bool b) {
    return unique_ptr<Expr>(new Expr(boolean, b));
}

unique_ptr<Expr> Expr::IntExpr(int i) {
    return unique_ptr<Expr>(new Expr(interger, i));
}

unique_ptr<Expr> Expr::ApplyExpr(vector<unique_ptr<Expr>> list) {
    return unique_ptr<Expr>(new Expr(apply, std::move(list)));
}

unique_ptr<Expr> Expr::DefExpr(vector<id_expr> arglist, unique_ptr<Expr> body) {
    return unique_ptr<Expr>(new Expr(def, std::move(arglist), std::move(body)));
}

unique_ptr<Expr> Expr::PlusExpr(vector<unique_ptr<Expr>> opers) {
    return unique_ptr<Expr>(new Expr(plus, std::move(opers)));
}

unique_ptr<Expr> Expr::MinusExpr(vector<unique_ptr<Expr>> opers) {
    return unique_ptr<Expr>(new Expr(minus, std::move(opers)));
}

unique_ptr<Expr> Expr::MultExpr(vector<unique_ptr<Expr>> opers) {
    return unique_ptr<Expr>(new Expr(times, std::move(opers)));
}

unique_ptr<Expr> Expr::DivExpr(vector<unique_ptr<Expr>> opers) {
    return unique_ptr<Expr>(new Expr(div, std::move(opers)));
}

unique_ptr<Expr> Expr::DefWithExpr(Expr::define_expr defition, unique_ptr<Expr> expr) {
    return unique_ptr<Expr>(new Expr(def_with_expr, std::move(defition), std::move(expr)));
}







ostream &operator<<(ostream &out, const Expr &self) {
    return self.prettyPrint(out, 0);
}

ostream& Expr::relaional_output(ostream &out, uint level, const string& op_name) const {
    auto &minus = std::get<Expr::binary_relation>(content);
    indentation(out, level) <<op_name<< '(' << endl;
    minus.expr1->prettyPrint(out, level + 1) << endl;
    minus.expr2->prettyPrint(out, level + 1) << endl;
    indentation(out, level) << ")";
    return out;
}


template <class primary_T>
ostream& Expr::primary_output(ostream &out, uint level, const string &val_name,std::function<string(const primary_T&)> getter) const {
    auto &id = std::get<primary_T>(content);
    indentation(out, level) << val_name<<'(';
    out << getter(id);
    out << ")";
    return out;
}

Expr::variant_t &Expr::operator*() {
    return content;
}

ostream &Expr::indentation(ostream &out, uint level) const {
    for (int i = 0; i < level; i++)
        out << '\t';
    return out;
}

ostream &Expr::prettyPrint(ostream &out, uint level) const {
    auto &self = (*this);

    switch (self.getTag()) {
        case Expr::cond: {
            auto &cond = std::get<Expr::if_caluse>(self.content);
            indentation(out, level) << "if(" << endl;
            cond.condition->prettyPrint(out, level + 1) << endl;
            cond.expr1->prettyPrint(out, level + 1) << endl;
            cond.expr2->prettyPrint(out, level + 1) << endl;
            indentation(out, level) << ")";
            return out;
        }
        case Expr::boolean:
            return primary_output<Expr::bool_expr>(out,level,"boolean",[](auto b){return (b.val?"true":"false");});
        case Expr::id:
            return primary_output<Expr::id_expr>(out,level,"id",[](auto id){return id.name;});
        case Expr::interger:
            return primary_output<Expr::int_expr>(out,level,"int",[](auto i){return to_string(i.val);});
        case Expr::neg: {
            auto &neg = std::get<Expr::not_expr>(self.content);
            indentation(out, level) << "not(" << endl;
            neg.expr->prettyPrint(out, level + 1) << endl;
            indentation(out, level) << ")";
            return out;
        }
        case Expr::conj: {
            auto &conj = std::get<Expr::and_expr>(self.content);
            indentation(out, level) << "and(" << endl;
            conj.expr1->prettyPrint(out, level + 1) << endl;
            conj.expr2->prettyPrint(out, level + 1) << endl;
            indentation(out, level) << ")";
            return out;
        }
        case Expr::disj: {
            auto &disj = std::get<Expr::or_expr>(self.content);
            indentation(out, level) << "or(" << endl;
            disj.expr1->prettyPrint(out, level + 1) << endl;
            disj.expr2->prettyPrint(out, level + 1) << endl;
            indentation(out, level) << ')';
            return out;
        }
        case Expr::apply: {
            auto &apply = std::get<Expr::apply_expr>(self.content);
            indentation(out, level) << "apply(" << endl;
            for (auto &item:apply.list)
                item->prettyPrint(out, level + 1) << endl;
            indentation(out, level) << ")";
            return out;
        }
        case Expr::def: {
            auto &def = std::get<Expr::define_expr>(self.content);
            indentation(out, level) << "define(";
            out << '(';
            for (auto &item:def.arglist)
                out << item.name << " ";
            out << ')' << endl;
            def.body->prettyPrint(out, level + 1) << endl;
            indentation(out, level) << ")";
            return out;
        }
        case Expr::plus: {
            auto &plus = std::get<Expr::arith_expr>(self.content);
            indentation(out, level) << "plus(" << endl;
            for (auto &item:plus.operlist)
                item->prettyPrint(out, level + 1) << endl;
            indentation(out, level) << ")";
            return out;
        }
        case Expr::minus: {
            auto &minus = std::get<Expr::arith_expr>(self.content);
            indentation(out, level) << "minus(" << endl;
            for (auto &item:minus.operlist)
                item->prettyPrint(out, level + 1) << endl;
            indentation(out, level) << ")";
            return out;
        }
        case Expr::times: {
            auto &times = std::get<Expr::arith_expr>(self.content);
            indentation(out, level) << "times(" << endl;
            for (auto &item:times.operlist)
                item->prettyPrint(out, level + 1) << endl;
            indentation(out, level) << ")";
            return out;
        }
        case Expr::div: {
            auto &div = std::get<Expr::arith_expr>(self.content);
            indentation(out, level) << "divide(" << endl;
            for (auto &item:div.operlist)
                item->prettyPrint(out, level + 1) << endl;
            indentation(out, level) << ")";
            return out;
        }
        case Expr::def_with_expr: {
            auto &def_with = std::get<Expr::define_with_expr>(self.content);
            indentation(out, level) << "define(" << endl;
            indentation(out, level + 1) << "(";
            for (auto &id:def_with.def_part.arglist)
                out << id.name << ' ';
            out << ')' << endl;
            def_with.def_part.body->prettyPrint(out, level + 1);
            indentation(out, level) << "in" << endl;
            def_with.expr->prettyPrint(out, level + 1) << endl;
            indentation(out, level) << ')';
            return out;
        }
        case eq:
            return relaional_output(out,level,"eq");
        case le:
            return relaional_output(out,level,"le");
        case ge:
            return relaional_output(out,level,"ge");
        case lt:
            return relaional_output(out,level,"lt");
        case gt:
            return relaional_output(out,level,"gt");
    }
}


