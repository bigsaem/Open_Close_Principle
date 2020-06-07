#include <iostream>
#include <vector>
using namespace std;
template <typename T> struct Specification;
template <typename T> struct AndSpecification;
//open and close rule
//you should be able to open to extend your system, but close to modification
enum class Color {red, green, blue};
enum class Size {small, medium, large};
struct Product{
    string name;
    Color color;
    Size size;
};
//this is bad because we always have to create a function for each attribute
struct ProductFilter
{
    vector<Product*> by_color(vector<Product*> items, Color color)
    {
        vector<Product*> result;
        for(auto& i : items)
            if(i->color == color)
                result.push_back(i);
        return result;
    }
};

template <typename T> struct Specification
{
    virtual bool is_satisfied(T* item) const = 0;
};
template <typename T> struct AndSpecification : Specification<T>
{
    const Specification<T>& first;
    const Specification<T>& second;

    AndSpecification(const Specification<T> &first, const Specification<T> &second) : first(first), second(second) {}

    bool is_satisfied(T *item) const override {
        return first.is_satisfied(item) && second.is_satisfied(item);
    }
};
template <typename T> AndSpecification<T> operator&&
        (Specification<T>& first, Specification<T>& second)
{
    return { first, second };
}

template <typename T> struct Filter
{
    virtual vector<T*> filter(vector<T*> items, Specification<T>& spec) = 0;
};

struct BetterFilter : Filter<Product>
{
    vector<Product *> filter(vector<Product *> items, Specification<Product> &spec) override
    {
        vector<Product*> result;
        for(auto& item :items){
            if(spec.is_satisfied(item))
                result.push_back(item);
        }
        return result;
    }
};

struct ColorSpecification : Specification<Product>
{
    Color color;
    ColorSpecification(Color color) : color(color){};

    bool is_satisfied(Product *item) const override {
        return item->color == color;
    }
};

struct SizeSpecification : Specification<Product>
{
    Size size;
    SizeSpecification(Size size) : size(size){};

    bool is_satisfied(Product *item) const override {
        return item->size == size;
    }
};


int main() {
    Product apple{"Apple", Color::green, Size::small};
    Product tree{"Tree", Color::green, Size::large};
    Product house{"House", Color::blue, Size::large};

    vector<Product*> items {&apple, &tree, &house};

    cout<<"-----Bad filter-----"<<endl;
    ProductFilter pf;
    auto green_things = pf.by_color(items, Color::green);
    for (auto& item : green_things)
        cout<<item->name<<" isgreen."<<endl;

    cout<<"-----Better filter-----"<<endl;
    BetterFilter bf;
    ColorSpecification green(Color::green);
    for (auto& item : bf.filter(items, green))
        cout<<item->name<<" is green."<<endl;

    SizeSpecification large(Size::large);
    AndSpecification<Product> green_and_large(green, large);

    for (auto& item : bf.filter(items, green_and_large)){
        cout<<item->name<<" is green and large"<<endl;
    }
    cout<<"-----Using operator overriding-----"<<endl;
    auto spec = green && large;
    for (auto& item : bf.filter(items, spec)){
        cout<<item->name<<" is green and large"<<endl;
    }
    return 0;
}
