#include "Tokenizer.h"

//								class Tokenizer
vector<string> Tokenizer::split(string haystack, string separator, bool removeEmptyEntries)
{
	vector<string> tokens;

	int startPos = 0;
	size_t foundPos;

	while (true)
	{
		foundPos = haystack.find(separator, startPos);
		if (foundPos != string::npos)
		{
			string token = haystack.substr(startPos, foundPos - startPos);
			tokens.push_back(token);

			startPos = foundPos + separator.length();
		}
		else
		{
			string token = haystack.substr(startPos, haystack.length() - startPos);
			tokens.push_back(token);
			break;
		}
	}
	return tokens;
}