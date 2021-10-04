#include "ShapeFactory.h"
#include "Tokenizer.h"
#include <string>
using namespace std;

//							class ShapeFactory
ShapeFactory* ShapeFactory::_instance = NULL;

ShapeFactory::ShapeFactory()
{
	_prototypes.push_back(make_shared<LLine>());
	_prototypes.push_back(make_shared<RRectangle>());
	_prototypes.push_back(make_shared<EEllipse>());
}

ShapeFactory* ShapeFactory::Instance()
{
	if (_instance == NULL)
	{
		_instance = new ShapeFactory();
	}
	return _instance;
}

shared_ptr<IShape> ShapeFactory::Create(int currShapeMode, PPoint p1, PPoint p2, int color, int penStyle, int penSize)
{
	return _prototypes[currShapeMode]->Create(currShapeMode, p1,  p2, color,  penStyle, penSize);
}

shared_ptr<IShape> ShapeFactory::getShape(int type, string s1, string s2, int color, int penStyle, int penSize)
{
	return _prototypes[type]->parse(s1, s2, penStyle, color, penSize);
}