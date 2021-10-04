#include "framework.h"
#include "Shape.h"
#include "Tokenizer.h"


shared_ptr<IShape> EEllipse::parse(string s1, string s2, int penStyle, COLORREF color, int penSize)
{
	vector<string> tokens1 = Tokenizer::split(s1, ",");
	vector<string> tokens2 = Tokenizer::split(s2, ",");

	return make_shared<EEllipse>(PPoint(stoi(tokens1[0]), stoi(tokens1[1])), PPoint(stoi(tokens2[0]), stoi(tokens2[1])), color, penStyle, penSize);
}

shared_ptr<IShape> EEllipse::Create(int currShapeMode, PPoint p1, PPoint p2, int color, int penStyle, int penSize)
{
	return make_shared<EEllipse>(p1, p2, color, penStyle, penSize);
}

EEllipse::EEllipse(PPoint A, PPoint B, COLORREF color, int penStyle, int penSize)
{
	this->A = A;
	this->B = B;
	this->type = ELLIPSE;
	this->color = color;
	this->penStyle = penStyle;
	this->penSize = penSize;
}

void EEllipse::Draw(HDC hdc)
{
	int x1 = A.get_x();
	int x2 = B.get_x();
	int y1 = A.get_y();
	int y2 = B.get_y();

	if (x1 < x2)
	{
		int tmp = x1;
		x1 = x2;
		x2 = tmp;
	}
	if (y1 < y2)
	{
		int tmp = y1;
		y1 = y2;
		y2 = tmp;
	}

	Ellipse(hdc, x1, y1, x2, y2);
}

string EEllipse::toString()
{
	stringstream writer;

	writer << A.toString() << " " << B.toString();
	string result = writer.str();

	return result;
}


