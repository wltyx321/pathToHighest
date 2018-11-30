#include<iostream>
#include<fstream>
#include <sstream>
#include<string>
#include<vector>
#include<algorithm>
#include<map>

using namespace std;

class HuffNode {
public:
	unsigned char val;
	int weight;
	int lc;
	int rc;
	int parent;
	bool isLeaf;
	HuffNode() :val('\0'), weight(0), lc(0), rc(0), parent(0), isLeaf(true) {	}
	void clear() {
		val = '\0';
		weight = 0;
		lc = 0;
		rc = 0;
		parent = 0;
		isLeaf = true;
	}
};


class HuffZip {
private:
	vector<HuffNode> HuffTree;
	map< unsigned char, vector<int> > valCodeMap;

	void buildStrCode(int root, string codeNow);
	int bulidHuffTree();
	int buildHuffCode(int root);//生成对应的huffcode编码
	string scanFile();//统计字符和对应频率
	int HuffRoot() { return HuffTree.size() - 1; }
	char bitOperator(string code);
public:
	int zip();
	int unZip();
};

string HuffZip::scanFile() {
	string fileName = "";
	cout << "file's name to zip: ";
	cin >> fileName;
	//fileName = "test.txt";

	ifstream file(fileName.c_str(), ios::binary);
	if (!file.is_open()) {
		cout << "error:scanFile-file can not open!" << endl;
		return "NULL";
	}
	unsigned char temp;
	int valFrequency[256] = { 0 };
	file.seekg(0, ios::end);
	long long fLen = file.tellg();
	file.seekg(0, ios::beg);
	for (int i = 0; i < fLen; i++) {
		temp = file.get();
		valFrequency[temp]++;
	}
	file.close();
	HuffNode tempNode;
	for (int i = 0; i < 256; i++) {
		if (valFrequency[i]) {
			tempNode.val = i;
			tempNode.weight = valFrequency[i];
			tempNode.isLeaf = true;
			HuffTree.push_back(tempNode);
		}
	}
	return fileName;
}

bool HuffNodeGreater(HuffNode i, HuffNode j) { return (i.weight < j.weight); }
int HuffZip::bulidHuffTree() {
	sort(HuffTree.begin(), HuffTree.end(), HuffNodeGreater);
	HuffNode IntlNode;
	for (int iter = 0; iter < HuffTree.size() - 1; iter += 2) {
		IntlNode.lc = iter; IntlNode.rc = iter + 1; IntlNode.isLeaf = false;
		IntlNode.weight = HuffTree[iter].weight + HuffTree[iter + 1].weight;
		HuffTree.push_back(IntlNode);
		sort(HuffTree.begin() + iter + 2, HuffTree.end(), HuffNodeGreater);
	}
	return 1;
}

void HuffZip::buildStrCode(int root, string codeNow) {
	if (HuffTree[root].isLeaf) {
		valCodeMap[HuffTree[root].val].push_back(HuffTree[root].weight);
		for (int i = 0; i < codeNow.length(); i++) {
			valCodeMap[HuffTree[root].val].push_back(codeNow[i] - '0');
		}
		return;
	}
	int left = HuffTree[root].lc;
	buildStrCode(left, codeNow + '0');
	int right = HuffTree[root].rc;
	buildStrCode(right, codeNow + '1');
}
int HuffZip::buildHuffCode(int root) {
	buildStrCode(root, "");
	return 1;
}

char HuffZip::bitOperator(string code) {
	char mask[] = { 128,64,32,16,8,4,2,1 };
	char x = 1 << 8;
	if (code.length() == 8) {
		for (int i = 0; i < 8; i++) {
			if ('1' == code[i]) {
				x = x | mask[i];
			}
		}
	}
	else if (code.length() < 8) {
		for (int i = 0; i < code.length(); i++) {
			if ('1' == code[i]) {
				x = x | mask[8 - i];
			}
			x << 1;
		}
		x << 8 - code.length();
	}
	return x;
}
int HuffZip::zip() {
	string fileName = scanFile();
	bulidHuffTree();
	buildHuffCode(HuffRoot());
	ifstream sourceFile(fileName.c_str(), ios::binary);
	if (!sourceFile.is_open()) {
		cout << "error:sourceFile-file can not open!" << endl;
		return 0;
	}

	string codeFileName = "";
	cout << "Zip:input code file's name: " << endl;
	cin >> codeFileName;
	//codeFileName = "code.dat";
	ofstream codeFile(codeFileName.c_str(), ios::binary);
	if (!codeFile.is_open()) {
		cout << "error:codeFile-file can not open!" << endl;
		return 0;
	}
	cout << "waiting:zip processing......" << endl;
	string code = "";
	code = to_string(valCodeMap.size());
	codeFile << code + "\n";
	for (map< unsigned char, vector<int> >::iterator
		iter = valCodeMap.begin(); iter != valCodeMap.end(); iter++) {
		codeFile << iter->first;
		codeFile << to_string(valCodeMap[iter->first][0]);
		codeFile << "\n";
	}
	sourceFile.seekg(0, ios::end);
	long long fLen = sourceFile.tellg();
	sourceFile.seekg(0, ios::beg);
	unsigned char temp;
	unsigned char buffer = 1 << 8; 
	unsigned char mark=1;

	map<unsigned char, string> valMap;
	for (int i = 0; i < valCodeMap.size(); i++) {
		valMap[i] = "";
		for (int j = 0; j < valCodeMap[i].size(); j++) {
			if (1 == valCodeMap[i][j]) {
				valMap[i] = valMap[i] + '1';
			}
			else if (0 == valCodeMap[i][j]) {
				valMap[i] = valMap[i] + '0';
			}
		}
	}
	while(!sourceFile.eof()){
		sourceFile.read((char*)&temp, sizeof(unsigned char));
		if (sourceFile.eof())
			break;
		for (char curr: valMap[temp]) {
			buffer <<= 1;
			mark <<= 1;
			if ('1'==curr) {
				buffer = buffer | 1;
			}
			if (mark == 0) {
				codeFile.write((char *)&buffer, 1);
				buffer = 1 << 8;
				mark = 0x01;
			}
		}
	}
	sourceFile.close();
	codeFile.close();
	return 1;
}
int HuffZip::unZip() {
	string fileName = "";
	cout << "input code file name: ";
	cin >> fileName;
	//fileName = "code.dat";
	//fileName = "1jpg.code";
	ifstream codeFile(fileName.c_str(), ios::binary);
	if (!codeFile.is_open()) {
		cout << "error:codeFile can not open\n" << endl;
		return 0;
	}
	codeFile.seekg(0, ios::end);
	long long fLen = codeFile.tellg();
	codeFile.seekg(0, ios::beg);
	long long int codeFileIter = 0;

	cout << "input target file's name" << endl;
	cin >> fileName;
	//fileName = "targetTest.txt";
	//fileName = "1_2.jpeg";
	ofstream targetFile(fileName.c_str(), ios::binary);
	if (!targetFile.is_open()) {
		cout << "error:targetFile can not open\n" << endl;
		return 0;
	}
	int valFrequency[256] = { 0 };
	string code = "";
	unsigned char temp = codeFile.get(); codeFileIter++;
	while ('\n' != temp) {
		code = code + (char)temp;
		temp = codeFile.get(); codeFileIter++;
	}
	istringstream iss(code);
	int valKind;
	iss >> valKind;//读取字符种类
	for (int i = 0; i < valKind; i++) {
		code = "";
		unsigned char valKey = codeFile.get();
		temp = codeFile.get(); codeFileIter += 2;
		while ('\n' != temp) {
			code = code + (char)temp;
			temp = codeFile.get(); codeFileIter++;
		}
		istringstream issTemp(code);
		if (i == 146) {
			cout << "" << endl;
		}
		int valNum;
		issTemp >> valNum;//读取字符种类
		valFrequency[valKey] = valNum;
	}//构建字符-频率字典
	HuffTree.clear();
	HuffNode tempNode;
	for (int i = 0; i < 256; i++) {
		if (valFrequency[i]) {
			tempNode.val = i;
			tempNode.weight = valFrequency[i];
			tempNode.isLeaf = true;
			HuffTree.push_back(tempNode);
		}
	}
	bulidHuffTree();
	//建树
	tempNode.clear();
	tempNode.isLeaf = HuffTree[HuffRoot()].isLeaf;
	tempNode.lc = HuffTree[HuffRoot()].lc;
	tempNode.rc = HuffTree[HuffRoot()].rc;
	tempNode.val = HuffTree[HuffRoot()].val;
	code = "";
	cout << "waiting: unzip processing......" << endl;
	for (int i = codeFileIter; i < fLen; i++) {
		temp = codeFile.get();
		for (int j = 0; j < 8; j++) {
			int next = 0;
			if (temp & 128) {
				next = tempNode.rc;//最高位是1
			}
			else {
				next = tempNode.lc;
			}
			tempNode.clear();
			tempNode.isLeaf = HuffTree[next].isLeaf;
			tempNode.lc = HuffTree[next].lc;
			tempNode.rc = HuffTree[next].rc;
			tempNode.val = HuffTree[next].val;
			if (tempNode.isLeaf) {
				targetFile.put(tempNode.val);
				tempNode.clear();
				tempNode.isLeaf = HuffTree[HuffRoot()].isLeaf;
				tempNode.lc = HuffTree[HuffRoot()].lc;
				tempNode.rc = HuffTree[HuffRoot()].rc;
				tempNode.val = HuffTree[HuffRoot()].val;
			}
			temp <<= 1;
		}
	}
	codeFile.close();
	targetFile.close();
	return 1;
}

void run() {
	HuffZip test;
	test.zip();
	test.unZip();
}
int main() {
	run();
	return 0;
}