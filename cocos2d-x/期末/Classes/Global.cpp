#include "Global.h"

#include <regex>
using std::regex;
using std::match_results;
using std::regex_match;
using std::cmatch;

string Global::gameSessionId = "";
long Global::score = 0;
string winner = "Both";

Global::Global()
{
}


Global::~Global()
{
}

string Global::toString(vector<char> *buffer) {
	string rst;
	for (char ch : *buffer) {
		rst.push_back(ch);
	}
	return rst;
}

string Global::getSessionIdFromHeader(string head) {
	regex nlp("\\r\\n");
	string header = regex_replace(head, nlp, " ");
	regex pattern(".*GAMESESSIONID=(.*) Content-Type.*");
	//match_results<std::string::const_iterator> result;
	cmatch result;
	bool valid = regex_match(header.c_str(), result, pattern);

	if (!valid) {
		return "";
	}
	return result[1];
}

void Global::setWinner(int a) {
	if (a == 0) winner = "Player2";
	else if (a == 1) winner = "Both";
	else winner = "Player1";
}

string Global::getWinner() {
	return winner;
}