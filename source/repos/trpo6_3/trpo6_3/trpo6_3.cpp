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

