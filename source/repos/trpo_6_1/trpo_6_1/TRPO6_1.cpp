#include <iostream>
#include <string>
#include <cmath>
using namespace std;

// Базовые классы
struct Expression {
    virtual ~Expression() = default;
    virtual double evaluate() const = 0;
    virtual Expression* transform(class Transformer* tr) const = 0;
};

struct Number : Expression {
    double value_;
    Number(double value) : value_(value) {}
    double evaluate() const override { return value_; }
    Expression* transform(class Transformer* tr) const override;
};

struct BinaryOperation : Expression {
    enum { PLUS = '+', MINUS = '-', DIV = '/', MUL = '*' };
    Expression const* left_;
    Expression const* right_;
    int op_;
    BinaryOperation(Expression const* left, int op, Expression const* right)
        : left_(left), op_(op), right_(right) {}
    ~BinaryOperation() { delete left_; delete right_; }
    double evaluate() const override {
        double left = left_->evaluate();
        double right = right_->evaluate();
        switch (op_) {
        case PLUS: return left + right;
        case MINUS: return left - right;
        case DIV: return left / right;
        case MUL: return left * right;
        }
        return 0.0;
    }
    Expression* transform(class Transformer* tr) const override;
};

struct FunctionCall : Expression {
    string const name_;
    Expression const* arg_;
    FunctionCall(string const& name, Expression const* arg)
        : name_(name), arg_(arg) {}
    ~FunctionCall() { delete arg_; }
    double evaluate() const override {
        double val = arg_->evaluate();
        if (name_ == "sqrt") return sqrt(val);
        else if (name_ == "abs") return fabs(val);
        return 0.0;
    }
    Expression* transform(class Transformer* tr) const override;
};

struct Variable : Expression {
    string const name_;
    Variable(string const name) : name_(name) {}
    double evaluate() const override { return 0.0; }
    Expression* transform(class Transformer* tr) const override;
};

// Интерфейс Transformer
struct Transformer {
    virtual ~Transformer() = default;
    virtual Expression* transformNumber(Number const*) = 0;
    virtual Expression* transformBinaryOperation(BinaryOperation const*) = 0;
    virtual Expression* transformFunctionCall(FunctionCall const*) = 0;
    virtual Expression* transformVariable(Variable const*) = 0;
};

// Реализация методов transform
Expression* Number::transform(Transformer* tr) const {
    return tr->transformNumber(this);
}

Expression* BinaryOperation::transform(Transformer* tr) const {
    return tr->transformBinaryOperation(this);
}

Expression* FunctionCall::transform(Transformer* tr) const {
    return tr->transformFunctionCall(this);
}

Expression* Variable::transform(Transformer* tr) const {
    return tr->transformVariable(this);
}

// Класс CopySyntaxTree
struct CopySyntaxTree : Transformer {
    Expression* transformNumber(Number const* number) override {
        return new Number(number->value_);
    }

    Expression* transformBinaryOperation(BinaryOperation const* binop) override {
        Expression* left = binop->left_->transform(this);
        Expression* right = binop->right_->transform(this);
        return new BinaryOperation(left, binop->op_, right);
    }

    Expression* transformFunctionCall(FunctionCall const* fcall) override {
        Expression* arg = fcall->arg_->transform(this);
        return new FunctionCall(fcall->name_, arg);
    }

    Expression* transformVariable(Variable const* var) override {
        return new Variable(var->name_);
    }
};

int main() {
    // Пример использования CopySyntaxTree
    Number* n32 = new Number(32.0);
    Number* n16 = new Number(16.0);
    BinaryOperation* minus = new BinaryOperation(n32, BinaryOperation::MINUS, n16);
    FunctionCall* callSqrt = new FunctionCall("sqrt", minus);
    Variable* var = new Variable("var");
    BinaryOperation* mult = new BinaryOperation(var, BinaryOperation::MUL, callSqrt);
    FunctionCall* callAbs = new FunctionCall("abs", mult);

    CopySyntaxTree CST;
    Expression* newExpr = callAbs->transform(&CST);

    cout << "Original expression result: " << callAbs->evaluate() << endl;
    cout << "Copied expression result: " << newExpr->evaluate() << endl;

    // Освобождение памяти
    delete n32; delete n16; delete minus; delete callSqrt;
    delete var; delete mult; delete callAbs; delete newExpr;

    return 0;
}