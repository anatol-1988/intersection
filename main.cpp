#include <iostream>
#include <memory>
#include <tuple>
#include <variant>
#include <vector>

using namespace std;

using Coordinate = pair<int, int>;

auto operator<<(ostream &os, Coordinate const &coordinate) -> ostream &
{
    os << "Coordinate{x = " << coordinate.first << ", y = " << coordinate.second
       << "}";
    return os;
}

struct Circle final {
    int radius;
    Coordinate center;
};

auto operator<<(ostream &os, Circle const &circle) -> ostream &
{
    os << "Circle{radius = " << circle.radius << ", center = " << circle.center
       << "}";
    return os;
}

struct Rectangle final {
    Coordinate leftBottom;
    int width;
    int height;
};

auto operator<<(ostream &os, Rectangle const &rect) -> ostream &
{
    os << "Rectangle{leftBottom = " << rect.leftBottom
       << ", width = " << rect.width << ", height = " << rect.height << "}";
    return os;
}

struct Polygon final {
    vector<Coordinate> vertexes;
};

auto operator<<(ostream &os, vector<Coordinate> const &vertexes) -> ostream &
{
    os << "[";

    if (!vertexes.empty())
        cout << vertexes.front();

    for (auto it = cbegin(vertexes) + 1; it < cend(vertexes); ++it)
        cout << ", " << *it;

    cout << "]";
    return os;
}

auto operator<<(ostream &os, Polygon const &polygon) -> ostream &
{
    os << "Polygon{vertexes = " << polygon.vertexes << "}";
    return os;
}

using Shape = variant<Circle, Rectangle, Polygon>;

struct Intersection final {
    Intersection(Circle const &circle, Circle const &circle2)
    {
        cout << circle << " vs " << circle2;
    }
    Intersection(Circle const &circle, Rectangle const &rect)
    {
        cout << circle << " vs " << rect;
    }
    Intersection(Circle const &circle, Polygon const &polygon)
    {
        cout << circle << " vs " << polygon;
    }
    Intersection(Rectangle const &rect, Polygon const &polygon)
    {
        cout << rect << " vs " << polygon;
    }
    Intersection(Rectangle const &rect, Rectangle const &rect2)
    {
        cout << rect << " vs " << rect2;
    }
    Intersection(Polygon const &polygon, Polygon const &polygon2)
    {
        cout << polygon << " vs " << polygon2;
    }
};

struct Visitor {
    virtual auto operator()(const Circle &circle) const -> Intersection = 0;
    virtual auto operator()(const Rectangle &rect) const -> Intersection = 0;
    virtual auto operator()(const Polygon &polygon) const -> Intersection = 0;
};

struct CircleVisitor final : Visitor {
    CircleVisitor(Circle const &circle)
        : _circle{circle}
    {
    }
    Circle const &_circle;

    auto operator()(Circle const &circle) const -> Intersection override
    {
        return {_circle, circle};
    }

    auto operator()(Rectangle const &rect) const -> Intersection override
    {
        return {_circle, rect};
    }

    auto operator()(Polygon const &polygon) const -> Intersection override
    {
        return {_circle, polygon};
    }
};

struct RectangleVisitor final : Visitor {
    RectangleVisitor(Rectangle const &rect)
        : _rect{rect}
    {
    }
    Rectangle const &_rect;

    auto operator()(Circle const &circle) const -> Intersection override
    {
        return {circle, _rect};
    }

    auto operator()(Rectangle const &rect) const -> Intersection override
    {
        return {_rect, rect};
    }

    auto operator()(Polygon const &polygon) const -> Intersection override
    {
        return {_rect, polygon};
    }
};

struct PolygonVisitor final : Visitor {
    PolygonVisitor(Polygon const &polygon)
        : _polygon{polygon}
    {
    }
    Polygon const &_polygon;

    auto operator()(Circle const &circle) const -> Intersection override
    {
        return {circle, _polygon};
    }

    auto operator()(Rectangle const &rect) const -> Intersection override
    {
        return {rect, _polygon};
    }

    auto operator()(Polygon const &polygon) const -> Intersection override
    {
        return {_polygon, polygon};
    }
};

struct GetVisitor final {
    auto operator()(Circle const &circle) const -> shared_ptr<Visitor>
    {
        return make_shared<CircleVisitor>(circle);
    }

    auto operator()(Rectangle const &rect) const -> shared_ptr<Visitor>
    {
        return make_shared<RectangleVisitor>(rect);
    }

    auto operator()(Polygon const &polygon) const -> shared_ptr<Visitor>
    {
        return make_shared<PolygonVisitor>(polygon);
    }

    auto from(Shape const &shape) const -> shared_ptr<Visitor>
    {
        return visit(*this, shape);
    }
};

auto getIntersection(Shape const &s1, Shape const &s2) -> Intersection
{
    auto const visitor = GetVisitor{}.from(s1);
    return visit(*visitor, s2);
}

auto main() -> int
{
    getIntersection(Circle{0, {10, 10}}, Polygon{{{10, 10}, {20, 20}}});
    cout << endl;
    return 0;
}
