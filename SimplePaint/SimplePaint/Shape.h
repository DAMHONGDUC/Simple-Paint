#pragma once
#include "Windows.h"
#include <string>
#include <vector>
#include <sstream>
#include <memory>
using namespace std;

enum ShapeMode
{
	LINE,
	RECTANGLE,
	ELLIPSE
};

//												class PPoint
class PPoint
{
private:
	int x;
	int y;
public:
	PPoint();

	PPoint(int x, int y);

	void copy(PPoint A)
	{
		x = A.get_x();
		y = A.get_y();
	}

	int get_x();
	
	int get_y();
	
	void set_x(int x);

	void set_y(int y);

	string toString();
};

//												class IShape
class IShape
{
protected:
	PPoint A;
	PPoint B;
	COLORREF color;
	ShapeMode type;
	int penStyle;
	int penSize;
public:
	
	virtual shared_ptr<IShape> parse(string s1, string s2, int penStyle, COLORREF color, int penSize) = 0;

	virtual shared_ptr<IShape> Create(int currShapeMode, PPoint p1, PPoint p2, int color, int penStyle, int penSize) = 0;

	bool Is_in(PPoint C);

	PPoint get_A();

	PPoint get_B();

	ShapeMode getType() ;

	int getPenStyle() ;

	int getPenSize();

	COLORREF getColor() ;

	void setType(ShapeMode type);

	void setPenStyle(int penStyle2);

	void setColor(COLORREF color);

	void setPenSize(int PenSize2);

	virtual void Draw(HDC hdc) = 0;

	virtual string toString() = 0;
};

//											class LLine
class LLine : public IShape {
public:

	LLine() {}

	shared_ptr<IShape> parse(string s1, string s2, int penStyle, COLORREF color, int penSize);

	shared_ptr<IShape> Create(int currShapeMode, PPoint p1, PPoint p2, int color, int penStyle, int penSize);

	LLine(PPoint A, PPoint B, COLORREF color, int penStyle, int penSize);

	void Draw(HDC hdc);

	string toString();
};

//											class EEllipse
class EEllipse : public IShape {
private:
public:
	EEllipse() {}

	shared_ptr<IShape> parse(string s1, string s2, int penStyle, COLORREF color, int penSize);

	shared_ptr<IShape> Create(int currShapeMode, PPoint p1, PPoint p2, int color, int penStyle, int penSize);

	EEllipse(PPoint A, PPoint B, COLORREF color, int penStyle, int penSize);
	
	void Draw(HDC hdc);

	string toString();
};

//											class RRectangle
class RRectangle : public IShape {
private:
public:

	RRectangle() {}

	shared_ptr<IShape> parse(string s1, string s2, int penStyle, COLORREF color, int penSize);

	shared_ptr<IShape> Create(int currShapeMode, PPoint p1, PPoint p2, int color, int penStyle, int penSize);

	RRectangle(PPoint A, PPoint B, COLORREF color, int penStyle, int penSize);

	void Draw(HDC hdc);

	string toString();
};
