#include <iostream>
#include <memory>
#include <map>
#include <string>

using namespace std;

class Shape {
public:
    virtual ~Shape() = default;
    virtual unique_ptr<Shape> clone() const = 0;
    virtual void draw() const = 0;
};

class Circle : public Shape {
private:
    int radius;
public:
    Circle(int r = 0) : radius(r) {}

    unique_ptr<Shape> clone() const override {
        return make_unique<Circle>(*this);
    }

    void draw() const override {
        cout << "Drawing a Circle with radius: " << radius << endl;
    }
};

class Rectangle : public Shape {
private:
    int width, height;
public:
    Rectangle(int w = 0, int h = 0) : width(w), height(h) {}

    unique_ptr<Shape> clone() const override {
        return make_unique<Rectangle>(*this);
    }

    void setSize(int _width, int _height) {
        width = _width;
        height = _height;
    }

    void draw() const override {
        cout << "Drawing a Rectangle with width: " << width << ", height: " << height << endl;
    }
};

class ShapeRegistry {
private:
    map<string, unique_ptr<Shape>> prototypes;

public:
    ShapeRegistry() {
        prototypes["Circle"] = make_unique<Circle>(5);
        prototypes["Rectangle"] = make_unique<Rectangle>(10, 20);
    }

    unique_ptr<Shape> createShape(const string& type) const {
        auto it = prototypes.find(type);
        if (it != prototypes.end()) {
            return it->second->clone();
        }
        cout << "Prototype not found: " << type << endl;
        return nullptr;
    }
};


int main() {
    ShapeRegistry registry;

    unique_ptr<Shape> shape1 = registry.createShape("Circle");
    if (shape1) {
        shape1->draw();
    }

    unique_ptr<Shape> shape2 = registry.createShape("Rectangle");
    if (shape2) {
        Rectangle* rect = dynamic_cast<Rectangle*>(shape2.get());

        if (rect) {
            rect->setSize(15, 25);
            rect->draw();
        }
    }

    unique_ptr<Shape> shape3 = registry.createShape("Circle");
    if (shape3) {
        shape3->draw();
    }

    return 0;
}
