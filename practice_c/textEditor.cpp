#include<stdio.h>
#include<windows.h>
#include <functional>   // std::greater
#include<iostream>
#include<string>
#include <fstream>
#include <sstream>
#include<map>
#include<vector>
#include<algorithm>
using namespace std;

//截至目前三个问题
//所有1和2代表的意义用宏重新书写
//search返回值更改为vector
//更改x，y计算方式为0起始
#define DEL_MODEL_FIRST 1  //删除第一个
#define DEL_MODEL_ALL 2  //删除所有
#define SEARCH_MODEL_USER 0 //用户调用
#define SEARCH_MODEL_FUNCTION 1 //程序调用

typedef struct XOYS{
	int line;
	int y;
	XOYS(int xPos, int yPos) {
		line = xPos;
		y = yPos;
	}
}XOY;
typedef struct posStruct {
	int paragrahPos;
	vector<XOY> matchArray;
	posStruct(int paragrahPlace, vector<XOY> matchList) {
		paragrahPos = paragrahPlace;
		matchArray = matchList;
	}
}POSITION;

class Paragrah {
private:
	string ParagrahContent;
	int maxLineChar;//此应用默认是80

	void getPos(const int found,int* x,int* y) //根据段落的行x列y求出string的相应位置
	{
		*x = found / maxLineChar;
		*y = found % maxLineChar;
	}
public:
	Paragrah() :maxLineChar(80),  ParagrahContent(""){}
	void clear() {
		ParagrahContent = "";
	}
	void insert(string strIn,int x,int y) {
		ParagrahContent = ParagrahContent.insert((x-1)*maxLineChar+y-1,strIn);
	}
	void append(string strIn) {
		ParagrahContent += strIn;
	}
	vector<XOY> search(string* strToFind) {
		vector<XOY> posArray;
		string* temp = &ParagrahContent;
		std::size_t found= temp->find(*strToFind);
		int x = 0; int y = 0;
		if (found != std::string::npos) {
			getPos(found,&x,&y);
			posArray.push_back(XOY(x,y));
		}
		else {
			return posArray;
		}
		for (found =found+1 ;
			found != std::string::npos;
			found++) {
			found = temp->find(strToFind->c_str(), found, strToFind->length());
			if (found == std::string::npos) {
				break;
			}
			getPos(found,&x, &y);
			posArray.push_back(XOY(x, y));
		}
		return posArray;
	}
	//这里妄图使得外部调用是正确的
	int deleteStr(string* strToOut,vector<XOY> posArray,int model= DEL_MODEL_FIRST) //model=1删除第一个model=2删除所有
	{
		int pos = 0;
		if (DEL_MODEL_FIRST == model) {
			pos = posArray[0].line*maxLineChar + posArray[0].y;
			ParagrahContent.replace(pos, strToOut->length(),"");
			return 1;
		}
		else {
			for (int i = 0; i < posArray.size();i++) {
				pos = posArray[i].line*maxLineChar + posArray[i].y;
				ParagrahContent.replace(pos-i* strToOut->length(), strToOut->length(), "");
			}
		}
		return 1;
	}
	int replaceStr() {

	}
	void printContent() //每行maxLineChar=80个字符；最后一行换行
	{
		int pre = 0;
		for (pre = 0; pre+ maxLineChar < ParagrahContent.length();
			pre += maxLineChar) {
			cout << ParagrahContent.substr(pre, maxLineChar)<<endl;
		}
		if (pre < ParagrahContent.length()) {
			cout << ParagrahContent.substr(pre) << endl;
		}
	}
	string content() {
		return ParagrahContent;
	}
	int getLineCount() //获取行数
	{
		int lineCount = ParagrahContent.length()/maxLineChar;
		if (ParagrahContent.length() % maxLineChar != 0) {
			lineCount++;
		}
		return lineCount;
	}
	bool isEmpty() {
		return ParagrahContent.length() == 0;
	}
	void setMaxLineChar(int maxChar) {
		maxLineChar = maxChar;
	}
};

class TextEditor {
public:
	int maxLineChar;
	vector<Paragrah> article;
	void clear() { article.clear(); }
	void printArticle();//打印每个段落
	void insert(); //计算出第x行是哪一个段落的第i行，段落中查找第i行和第y列
	vector<POSITION> search(string strToFind = "", int model = 0);//打印这个字符串在文章出现的位置
	int deleteStr();//输入一个需要删除字符串，查找出文章那些位置出现过这个字符串，然后提示是删除第一个还是删除全部找到的字符串
	
	int scanFile(); //每读一个段落就content插入一个段落//暂时不能处理中文
	int saveFile();
	void setMaxLineChar(int maxChar) {
		maxLineChar = maxChar;
		for (vector<Paragrah>::iterator iter = article.begin();
			iter != article.end(); iter++) {
			(*iter).setMaxLineChar(maxChar);
		}
	}
};

void TextEditor::printArticle()//第一行行标是1
{
	cout << "\n\n-------------Article-Print----------------" << endl;
	if (0 == article.size()) {
		cout << "Empty article!" << endl;
		return;
	}
	for (vector<Paragrah>::iterator iter = article.begin();
		iter != article.end();iter++) {
		//(*iter).setMaxLineChar(4);
		(*iter).printContent();
		cout << "" << endl;
	}
}

void TextEditor::insert()
{
	cout << "\n\n-------------Article-Insert----------------" << endl;
	if (0 == article.size()) {
		cout << "Empty article!" << endl;
		return;
	}
	string strIn; int x; int y;
	do {
		cout << "Input string to insert here:  "; cin >> strIn;
		cout << "Input x to insert:  "; cin >> x;
		cout << "Input y to insert:  "; cin >> y;
	} while (x<1||y<1);
	//x和y必须大于等于1
	//--
	
	int count =0;
	int paragrahX = 0;
	int paragrahIndex = 0;
	if (x <= article[0].getLineCount())
	{
		article[0].insert(strIn, x, y);
		return;
	}
	else
	{
		for (vector<Paragrah>::iterator iter = article.begin();
			iter != article.end(); iter++) {
			count += (*iter).getLineCount();
			if (count > x) {
				paragrahX = x - (count - (*iter).getLineCount());
				paragrahIndex = iter - article.begin();
				break;
			}
		}
	}
	if (count < x) {
		cout << "error:x is too large!" << endl;
		return;
	}
	article[paragrahIndex].insert(strIn,paragrahX,y);
}

vector<POSITION> TextEditor::search(string strToFind, int model)
{
	vector<POSITION> matchPos;

	if (SEARCH_MODEL_USER == model) {
		strToFind = "";
		cout << "\n\n-------------Article-Search----------------" << endl;
		if (0 == article.size()) {
			cout << "Empty article!" << endl;
			return matchPos;
		}
		cout << "Input string to search here:  ";cin >> strToFind;
		//strToFind = "-----546282902-----";
	}
	//--
	int findCount = 0;
	for (vector<Paragrah>::iterator iter = article.begin();
		iter != article.end(); iter++) {
		vector<XOY> temp = (*iter).search(&strToFind);
		if (!temp.empty()) {
			findCount += temp.size();
			matchPos.push_back(POSITION(iter - article.begin(),temp));
		}
	}
	if (findCount == 0) {
		cout << "Nothing match!" << endl;
		return matchPos;
	}
	cout << "Match Count:\t" << findCount << endl;
	return matchPos;
}

int TextEditor::deleteStr()
{
	string strToDel;
	cout << "\n\n-------------Article-Delete----------------" << endl;
	if (0 == article.size()) {
		cout << "Empty article!" << endl;
		return 0;
	}
	cout << "Input string to Delete here:  ";cin >> strToDel;
	//strToDel = "-----546282902-----";
	//--

	vector<POSITION> matchPos;
	matchPos = search(strToDel, SEARCH_MODEL_FUNCTION);
	cout << "Paragrah\t" << "xPos\t" << "yPos\t" << endl;
	for (int i = 0; i < matchPos.size(); i++) {
		for (int iter = 0; iter < matchPos[i].matchArray.size();
			iter++) {
			cout << matchPos[i].paragrahPos << "\t\t"
				<< matchPos[i].matchArray[iter].line + 1 << "\t"
				<< matchPos[i].matchArray[iter].y + 1 << "\t" << endl;
		}
	}
	if (matchPos.empty()) {
		cout << "match nothing,can not delete!" << endl;
		return 0;
	}
	int choice = 1;
	do {
		cout<< "1: delete the first match!\n"
			<< "2: delete all!\n" << endl;
		cout << "Choice to delete: "; cin >> choice;
	} while (choice != DEL_MODEL_FIRST && choice != DEL_MODEL_ALL);
	if (choice == DEL_MODEL_FIRST) {
		article[matchPos[0].paragrahPos].deleteStr(&strToDel,matchPos[0].matchArray);
	}
	else if (choice == DEL_MODEL_ALL) {
		for (int i = 0; i < matchPos.size(); i++) {
			article[matchPos[i].paragrahPos].deleteStr(&strToDel, matchPos[i].matchArray, DEL_MODEL_ALL);
		}
	}
	return 1;
}

int TextEditor::scanFile()//暂时不能处理中文
{
	string fileName = "";
	cout << "Input fileName to open here:  "; cin >> fileName;
	//fileName = "Editor.txt";

	ifstream file(fileName.c_str(),ios::in);
	if (!file.is_open()) {
		cout << "error: file open error!" << endl;
		return 0;
	}
	//按段落与段落之间有空行来读取段落
	Paragrah temp;
	while (!file.eof()) {
		string content;
		getline(file, content);
		if (content == "") //读到空行
		{
			article.push_back(temp);
			temp.clear();
		}
		else {
			temp.append(content);
		}
	}
	if (!temp.isEmpty()) {
		article.push_back(temp);
	}
	//--
	file.close();
	return 1;
}

int TextEditor::saveFile()
{
	string fileName = "";
	cout << "Input fileName to save here:  "; cin >> fileName;
	//fileName = "EditorSave.txt";

	ofstream file(fileName.c_str(), ios::out);
	if (!file.is_open()) {
		cout << "error: file open error!" << endl;
		return 0;
	}

	for (int i = 0; i < article.size();i++) {
		file << article[i].content().c_str() << "\n";
	}
	file.close();
	return 1;
}

void run() {
	TextEditor editor;
	editor.setMaxLineChar(80);
	while(true) {
		system("cls");
		cout<< "-----------------MENU---------------------\n"
			<< "|            1:Print Article;            |\n"
			<< "|            2:Insert String;            |\n"
			<< "|            3:Search String;            |\n"
			<< "|            4:Delete String;            |\n"
			<< "|            5:Scan  Article;            |\n"
			<< "|            6:Save  Article;            |\n"
			<< "|            7:Quit;                     |\n"
			<< "-----------------MENU---------------------\n" << endl;
		int choice = 0;
		cout << "Input choice: "; cin >> choice;
		if (choice < 1 || choice>7) {
			cout << "error:input wrong" << endl;
			cout << "wating for order......" << endl;
			getchar();
			continue;
		}
		vector<POSITION> temp;
		switch (choice) {
		case 1:
			editor.printArticle();
			break;
		case 2:
			editor.insert();
			break;
		case 3:
			temp.clear();temp= editor.search();
			if (temp.empty()) {
				break;
			}
			cout << "Paragrah\t" << "xPos\t" << "yPos\t" << endl;
			for (int i = 0; i < temp.size();i++) {
				for (int iter = 0; iter < temp[i].matchArray.size();
					iter++) {
					cout << temp[i].paragrahPos << "\t\t"
						<< temp[i].matchArray[iter].line+1 << "\t"
						<< temp[i].matchArray[iter].y+1 << "\t"<<endl;
				}
			}
			break;
		case 4:
			editor.deleteStr();
			break;
		case 5:
			editor.scanFile();
			cout << "scan over!" << endl;
			break;
		case 6:
			editor.saveFile();
			cout << "save over!" << endl;
			break;
		case 7:
			return;
		}
		cout << "wating for continue......" << endl;
		getchar(); getchar();
	}
	
}
int main() {
	run();
	return 0;
}


