#include "factory.hpp"
#include "../../../../cpp/src/shapes/shapes.cpp"


using namespace ilrd_rd141;

std::shared_ptr<Shape> CreateLine()
{
    return std::make_shared<Line>();
}

std::shared_ptr<Shape> CreateCircle()
{
    return std::make_shared<Circle>();
}

std::shared_ptr<Shape> CreateRectangle()
{
    return std::make_shared<Rectangle>();
}

std::shared_ptr<Shape> CreateSquare()
{
    return std::make_shared<Square>();
}

int main()
{
    Factory::Factory<Shape, std::string> &shapeFactory = Singleton<Factory::Factory<Shape, std::string>>::getInstance();

    shapeFactory.Add("Line", CreateLine);
    shapeFactory.Add("Circle", CreateCircle);
    shapeFactory.Add("Rectangle", CreateRectangle);
    shapeFactory.Add("Square", CreateSquare);

    std::shared_ptr<Shape> line = shapeFactory.Create("Line");
    std::shared_ptr<Shape> circle = shapeFactory.Create("Circle");
    std::shared_ptr<Shape> rectangle = shapeFactory.Create("Rectangle");
    std::shared_ptr<Shape> square = shapeFactory.Create("Square");

    line->Draw();
    circle->Draw();
    rectangle->Draw();
    square->Draw();

    return 0;
}
