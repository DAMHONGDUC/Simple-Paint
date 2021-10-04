#include "framework.h"
#include "Shape.h"
#include "Tokenizer.h"


shared_ptr<IShape> LLine::parse(string s1, string s2, int penStyle, COLORREF color, int penSize)
{
	vector<string> tokens1 = Tokenizer::split(s1, ",");
	vector<string> tokens2 = Tokenizer::split(s2, ",");

	return make_shared<LLine>(PPoint(stoi(tokens1[0]), stoi(tokens1[1])), PPoint(stoi(tokens2[0]), stoi(tokens2[1])), color, penStyle, penSize);
}

shared_ptr<IShape> LLine::Create(int currShapeMode, PPoint p1, PPoint p2, int color, int penStyle, int penSize)
{
	return make_shared<LLine>(p1, p2, color, penStyle, penSize);
}

LLine::LLine(PPoint A, PPoint B, COLORREF color, int penStyle, int penSize)
{
	this->A = A;
	this->B = B;
	this->type = LINE;
	this->color = color;
	this->penStyle = penStyle;
	this->penSize = penSize;
}

void LLine::Draw(HDC hdc)
{
	MoveToEx(hdc, A.get_x(), A.get_y(), NULL);
	LineTo(hdc, B.get_x(), B.get_y());
}

string LLine::toString()
{
	stringstream writer;

	writer << A.toString() << " " << B.toString();
	string result = writer.str();

	return result;
}


