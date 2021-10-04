#include "Shape.h"

// class ShapeFactory
class ShapeFactory
{
private:
	static ShapeFactory* _instance;

	vector<shared_ptr<IShape>> _prototypes;

	ShapeFactory();

public:
	static ShapeFactory* Instance();

	shared_ptr<IShape> Create(int currShapeMode, PPoint p1, PPoint p2, int color, int penStyle, int penSize);

	shared_ptr<IShape> getShape(int type, string s1, string s2, int color, int penStyle, int penSize);
};