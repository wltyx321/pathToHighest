#include<stdlib.h>
#include<stdio.h>
#include<Windows.h>
#include <sstream>
#include<math.h>
#include<iostream>
#include<vector>
#include<queue>
#include<stack>
#include <string>
#include<algorithm>
#include<cstring>
const int defaultSize = 0;
using namespace std;
const int optrLen = 10;
const char OPTR[optrLen] = { '{','}', '[',']', '(',')',
'*','/', '+','-' };//0->n运算符优先级降低  //0->5是括号
const string testStr[][2] = 
{ 
	{ "(2+3" ,"" },
	{ "2+3)","" },
	{ "2+3)+" ,"" },
	{ "1.2.2+3" ,"" },
	{"(1+2)*((8-2)/(7-4))",""} ,
	{ "(1.1+2.2)*((8-2)/(7-4))",""},
	{ "(#1+2)*(1+2)*((8-2)/(7-4))","" } ,
	{ "[(8-2)/(7-4)]*(1+2)","" } ,
	{ "[(8.6-2)/(7-4)]*(1.1+2)","" },
};
/*
	允许包含的字符为OPTR中的 和 数字 和 '#' 和 '.'
*/
bool isOPTR(char charNow,int *whichOptr) {
	bool isOptr = false;
	int i = 0;
	for (i = 0; i < optrLen; i++) {
		if (charNow == OPTR[i]) {
			isOptr = true;
			break;
		}
	}
	*whichOptr = i;
	return isOptr;
}

string inputExpression() {
	bool expressRight = false;
	string expression;
	while (!expressRight) {
		cout << "Input your expression here: ";
		cin >> expression;
		for (string::iterator iter = expression.begin(); iter != expression.end(); iter++) {
			char curr = *iter;
			if ((curr - '0' >= 0) && ('9' - curr >= 0)) {
				continue;
			}
			int i = 0;
			bool isOptr = isOPTR(curr,&i);
			if ((!isOptr) && ('#' != curr) && ('.'!=curr)) {
				expressRight = false;
				break;
			}
			expressRight = true;
		}
	}
	return expression.c_str();
}

bool numRight(string numStr) {
	int point = 0;
	for (string::iterator iter = numStr.begin(); iter != numStr.end(); iter++) {
		if('.'==(*iter)) {
			point++;
		}
		if (1<point) {
			cout << "error: more than one point in one num!" << endl;
			return false;//多于一个小数点
		}
	}
	return true;
}

string postExpression(string expression,bool *flag) {
	expression = "(1+2)*(1+2)*((8-2)/(7-4))";
	typedef struct { char optr; int place; }OPTRANDPLACE;
	stack<OPTRANDPLACE> optr;//记录操作符
	string back;
	for (string::iterator iter = expression.begin(); iter != expression.end(); iter++) {
		int i = 0;
		bool isOptr = isOPTR(*iter, &i);
		if (!isOptr) //操作数
		{
			//back += *iter;
			string numToBack;
			while ( !isOPTR(*iter,&i) ) {
				numToBack += *iter;
				if (expression.end() == (iter + 1)) {
					break;
				}
				iter++;
			}
			iter--;
			bool isRight=numRight(numToBack);
			if (!isRight) {
				*flag = false;
				return back;
			}
			back += " ";
			back += numToBack;
		}
		if (isOptr) //操作符
		{
			if (optr.empty()) {//栈空
				OPTRANDPLACE temp;
				temp.optr = *iter;
				temp.place = i;
				optr.push(temp);
				continue;
			}
			if ((i < 6) && (0 == (i + 1) % 2))//运算符是右括号
			{
				bool getLeftBrack = false;
				while (!optr.empty() && (!getLeftBrack)) {
					OPTRANDPLACE topOptr = optr.top();
					if (topOptr.place < 6 && (0 == topOptr.place % 2))//遇到左括号
					{
						getLeftBrack = true;
						optr.pop();
						break;
					}
					back += topOptr.optr;
					back += " ";
					optr.pop();
				}
				continue;
			}
			OPTRANDPLACE optrTop = optr.top();
			//不是括号 栈顶运算符优先级比当前运算符优先级低
			//是左括号 进栈 
			if ((optrTop.place<6) || (optrTop.place >= i) || ((i < 6) && (0 != (i + 1) % 2)))
			{
				OPTRANDPLACE temp;
				temp.optr = *iter;
				temp.place = i;
				optr.push(temp);
				continue;
			}
			//栈顶运算符优先级比当前运算符优先级高
			back += " ";
			back += optr.top().optr;
			optr.pop();
		}
	}
	//当所有括号弹出栈后 剩于运算符加入表达式(1+2)*((8-2)/(7-4))最后剩余*
	//再例如表达式"(1+2)*(1+2)*((8-2)/(7-4))"最后剩余**
	while (!optr.empty()) {
		back += " ";
		back += optr.top().optr;
		optr.pop();
	}
	return back;
}

double XRelationY(double x, double y, char relation) {
	switch (relation) {
	case '+':
		return x + y;
	case '-':
		return x - y;
	case '*':
		return x * y;
	case '/':
		return x / y;
	}
}

vector<string> splitStr(string expression) {
	vector<string> backSplit;
	string split = " ";
	char* temp = strtok((char *)(expression.c_str()), split.c_str());
	while (NULL!=temp) {
		backSplit.push_back(temp);
		temp = strtok(NULL, split.c_str());
	}
	return backSplit;
}

double getVal(string postExpress, bool *flag) {
	stack<double> expressNumber;
	vector<string> expressSplit = splitStr(postExpress);
	try {
		for (vector<string>::iterator iter = expressSplit.begin(); iter != expressSplit.end();iter++) {
			string temp = *iter;
			int i = 0;
			bool isOptr = isOPTR(temp[0], &i);
			if (!isOptr) {
				istringstream iss(temp);
				double num;
				iss >> num;
				expressNumber.push(num);
			}
			if(isOptr) {
				double x = expressNumber.top();
				expressNumber.pop();
				double y = expressNumber.top();
				expressNumber.pop();
				double xRy = XRelationY(y, x, OPTR[i]);
				expressNumber.push(xRy);
			}
		}
	}
	catch (std::exception) {
		printf("expression illegal\n");
		*flag = false;
		return 0;
	}
	return expressNumber.top();
}

void run() {
	bool isExit = false;
	do {
		bool expressTrue = true;
		string expression;
		//expression = inputExpression();
		expression=testStr[0][0];
		expression = postExpression(expression,&expressTrue);
		if (expressTrue) {
			cout << "Postfix expression: " << expression << endl;
			bool flag = true;
			double value = getVal(expression, &flag);
			if (flag) {
				cout << "Value of expression: " << value << endl;
			}
		}
		cout << "\n\ndo you want to continue?(Y/N): ";
		char isGo = 'Y';
		cin >> isGo;
		isExit = isGo == 'Y' ? 0 : 1;
	} while (!isExit);
}

int main() {
	run();
	return 0;
}
