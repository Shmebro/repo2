#include <iostream>
#include <string>
using namespace std;

// Базовые классы
struct Expression {
    virtual ~Expression() = default;
    virtual double evaluate() const = 0;
    virtual Expression* transform(Transformer* tr) const = 0;
};

struct Number : Expression {
    double value_;
    Number(double value) : value_(value) {}
    double evaluate() const override { return value_; }
    Expression* transform(Transformer* tr) const override;
};

struct BinaryOperation : Expression {
    enum { PLUS = '+', MINUS = '-', DIV = '/', MUL = '*' };
    Expression* left_;
    Expression* right_;
    int op_;

    BinaryOperation(Expression* left, int op, Expression* right)
        : left_(left), op_(op), right_(right) {
        // Убираем assert, так как проверка должна быть в вызывающем коде
    }

    ~BinaryOperation() override {
        delete left_;
        delete right_;
    }

    double evaluate() const override {
        double l = left_->evaluate();
        double r = right_->evaluate();
        switch (op_) {
        case PLUS: return l + r;
        case MINUS: return l - r;
        case MUL: return l * r;
        case DIV: return l / r;
        }
        return 0.0;
    }

    Expression* transform(Transformer* tr) const override;
};

struct FunctionCall : Expression {
    string name_;
    Expression* arg_;

    FunctionCall(string name, Expression* arg)
        : name_(name), arg_(arg) {}

    ~FunctionCall() override {
        delete arg_;
    }

    double evaluate() const override {
        if (name_ == "sqrt") return sqrt(arg_->evaluate());
        else if (name_ == "abs") return fabs(arg_->evaluate());
        return 0.0;
    }

    Expression* transform(Transformer* tr) const override;
};

struct Variable : Expression {
    string name_;
    Variable(string name) : name_(name) {}
    double evaluate() const override { return 0.0; }
    Expression* transform(Transformer* tr) const override;
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
        Expression* newLeft = binop->left_->transform(this);
        Expression* newRight = binop->right_->transform(this);
        return new BinaryOperation(newLeft, binop->op_, newRight);
    }

    Expression* transformFunctionCall(FunctionCall const* fcall) override {
        Expression* newArg = fcall->arg_->transform(this);
        return new FunctionCall(fcall->name_, newArg);
    }

    Expression* transformVariable(Variable const* var) override {
        return new Variable(var->name_);
    }
};

int main() {
    
    Number* n32 = new Number(32.0);
    Number* n16 = new Number(16.0);
    BinaryOperation* minus = new BinaryOperation(n32, BinaryOperation::MINUS, n16);
    FunctionCall* callSqrt = new FunctionCall("sqrt", minus);
    Variable* var = new Variable("var");
    BinaryOperation* mult = new BinaryOperation(var, BinaryOperation::MUL, callSqrt);
    FunctionCall* callAbs = new FunctionCall("abs", mult);

    
    CopySyntaxTree CST;
    Expression* newExpr = callAbs->transform(&CST);

    
    cout << "Original result: " << callAbs->evaluate() << endl;
    cout << "Copied result: " << newExpr->evaluate() << endl;

    
    delete n32;
    delete n16;
    delete minus;
    delete callSqrt;
    delete var;
    delete mult;
    delete callAbs;
    delete newExpr;

    return 0;
}