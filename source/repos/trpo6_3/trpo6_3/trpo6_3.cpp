#include <iostream>
#include <vector>
#include <iomanip>
#include <memory>

using namespace std;

struct NutritionVisitor;

// Базовый интерфейс элемента рецепта
struct Ingredient {
    virtual ~Ingredient() = default;
    virtual void accept(NutritionVisitor& visitor) = 0;
    virtual double getWeight() const = 0;
};

// Интерфейс посетителя
struct NutritionVisitor {
    virtual void visit(class Mushrooms* item) = 0;
    virtual void visit(class Shrimp* item) = 0;
    virtual void visit(class SourCream* item) = 0;
    virtual void visit(class Cheese* item) = 0;
    virtual void visit(class HerbsAndSpices* item) = 0;

    struct NutritionData {
        double calories = 0.0;
        double proteins = 0.0;
        double fats = 0.0;
        double carbs = 0.0;
        double cost = 0.0;
    } data;
};

// Конкретные элементы рецепта
struct Mushrooms : Ingredient {
    Mushrooms(double g) : weight(g) {}

    void accept(NutritionVisitor& visitor) override {
        visitor.visit(this);
    }

    double getWeight() const override { return weight; }

private:
    double weight;
};

struct Shrimp : Ingredient {
    Shrimp(double g) : weight(g) {}

    void accept(NutritionVisitor& visitor) override {
        visitor.visit(this);
    }

    double getWeight() const override { return weight; }

private:
    double weight;
};

struct SourCream : Ingredient {
    SourCream(double g) : weight(g) {}

    void accept(NutritionVisitor& visitor) override {
        visitor.visit(this);
    }

    double getWeight() const override { return weight; }

private:
    double weight;
};

struct Cheese : Ingredient {
    Cheese(double g) : weight(g) {}

    void accept(NutritionVisitor& visitor) override {
        visitor.visit(this);
    }

    double getWeight() const override { return weight; }

private:
    double weight;
};

struct HerbsAndSpices : Ingredient {
    HerbsAndSpices(double g) : weight(g) {}

    void accept(NutritionVisitor& visitor) override {
        visitor.visit(this);
    }

    double getWeight() const override { return weight; }

private:
    double weight;
};

// Реализация посетителя
struct NutritionCalculator : NutritionVisitor {
    void visit(Mushrooms* item) override {
        const double ratio = item->getWeight() / 100.0;
        data.calories += 27.0 * ratio;
        data.proteins += 4.4 * ratio;
        data.fats += 1.0 * ratio;
        data.carbs += 0.2 * ratio;
        data.cost += 20.0 * ratio;
    }

    void visit(Shrimp* item) override {
        const double ratio = item->getWeight() / 100.0;
        data.calories += 83.0 * ratio;
        data.proteins += 18.0 * ratio;
        data.fats += 1.0 * ratio;
        data.cost += 50.0 * ratio;
    }

    void visit(SourCream* item) override {
        const double ratio = item->getWeight() / 100.0;
        data.calories += 120.0 * ratio;
        data.proteins += 3.3 * ratio;
        data.fats += 10.0 * ratio;
        data.carbs += 3.3 * ratio;
        data.cost += 14.4 * ratio;
    }

    void visit(Cheese* item) override {
        const double ratio = item->getWeight() / 100.0;
        data.calories += 345.0 * ratio;
        data.proteins += 25.0 * ratio;
        data.fats += 25.0 * ratio;
        data.cost += 70.0 * ratio;
    }

    void visit(HerbsAndSpices* item) override {
        const double ratio = item->getWeight() / 100.0;
        data.calories += 37.0 * ratio;
        data.proteins += 3.3 * ratio;
        data.carbs += 7.0 * ratio;
        data.cost += 20.0 * ratio;
    }
};

// Класс рецепта
struct Recipe {
    vector<unique_ptr<Ingredient>> items;

    void addItem(unique_ptr<Ingredient>&& ingredient) {
        items.push_back(move(ingredient));
    }

    NutritionVisitor::NutritionData calculate() {
        NutritionCalculator visitor;
        for (auto& item : items) {
            item->accept(visitor);
        }
        return visitor.data;
    }
};

int main() {
    setlocale(LC_ALL, "Russian");

    Recipe dish;
    dish.addItem(make_unique<Mushrooms>(500.0));
    dish.addItem(make_unique<Shrimp>(100.0));
    dish.addItem(make_unique<SourCream>(30.0));
    dish.addItem(make_unique<Cheese>(20.0));
    dish.addItem(make_unique<HerbsAndSpices>(30.0));

    auto results = dish.calculate();

    cout << "------------------------------\n";
    cout << "| Питательная ценность блюда |\n";
    cout << "------------------------------\n";
    cout << fixed << setprecision(2);
    cout << "| Калории:    " << setw(8) << results.calories << " ккал|\n";
    cout << "| Белки:      " << setw(8) << results.proteins << " г   |\n";
    cout << "| Жиры:       " << setw(8) << results.fats << " г   |\n";
    cout << "| Углеводы:   " << setw(8) << results.carbs << " г   |\n";
    cout << "| Стоимость:  " << setw(8) << results.cost << " руб |\n";
    cout << "------------------------------\n";

    return 0;
}