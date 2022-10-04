#pragma once
class Point {
public:
    int x, y;
    Point() {};
    ~Point() {};
};

class Circle
{
public:
    int maxX, minX, maxY, minY, R;
    Point point1;
    Point point2;
    Circle() {};
    ~Circle() {};
};