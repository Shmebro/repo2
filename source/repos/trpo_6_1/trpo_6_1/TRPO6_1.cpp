#include <iostream>
#include <cmath>
#include <string>

using namespace std;

// Forward declarations
struct Expression;
struct Number;
struct BinaryOperation;
struct FunctionCall;
struct Variable;

// Интерфейс Transformer с абстрактными методами
struct Transformer {
    virtual ~Transformer() = default;
    virtual Expression* transformNumber(Number const*) = 0;
    virtual Expression* transformBinaryOperation(BinaryOperation const*) = 0;
    virtual Expression* transformFunctionCall(FunctionCall const*) = 0;
    virtual Expression* transformVariable(Variable const*) = 0;
};

// Базовый класс Expression
struct Expression {
    virtual ~Expression() = default;
    virtual double evaluate() const = 0;
    virtual Expression* transform(Transformer* tr) const = 0;
};

// Класс Number
struct Number : Expression {
    explicit Number(double value) : value_(value) {}

    double value() const { return value_; }
    double evaluate() const override { return value_; }

    Expression* transform(Transformer* tr) const override {
        return tr->transformNumber(this);
    }

private:
    double value_;
};

// Класс BinaryOperation
struct BinaryOperation : Expression {
    enum Operation { PLUS, MINUS, DIV, MUL };

    BinaryOperation(Expression* left, Operation op, Expression* right)
        : left_(left), op_(op), right_(right) {}

    ~BinaryOperation() override {
        delete left_;
        delete right_;
    }

    Expression* left() const { return left_; }
    Expression* right() const { return right_; }
    Operation op() const { return op_; }

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

    Expression* transform(Transformer* tr) const override {
        return tr->transformBinaryOperation(this);
    }

private:
    Expression* left_;
    Operation op_;
    Expression* right_;
};

// Класс FunctionCall
struct FunctionCall : Expression {
    FunctionCall(const string& name, Expression* arg)
        : name_(name), arg_(arg) {}

    ~FunctionCall() override {
        delete arg_;
    }

    string name() const { return name_; }
    Expression* arg() const { return arg_; }

    double evaluate() const override {
        double arg_value = arg_->evaluate();
        if (name_ == "sqrt") return sqrt(arg_value);
        if (name_ == "abs") return fabs(arg_value);
        return 0.0;
    }

    Expression* transform(Transformer* tr) const override {
        return tr->transformFunctionCall(this);
    }

private:
    string name_;
    Expression* arg_;
};

// Класс Variable
struct Variable : Expression {
    Variable(const string& name, double value = 0.0) : name_(name), value_(value) {}

    string name() const { return name_; }
    double value() const { return value_; }
    void setValue(double value) { value_ = value; }

    double evaluate() const override { return value_; }

    Expression* transform(Transformer* tr) const override {
        return tr->transformVariable(this);
    }

private:
    string name_;
    double value_;
};

// Реализация CopySyntaxTree
struct CopySyntaxTree : Transformer {
    Expression* transformNumber(Number const* number) override {
        return new Number(number->value());
    }

    Expression* transformBinaryOperation(BinaryOperation const* binop) override {
        Expression* left = binop->left()->transform(this);
        Expression* right = binop->right()->transform(this);
        return new BinaryOperation(left, binop->op(), right);
    }

    Expression* transformFunctionCall(FunctionCall const* fcall) override {
        Expression* arg = fcall->arg()->transform(this);
        return new FunctionCall(fcall->name(), arg);
    }

    Expression* transformVariable(Variable const* var) override {
        return new Variable(var->name(), var->value());
    }
};

// Реализация FoldConstants
struct FoldConstants : Transformer {
    Expression* transformNumber(Number const* number) override {
        return new Number(number->value());
    }

    Expression* transformBinaryOperation(BinaryOperation const* binop) override {
        Expression* left = binop->left()->transform(this);
        Expression* right = binop->right()->transform(this);

        if (auto* numLeft = dynamic_cast<Number*>(left)) {
            if (auto* numRight = dynamic_cast<Number*>(right)) {
                double result = 0;
                switch (binop->op()) {
                case BinaryOperation::PLUS:  result = numLeft->value() + numRight->value(); break;
                case BinaryOperation::MINUS: result = numLeft->value() - numRight->value(); break;
                case BinaryOperation::MUL:   result = numLeft->value() * numRight->value(); break;
                case BinaryOperation::DIV:   result = numLeft->value() / numRight->value(); break;
                }
                delete left;
                delete right;
                return new Number(result);
            }
        }
        return new BinaryOperation(left, binop->op(), right);
    }

    Expression* transformFunctionCall(FunctionCall const* fcall) override {
        Expression* arg = fcall->arg()->transform(this);
        if (auto* numArg = dynamic_cast<Number*>(arg)) {
            double val = numArg->value();
            delete arg;
            if (fcall->name() == "sqrt") return new Number(sqrt(val));
            if (fcall->name() == "abs") return new Number(fabs(val));
        }
        return new FunctionCall(fcall->name(), arg);
    }

    Expression* transformVariable(Variable const* var) override {
        return new Variable(var->name(), var->value());
    }
};

int main() {
    // Создание AST: abs(var * sqrt(32 - 16))
    Expression* expr = new FunctionCall("abs",
        new BinaryOperation(
            new Variable("var", 5.0), // Значение переменной var установлено в 5.0
            BinaryOperation::MUL,
            new FunctionCall("sqrt",
                new BinaryOperation(
                    new Number(32.0),
                    BinaryOperation::MINUS,
                    new Number(16.0)
                )
            )
        )
    );

    cout << "Original: " << expr->evaluate() << endl;

    // Копирование дерева
    CopySyntaxTree copier;
    Expression* copied = expr->transform(&copier);
    cout << "Copied: " << copied->evaluate() << endl;

    // Сворачивание констант
    FoldConstants folder;
    Expression* folded = expr->transform(&folder);
    cout << "Folded: " << folded->evaluate() << endl;


    delete expr;
    delete copied;
    delete folded;

    return 0;
}