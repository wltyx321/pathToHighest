#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<vector>
#include<map>
#include<graphics.h>
#include <iomanip>
#include<conio.h>
using namespace std;

//#define NOT_FIND_EDGE -1
#define NO_EDGE  0
#define CANNOT_REACH INT_MAX
#define UNVISITED  0
#define VISITED  1
#define XOY_START "XOY_START"
#define TEXT_DRAW_SIZE 50 //画文字的边长
typedef struct {
	string from;
	map<string, int> toArray;
}REACH_LIST;//一个顶点可以到达的路径
typedef struct {
	int pathLen;
	vector<int> path;//存储路径上依次经过的顶点
}MIN_PATH;
typedef struct {
	int x;
	int y;
}XOY;

class Graph {
private:
	int numPoint, numEdge;
	int** edgeMatrix;//边权重矩阵
	int* mark;//顶点标记数组
public:
	Graph() {}
	void init(int n) {
		numPoint = n;
		numEdge = 0;
		mark = (int*)malloc(sizeof(int)*numPoint);
		memset(mark,UNVISITED,numPoint);
		edgeMatrix = (int**)malloc(sizeof(int*)*numPoint);
		for (int i = 0; i < numPoint; i++) {
			edgeMatrix[i]= (int*)malloc(sizeof(int)*numPoint);
			for (int j = 0; j < numPoint; j++) {
				edgeMatrix[i][j] = NO_EDGE;
			}
		}
	}
	int n() { return numPoint; }
	int e() { return numEdge; }
	int first(int v) {
		for (int i = 0; i < numPoint;i++) {
			if (edgeMatrix[v][i] != NO_EDGE) {
				return i;
			}
		}
		return numPoint;
	}
	int next(int v,int w) {
		for (int i = w + 1; i < numPoint;i++) {
			if (edgeMatrix[v][i] != NO_EDGE) {
				return i;
			}
		}
		return numPoint;
	}
	void setEdge(int v1,int v2,int weight) //边是有向的
	{
		if (weight<0) {
			cout << "weight is illegal" << endl;
			return;
		}
		if (edgeMatrix[v1][v2]==NO_EDGE) {
			numEdge++;
		}
		edgeMatrix[v1][v2] = weight;
	}
	void delEdge(int v1, int v2) {
		if (edgeMatrix[v1][v2] != NO_EDGE) {
			numEdge--;
		}
		edgeMatrix[v1][v2] = NO_EDGE;
	}
	bool hasEdge(int v1, int v2) {
		return edgeMatrix[v1][v2] != NO_EDGE;
	}
	int weight(int v1, int v2) {
		return edgeMatrix[v1][v2];
	}
	int getMark(int v) {
		return mark[v];
	}
	void setMark(int v,int val) {
		mark[v] = val;
	}

};

class placeMap {
private:
	string defaultFileName = "place_file.dat";
	map<string, int> place;//地点名称 和 对应顶点下标
	map<int, string> pointPlace;//顶点下标 和对应的地点名称
	Graph placeGraph;
	map<string, XOY> position;

	int minDisHelp(int* D);//D:顶点标记数组
	void drawLoad(RECT r, XOY to) {
		int toX = to.x;
		int toY = to.y;
		if (toY < r.top) {
			if (toX < r.left) {
				line(r.left, r.top, toX + TEXT_DRAW_SIZE, toY + TEXT_DRAW_SIZE);
			}
			else if (toX > r.right) {
				line(r.right, r.top, toX, toY + TEXT_DRAW_SIZE);
			}
			else {
				line((r.left + r.right) / 2, r.top, toX + TEXT_DRAW_SIZE / 2, toY + TEXT_DRAW_SIZE);
			}
		}
		else if (toY > r.bottom) {
			if (toX < r.left) {
				line(r.left, r.bottom, toX + TEXT_DRAW_SIZE, toY);
			}
			else if (toX > r.right) {
				line(r.right, r.bottom, toX, toY);
			}
			else {
				line((r.left + r.right) / 2, r.bottom, toX + TEXT_DRAW_SIZE / 2, toY);
			}
		}
		else {
			if (toX < r.left) {
				line(r.left, (r.top + r.bottom) / 2, toX + TEXT_DRAW_SIZE, toY + TEXT_DRAW_SIZE / 2);
			}
			else {
				line(r.right, (r.top + r.bottom) / 2, toX, toY + TEXT_DRAW_SIZE / 2);
			}
		}
	}
public:
	bool readPlaceFile();//读取一个存储图信息的文件
	bool writePlaceFile();
	MIN_PATH minDistance(string from, string to);//获取两点最短距离和路径
	void fileName(string name) {
		defaultFileName = name;
	}
	string placeName(int point) {
		return pointPlace[point];
	}

	void printPlace() {
		cout << std::right << "place" << endl;
		for (map<string, int>::iterator iter = place.begin();
			iter != place.end();iter++) {
			cout << iter->first << endl;
		}
	}
	void drawMap() {
		RECT r;//left;top;right;bottom;
		XOY temp;
		//draw place
		for (map<string, int>::iterator iter = place.begin();
			iter != place.end(); iter++) {
			string placeName = iter->first;
			r.left = position[placeName].x;
			r.top = position[placeName].y;
			r.right = r.left + TEXT_DRAW_SIZE;
			r.bottom = r.top + TEXT_DRAW_SIZE;
			line(r.left, r.top, r.left, r.bottom);//left
			line(r.left, r.top, r.right, r.top);//top
			line(r.right, r.top, r.right, r.bottom);//right
			line(r.left, r.bottom, r.right, r.bottom);//bottom
			drawtext(_T(placeName.c_str()), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_PATH_ELLIPSIS);
			int pointInt = iter->second;
			for (int w = placeGraph.first(pointInt); 
				w < placeGraph.n(); w = placeGraph.next(pointInt, w)) {
				temp.x= position[pointPlace[w]].x;
				temp.y= position[pointPlace[w]].y;
				drawLoad(r, temp);
			}
		}

	}
	void drawPath(vector<int> path) {
		RECT r;//left;top;right;bottom;
		XOY temp;
		setlinecolor(RED);
		setlinestyle(PS_SOLID,5);
		for (vector<int>::iterator iter = path.begin(); 
			iter != path.end()-1;iter++) {
			string fromName = pointPlace[*iter];
			r.left = position[fromName].x;
			r.top = position[fromName].y;
			r.right = r.left + TEXT_DRAW_SIZE;
			r.bottom = r.top + TEXT_DRAW_SIZE;
			string toName= pointPlace[*(iter+1)];
			temp.x = position[toName].x;
			temp.y = position[toName].y;
			Sleep(1000);
			drawLoad(r,temp);
		}
		setlinecolor(WHITE);
		setlinestyle(PS_DASH,1);
	}
};

bool placeMap::readPlaceFile() {
	//place file 格式:place name:{place can reach name:...;...place can reach name:...;...}是一行
	ifstream file(defaultFileName, ios::in|ios::binary);
	if (!file.is_open()) {
		cout << "file:" << defaultFileName << " can not open!" << endl;
		return false;
	}
	//init load list
	vector<REACH_LIST> dict;
	while (true) {
		REACH_LIST currStart;
		string currReachList = "";
		file >> currReachList;
		if (0==strcmp(currReachList.c_str(), XOY_START)) {
			break;
		}
		currStart.from = currReachList.substr(0, currReachList.find_first_of(":"));
		int found = currReachList.find_first_of("{");
		int end = currReachList.find_last_of("}");
		if (found+1==end) {
			dict.push_back(currStart);
			continue;
		}
		currReachList = currReachList.substr(found+1, end- found-1);
		end = -1;
		do {
			found = end+1;
			end = currReachList.find_first_of(";",end+1);
			string split = currReachList.substr(found,end-found);
			int nameTag = split.find_first_of(":");
			string name = split.substr(0,nameTag);
			string lenStr= split.substr(nameTag+1);
			double lenNum;
			istringstream iss(lenStr);
			iss >> lenNum;
			currStart.toArray[name] = lenNum;
		} while (end !=std::string::npos);
		dict.push_back(currStart);
	}
	for (int i = 0; i < dict.size();i++) {
		place[dict[i].from] = i;
		pointPlace[i] = dict[i].from;
	}
	placeGraph.init(place.size());
	for(int i = 0; i < dict.size(); i++){
		string from = dict[i].from;
		map<string, int>* toArray = &dict[i].toArray;
		for (map<string, int>::iterator iter = toArray->begin();
			iter != toArray->end();iter++) {
			placeGraph.setEdge(place[from], place[iter->first],iter->second);
		}
	}
	//init position xoy
	string xoyStr = "";
	getline(file, xoyStr);
	while (!file.eof()) {
		XOY temp;
		xoyStr = "";
		getline(file, xoyStr);
		int found= xoyStr.find_first_of(":");
		string name = xoyStr.substr(0,found);
		xoyStr= xoyStr.substr(found+1);
		found = xoyStr.find_first_of(";");
		istringstream issX(xoyStr.substr(0, found));
		issX >> temp.x;
		xoyStr = xoyStr.substr(found+1);
		found = xoyStr.find_first_of(";");
		istringstream issY(xoyStr.substr(0, found));
		issY >> temp.y;
		position[name] = temp;
	}
	file.close();
	return true;
}

MIN_PATH placeMap::minDistance(string from, string to) {
	MIN_PATH back;
	int start; int end;
	std::map<string, int>::iterator iter= place.find(from);
	if (iter == place.end()) {
		back.pathLen = 0;
		return back;
	}
	else if ((iter = place.find(to)) == place.end()) {
		back.pathLen = 0;
		return back;
	}
	else {
		start = place[from];
		end = place[to];
	}
	vector<vector<int>> temp(placeGraph.n());
	int* D = (int*)malloc(sizeof(int)*placeGraph.n());
	for (int i = 0; i < placeGraph.n();i++) {
		placeGraph.setMark(i,UNVISITED);
		D[i] = placeGraph.weight(start,i);
		if (D[i]== NO_EDGE) {
			D[i]= CANNOT_REACH;
		}
		temp[i].push_back(i);
	}
	int v, w;
	for (size_t i = 0; i < placeGraph.n(); i++)
	{
		v = minDisHelp(D);
		if (D[v]== CANNOT_REACH) {
			break;
		}
		placeGraph.setMark(v,VISITED);
		for (w = placeGraph.first(v); w < placeGraph.n();w=placeGraph.next(v,w)) {
			int seeTemp = placeGraph.weight(v, w);
			if (D[w]>D[v]+ seeTemp) {
				D[w] = D[v] + placeGraph.weight(v, w);
				temp[w].clear();
				for (vector<int>::iterator iter = temp[v].begin();
					iter != temp[v].end();iter++) {
					temp[w].push_back(*iter);
				}
				temp[w].push_back(w);
			}
		}
	}
	back.pathLen = D[end];
	back.path.push_back(start);
	for (int i = 0; i < temp[end].size();i++) {
		back.path.push_back(temp[end][i]);
	}
	return back;

}

int placeMap::minDisHelp(int* D) {
	int i = 0, v = -1;
	for (i = 0; i < placeGraph.n();i++) {
		if (placeGraph.getMark(i) == UNVISITED) { v = i; break; }
	}
	for (i++; i < placeGraph.n(); i++) {
		if (placeGraph.getMark(i) == UNVISITED && D[i]<D[v]) { 
			v = i; 
		}
	}
	return v;
}


void run() {
	//n个地点 地点坐标 道路 道路距离 道路方向 需要有一些单行道(可以自己编一些道路)
	//获取输入两点之间的最短距离
	placeMap test;
	//test.fileName();
	//读入的文件必须是ASCII编码格式中文才能正确显示
	test.readPlaceFile();
	string from = "七舍";
	string to = "体育馆";
	while (true) {
		from = ""; to = "";
		test.printPlace();
		cout << "input 'out' to quit!" << endl;
		cout << "from: ";cin >> from;
		if (strcmp(from.c_str(),"out")==0) {
			return;
		}
		cout << "to: "; cin >> to;
		if (strcmp(to.c_str(), "out") == 0) {
			return;
		}
		MIN_PATH pathTest = test.minDistance(from, to);
		if (pathTest.pathLen == 0) {
			cout << "没有道路!" << endl;
			continue;
		}
		initgraph(1500, 700);
		setlinestyle(PS_DASH, 1);
		test.drawMap();
		cout << "路径:" << from;
		for (int i = 1; i < pathTest.path.size(); i++) {
			cout << "->" << test.placeName(pathTest.path[i]);
		}
		cout << endl;
		cout << "路径长度:" << pathTest.pathLen << endl;
		setlinestyle(PS_DASH);
		_getch();
		test.drawPath(pathTest.path);
		_getch();// 按任意键退出
		closegraph();
		system("cls");
	}
	return;
}

int main() {
	run();
	//place_file.dat
	return 0;
}