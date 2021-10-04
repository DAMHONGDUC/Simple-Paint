#include "framework.h"
#include "Shape.h"
#include "Tokenizer.h"

bool IShape::Is_in(PPoint C)
{
	int max_x = max(A.get_x(), B.get_x());
	int min_x = min(A.get_x(), B.get_x());

	int max_y = max(A.get_y(), B.get_y());
	int min_y = min(A.get_y(), B.get_y());

	if (((A.get_x() < B.get_x()) && (A.get_y() < B.get_y())) || ((A.get_x() > B.get_x()) && (A.get_y() > B.get_y())))
	{
		if (min_x <= C.get_x() && C.get_x() <= max_x)
			if (min_y <= C.get_y() && C.get_y() <= max_y) return true;
		return false;
	}
	if (((A.get_x() > B.get_x()) && (A.get_y() < B.get_y())) || ((A.get_x() < B.get_x()) && (A.get_y() > B.get_y())))
	{
		if (min_x <= C.get_x() && C.get_x() <= max_x)
			if (min_y <= C.get_y() && C.get_y() <= max_y) return true;
		return false;
	}
	return false;
}

void IShape::setType(ShapeMode type)
{
	this->type = type;
}

void IShape::setColor(COLORREF color)
{
	this->color = color;
}

void IShape::setPenStyle(int penStyle2)
{
	this->penStyle = penStyle2;
}

void IShape::setPenSize(int penSize2)
{
	this->penSize = penSize2;
}

PPoint IShape::get_A()
{
	return A;
}

PPoint IShape::get_B()
{
	return B;
}

ShapeMode IShape::getType()
{
	return this->type;
}

int IShape::getPenSize()
{
	return this->penSize;
}

int IShape::getPenStyle()
{
	return this->penStyle;
}

COLORREF IShape::getColor()
{
	return this->color;
}
