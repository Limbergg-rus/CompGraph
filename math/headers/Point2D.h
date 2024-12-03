#ifndef POINT2D_H
#define POINT2D_H

#include <vector>
#include <string>
#include <QColor>
#include <iostream>
#include "Vector3D.h"
#include "Vector2D.h"

class Point2D
{
	struct Point2DHash {
		size_t operator()(const Point2D& point) const {
			return std::hash<int>()(point.x) ^ (std::hash<int>()(point.y) << 1);
		}
	};
public:
    Point2D();

    Point2D(const Vector2D &a);

    Point2D(float x, float y);

    [[nodiscard]] float getX() const;

    [[nodiscard]] float getY() const;


    void setX(float x);

    void setY(float y);

    void set(float x, float y);

    //static Point2D vertex_to_point(Vector2D vertex, int width, int height);

    friend bool operator==(const Point2D &lhs, const Point2D &rhs)
    {
        return lhs.x == rhs.x
               && lhs.y == rhs.y;
    }

    friend bool operator!=(const Point2D &lhs, const Point2D &rhs)
    {
        return !(lhs == rhs);
    }
    bool operator<(const Point2D& other) const
    {
        return (x < other.x) || (x == other.x && y < other.y);
    }
	int generate_key();

private:
    float x;
    float y;

};
class MyHashFunction {
public:
	//TODOЕсли какие-то баги в первуб очередь что-то сделать с этим хешем

	size_t operator()(const Point2D& p) const
	{
		return (std::hash<std::string>()(std::to_string(p.getX()))) ^
			(std::hash<std::string>()(std::to_string(p.getY())));
	}

	size_t operator()(const QColor& color) const {
		auto a = std::hash<int>()(color.red()) ^
			(std::hash<int>()(color.green()) << 8) ^
			(std::hash<int>()(color.blue()) << 16) ^
			(std::hash<int>()(color.alpha()) << 24);
//		std::cout << color.red() << " " << color.green() << " " << color.blue() << " " << color.alpha() << " " << a<< std::endl;

		return a;
	}
};
#endif
