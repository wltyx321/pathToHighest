#include<string>
#include<vector>
#include<stack>
using namespace std;

class NODE {
public:
	int index;
	int level;
	string name;
	string id;
	string content;

	int parent;
	vector<int> child;
	NODE():level(0),name(""),id(""),content(""),parent(0){}
	bool hasChild(const vector<NODE>* content,string tag,int* place) {
		if (tag[0] == '#') {
			for (int i = 0; i < child.size();i++) {
				if (0==strcmp((*content)[child[i]].id.c_str(),tag.substr(1).c_str())) {
					*place = child[i];
					return true;
				}
			}
			return false;
		}
		for (int i = 0; i < child.size(); i++) {
			if (0 == strcmp((*content)[child[i]].name.c_str(), tag.c_str())) {
				*place = child[i];
				return true;
			}
		}
		return false;
	}
};

class HTML {
private:
	vector<NODE> content;

	void setTree(vector<NODE>::iterator iter) {
		for (vector<NODE>::iterator curr = content.begin();
			curr != content.end(); curr++) {
			vector<NODE>::iterator after = curr + 1;
			for (;
				after != content.end(); after++){
				if ((*after).level-(*curr).level== 0) {
					break;
				}
				if ((*after).level-(*curr).level == 1) {
					(*curr).child.push_back((*after).index);
					(*after).parent = (*curr).index;
				}
			}
		}
	}
	void A_to_a(string* str) {
		for (int i = 0; i < str->length();i++) {
			if ((*str)[i]>='A'&& (*str)[i]<='Z') {
				(*str)[i] = (*str)[i] - 'A' + 'a';
			}
		}
	}
	
public:
	void clear() {
		content.clear();
	}
	void getContent(int lines) {
		string tempStr = "";
		vector<string> tempContent;
		stack<int[2]> levelStack;//place+level
		getchar();
		while (lines--) {
			tempStr = "";
			getline(std::cin,tempStr);
			tempContent.push_back(tempStr);
		}
		int dotCount = 0;
		while (!tempContent.empty()) {
			NODE temp;
			dotCount = 0;
			int i = 0;
			for (; tempContent[0][i] == '.';i++, dotCount++) {}
			temp.level = dotCount / 2;
			for (; i<tempContent[0].length()&&tempContent[0][i] != ' ';i++) {
				temp.name += tempContent[0][i];
			}
			if (i+1 < tempContent[0].length()&&
				tempContent[0][i+1]=='#') {
				i++;
				for (i++; i < tempContent[0].length(); i++) {
					temp.id += tempContent[0][i];
					A_to_a(&temp.id);
				}
			}
			temp.index = content.size();
			content.push_back(temp);
			tempContent.erase(tempContent.begin());
		}
		setTree(content.begin());
	}
	vector<int> selectTag(string tag) {
		vector<int> back;
		for (vector<NODE>::iterator iter = content.begin();
			iter != content.end(); iter++) {
			if (0==strcmp((*iter).name.c_str(),tag.c_str())) {
				back.push_back((*iter).index);
			}
		}
		return back;
	}
	vector<int> selectId(string id) {
		vector<int> back;
		if (id[0] == '#') {
			id = id.substr(1);
		}
		A_to_a(&id);
		for (vector<NODE>::iterator iter = content.begin();
			iter != content.end(); iter++) {
			if (0 == strcmp((*iter).id.c_str(), id.c_str())) {
				back.push_back((*iter).index);
			}
		}
		return back;
	}
	vector<int> selectChild(vector<string> childFlag) {
		vector<int> back;
		for (vector<NODE>::iterator iter = content.begin();
			iter != content.end(); iter++) {
			if (childFlag[0][0]=='#'&&
				0 == strcmp((*iter).id.c_str(), childFlag[0].substr(1).c_str())) {
				back.push_back((*iter).index);
			}
			else if(0 == strcmp((*iter).name.c_str(), childFlag[0].c_str())) {
				back.push_back((*iter).index);
			}
		}
		for (int i = 1; i < childFlag.size();i++) {
			for (int j = 0; j < back.size();j++) {
				int tempPlace = -1;
				if (content[ back[j] ].hasChild(&content,childFlag[i],&tempPlace)) {
					back[j] = tempPlace;
				}
				else {
					back.erase(back.begin()+j);
					j--;
				}
			}
			if (back.empty()) {
				return back;
			}
		}
		return back;
	}
};
vector<string> split(string target) {
	vector<string> back;
	vector<int> findP;
	
	std::size_t found = target.find_first_of(" ");
	while (found != std::string::npos)
	{
		findP.push_back(found);
		target[found] = '*';
		found = target.find_first_of(" ", found + 1);
	}
	if (findP.empty()) {
		return back;
	}
	else if (findP.size()==1) {
		back.push_back(target.substr(0, findP[0]));
		back.push_back(target.substr(findP[findP.size() - 1] + 1));
		return back;
	}
	back.push_back(target.substr(0,findP[0]));
	for (int i = 1; i < findP.size();i++) {
		back.push_back(target.substr(findP[i-1]+1, findP[i] - findP[i - 1] - 1));
	}
	back.push_back(target.substr(findP[findP.size()-1]+1));
	return back;
}

void run() {
	HTML test;
	int lines = 0; int selectCount = 0;
	cin >> lines; cin >> selectCount;
	test.getContent(lines);
	vector<string> select;
	while (selectCount--) {
		string temp = "";
		getline(std::cin,temp);
		select.push_back(temp);
	}
	for (int i = 0; i < select.size();i++) {
		vector<string> temp = split(select[i]);
		vector<int> place;
		if (temp.size()>1) {
			place=test.selectChild(temp);
		}
		else if (temp.size() == 0&&
			select[i][0]=='#') {
			place=test.selectId(select[i]);
		}
		else{
			place=test.selectTag(select[i]);
		}
		cout << place.size() << " ";
		for (int j = 0; j < place.size();j++) {
			cout << place[j]+1 << " ";
		}
		cout << endl;
	}
}
/*
	输入文本
11 5
html
..head
....title
..body
....h1
....p #subtitle
....div #main
......h2
......p #one
......div
........p #two
p
#subtitle
h3
div p
div div p
*/
/*
	输出文本
	3 6 9 11
	1 6
	0
	2 9 11
	1 11
*/
int main(){
	run();
	return 0;
}