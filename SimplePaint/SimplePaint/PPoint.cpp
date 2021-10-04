#include "framework.h"
#include "Shape.h"

PPoint::PPoint()
{
	x = 0;
	y = 0;
}

PPoint::PPoint(int x, int y)
{
	this->x = x;
	this->y = y;
}

int PPoint::get_x()
{
	return x;
}

int PPoint::get_y()
{
	return y;
}

void PPoint::set_x(int x)
{
	this->x = x;
}

void PPoint::set_y(int y)
{
	this->y = y;
}

string PPoint::toString()
{
	stringstream writer;

	writer << x << "," << y;
	string result = writer.str();

	return result;
}