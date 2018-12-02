#include<stdlib.h>
#include<iostream>
#include<Windows.h>
#include<time.h>
#include<graphics.h>
#include <conio.h>
using namespace std;
const int defaultSize = 0;
const int Width = 1280;               // 绘图区宽
const int Height = 720;               // 绘图区高
const int block_size = 20;           // 块边长

									 //需要一个数据结构:可以随机选取一个存放的单元格的位置并弹出
									 //一个值得思考的问题:realloc重新调整AList大小产生的空间是否比LList插入新节点高效
									 //list by array can be reset
template<typename E>
class REAList {
private:
	int curr;
	int listSize;
	int maxSize;
	E* listArray;
	int growSize;
public:
	REAList(int growS, int size = defaultSize) {//defaultSize
		maxSize = size;
		listSize = curr = 0;
		listArray = new E[maxSize];
		growSize = growS;
	}
	~REAList() {
		delete[] listArray;
	}
	void clear() {
		listSize = curr = 0;
		delete[] listArray;
		listArray = new E[maxSize];
	}
	void insert(const E it) {
		if (!(listSize < maxSize)) {
			cout << "Attention: listSize>=maxSize!  listSize is reset now!" << endl;
			resetArray();
		}
		int i;
		for (i = listSize; i > curr; i--) {
			listArray[i] = listArray[i - 1];
		}
		listArray[curr] = it;
		listSize++;
	}
	void append(const E it) {
		if (!(listSize < maxSize)) {
			cout << "Attention: listSize>=maxSize!  listSize is reset now!" << endl;
			resetArray();
		}
		listArray[listSize++] = it;//不写listArray[listSize+1] = it;listSize++;避免将listSize运算两次
	}
	E remove() {
		if (!(curr >= 0 && curr < listSize)) {
			printf("error: not found item!\n");
			E* temp = new E;//此处有问题；不可以这样写；只为了可以编译通过
			listSize--;
			return *temp;
		}
		E it = listArray[curr];
		for (int i = curr; i < listSize - 1; i++) {
			listArray[i] = listArray[i + 1];
		}
		listSize--;
		return it;
	}
	void moveToStart() { curr = 0; }
	void moveToEnd() { curr = listSize - 1; }
	void prev() {
		if (curr != 0)curr--;
	}
	void next() {
		if (curr < listSize)curr++;
	};
	int length() const {
		return listSize;
	}
	int currPos() const {
		return curr;
	}
	void moveToPos(int pos) {
		if (!(pos >= 0 && pos <= listSize)) {
			printf("error: can not find!");
			return;
		}
		curr = pos;
	}
	const E& getValues() const {
		return listArray[curr];
	}
	void resetArray() {
		listArray = (E*)realloc(listArray, maxSize + sizeof(E)*growSize);
	}
};
//基于随机prim算法
typedef struct { int visit; int UP; int RIGHT; int DOWN; int LEFT; }WALL;
typedef struct { int x; int y; WALL wall; }PLACE;
WALL** maze;//迷宫,每个元素含有四面墙
REAList<PLACE>* wall;//wall存储蓝色2(不确定的墙)
int mazeSize;
/*黄色0(未访问的迷宫单元)
红色1(访问过的迷宫单元)
灰色0(墙未打通)灰色->DARKGRAY
红色1(墙打通)
蓝色2(墙不确定)(不确定的墙仍然未打通)*/
void drawWall(int wall, int opersiteWall, int isSide, int direc, int *place) {
	COLORREF color;
	if ((0 == wall) && (0 == opersiteWall)) {
		//color=black
		color = DARKGRAY;//墙未打通
	}
	else if ((1 == wall) && (1 == opersiteWall)) {
		//color=white
		color = RED;//墙打通
	}
	else if ((2 == wall) && (2 == opersiteWall)) {
		color = BLUE;//墙不确定
	}
	if (isSide) {
		color = DARKGRAY;//墙未打通
	}
	switch (direc)
	{
	case 0://UP
		setfillcolor(color);
		fillrectangle(place[0], place[1] - block_size, place[0] + block_size, place[1]);
		break;
	case 1://RIGHT
		setfillcolor(color);
		fillrectangle(place[0] + block_size, place[1], place[0] + block_size * 2, place[1] + block_size);
		break;
	case 2://DOWN
		setfillcolor(color);
		fillrectangle(place[0], place[1] + block_size, place[0] + block_size, place[1] + block_size * 2);
		break;
	case 3://LEFT
		setfillcolor(color);
		fillrectangle(place[0] - block_size, place[1], place[0], place[1] + block_size);
		break;
	}
}
void drawMaze(int size) {
	// 获取窗口句柄
	HWND hwnd = GetHWnd();
	// 设置窗口标题文字
	MoveWindow(hwnd, 0, 0, Width, Height, false);//窗体句柄,x,y.新宽度,新高度,是否重画
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			int	x = (2 * j + 1)*block_size;//i是行 x是列
			int	y = (2 * i + 1)*block_size;
			//setfillcolor(GREEN);//不要删除这一句
			setfillcolor(DARKGRAY);
			fillrectangle(x - block_size, y - block_size, x, y);//fillrectangle(左边的位置,上边的位置,右边的位置,下边的位置);
			fillrectangle(x + block_size, y - block_size, x + block_size * 2, y);
			fillrectangle(x + block_size, y + block_size, x + block_size * 2, y + block_size * 2);
			fillrectangle(x - block_size, y + block_size, x, y + block_size * 2);
			int place[2] = { x,y };
			if (i - 1 >= 0) {
				drawWall(maze[i][j].UP, maze[i - 1][j].DOWN, 0, 0, place);
			}
			else {
				drawWall(maze[i][j].UP, 1, 1, 0, place);
			}
			if (j + 1 < size) {
				drawWall(maze[i][j].RIGHT, maze[i][j + 1].LEFT, 0, 1, place);
			}
			else {
				drawWall(maze[i][j].RIGHT, 1, 1, 1, place);
			}
			if (i + 1 < size) {
				drawWall(maze[i][j].DOWN, maze[i + 1][j].UP, 0, 2, place);
			}
			else {
				drawWall(maze[i][j].DOWN, 1, 1, 2, place);
			}
			if (j - 1 >= 0) {
				drawWall(maze[i][j].LEFT, maze[i][j - 1].RIGHT, 0, 3, place);
			}
			else {
				drawWall(maze[i][j].LEFT, 1, 1, 3, place);
			}
			if (maze[i][j].visit) {
				setfillcolor(RED);//cell被访问过为RED  cell未被访问过为YELLOW
			}
			else {
				setfillcolor(YELLOW);
			}

			fillrectangle(x, y, x + block_size, y + block_size);
		}
	}

}
int getSize() {
	int size = 0;
	while (0 == size % 2) {
		cout << "input size(odd) of maze:   ";//奇数
		cin >> size;
	}
	getchar();
	cout << "size of maze you set:  " << size * size << endl;
	return size;
}
void addUnSureWall(const int x, const int y, const int flag) //flag为2表示起始位置调用 0表示其他位置调用
{
	for (int i = 0; i < 4; i++) {
		PLACE place = { x,y };
		bool hasEidt = false;
		switch (i)
		{
		case 0://UP
			if (flag == maze[y][x].UP) {
				maze[y][x].UP = 2;
				if (y - 1 >= 0)
				{
					maze[y - 1][x].DOWN = 2;
				}
				else {
					hasEidt = false;
					maze[y][x].UP = 0;
					break;
				}
				place.wall.UP = 2;
				place.wall.RIGHT = 0;
				place.wall.DOWN = 0;
				place.wall.LEFT = 0;
				hasEidt = true;
			}
			break;
		case 1://RIGHT
			if (flag == maze[y][x].RIGHT) {
				maze[y][x].RIGHT = 2;
				if (x + 1<mazeSize)
				{
					maze[y][x + 1].LEFT = 2;
				}
				else {
					hasEidt = false;
					maze[y][x].RIGHT = 0;
					break;
				}
				place.wall.UP = 0;
				place.wall.RIGHT = 2;
				place.wall.DOWN = 0;
				place.wall.LEFT = 0;
				hasEidt = true;
			}
			break;
		case 2://DOWN
			if (flag == maze[y][x].DOWN) {
				maze[y][x].DOWN = 2;
				if (y + 1<mazeSize)
				{
					maze[y + 1][x].UP = 2;
				}
				else {
					hasEidt = false;
					maze[y][x].DOWN = 0;
					break;
				}
				place.wall.UP = 0;
				place.wall.RIGHT = 0;
				place.wall.DOWN = 2;
				place.wall.LEFT = 0;
				hasEidt = true;
			}
			break;
		case 3:
			if (flag == maze[y][x].LEFT) {
				maze[y][x].LEFT = 2;
				if (x - 1 >= 0)
				{
					maze[y][x - 1].RIGHT = 2;
				}
				else {
					hasEidt = false;
					maze[y][x].LEFT = 0;
					break;
				}
				place.wall.UP = 0;
				place.wall.RIGHT = 0;
				place.wall.DOWN = 0;
				place.wall.LEFT = 2;
				hasEidt = true;
			}
			break;
		}
		if (hasEidt) {
			wall->append(place);
		}
	}
}
void initStart() {
	const int randXPlace = rand() % mazeSize;
	const int randYPlace = rand() % mazeSize;
	maze[randYPlace][randXPlace].visit = 1;
	maze[randYPlace][randXPlace].UP = (randYPlace == 0) ? 0 : 2;
	maze[randYPlace][randXPlace].RIGHT = (randXPlace == mazeSize - 1) ? 0 : 2;
	maze[randYPlace][randXPlace].DOWN = (randYPlace == mazeSize - 1) ? 0 : 2;
	maze[randYPlace][randXPlace].LEFT = (randXPlace == 0) ? 0 : 2;
	//标记起始单元的蓝色2(不确定的墙)
	if (0 != randYPlace) {
		maze[randYPlace - 1][randXPlace].DOWN = maze[randYPlace][randXPlace].UP;
	}
	if (mazeSize - 1 != randXPlace) {
		maze[randYPlace][randXPlace + 1].LEFT = maze[randYPlace][randXPlace].RIGHT;
	}
	if (mazeSize - 1 != randYPlace) {
		maze[randYPlace + 1][randXPlace].UP = maze[randYPlace][randXPlace].DOWN;
	}
	if (0 != randXPlace) {
		maze[randYPlace][randXPlace - 1].RIGHT = maze[randYPlace][randXPlace].LEFT;
	}
	//更新相邻迷宫单元的wall值
	addUnSureWall(randXPlace, randYPlace, 2);
	//蓝色2(不确定的墙)加入wall
}
void eidtPlace(const PLACE place, const int wallState) //只修改单面墙的状态
{
	/*wallState==0;接下来这面墙标记为未打通
	wallState==1;接下来这面墙标记为打通
	wallState==2;接下来这面墙标记为不确定*/
	int flag;
	//下一个状态是2,即把灰色0变成蓝色2,墙加入表
	if (2 == wallState)
	{
		flag = 0;
	}
	//下一个状态是1,即把蓝色2变成红色1,墙打通
	if (1 == wallState)
	{
		flag = 2;
	}
	//下一个状态是0,即把蓝色2变成灰色0,墙出表
	if (0 == wallState)
	{
		flag = 2;
	}
	int wallDirct[4] = { place.wall.UP ,place.wall.RIGHT,place.wall.DOWN,place.wall.LEFT };
	int direct = -1;
	for (int i = 0; i < 4; i++) {
		if (flag == wallDirct[i]) {
			direct = i;
			break;
		}
	}
	switch (direct) {
	case 0://UP
		maze[place.y][place.x].UP = wallState;
		if (0 != place.y) {
			maze[place.y - 1][place.x].DOWN = wallState;
			//下一个状态是1,即把蓝色2变成红色1,墙打通,新单元的墙加入表
			if (1 == wallState)
			{
				maze[place.y - 1][place.x].visit = 1;
				addUnSureWall(place.x, place.y - 1, 0);
			}
		}
		break;
	case 1://RIGHT
		maze[place.y][place.x].RIGHT = wallState;
		if (mazeSize - 1 != place.x) {
			maze[place.y][place.x + 1].LEFT = wallState;
			//下一个状态是1,即把蓝色2变成红色1,墙打通,新单元的墙加入表
			if (1 == wallState)
			{
				maze[place.y][place.x + 1].visit = 1;
				addUnSureWall(place.x + 1, place.y, 0);
			}
		}
		break;
	case 2://DOWN
		maze[place.y][place.x].DOWN = wallState;
		if (mazeSize - 1 != place.y) {
			maze[place.y + 1][place.x].UP = wallState;
			//下一个状态是1,即把蓝色2变成红色1,墙打通,新单元的墙加入表
			if (1 == wallState)
			{
				maze[place.y + 1][place.x].visit = 1;
				addUnSureWall(place.x, place.y + 1, 0);
			}
		}
		break;
	case 3://LEFT
		maze[place.y][place.x].LEFT = wallState;
		if (0 != place.x) {
			maze[place.y][place.x - 1].RIGHT = wallState;
			//下一个状态是1,即把蓝色2变成红色1,墙打通,新单元的墙加入表
			if (1 == wallState)
			{
				maze[place.y][place.x - 1].visit = 1;
				addUnSureWall(place.x - 1, place.y, 0);
			}
		}
		break;
	}
}
bool isVisit(const PLACE place) {
	//默认进来的place的墙是不会有边界单元向边界外方向的
	const int wallJudge[4] = { place.wall.UP,place.wall.RIGHT,place.wall.DOWN,place.wall.LEFT };
	int wallDirect = -1;
	bool visit = false;
	for (int i = 0; i < 4; i++) {
		if (2 == wallJudge[i]) { wallDirect = i; }
	}
	switch (wallDirect)
	{
	case 0://UP
		visit = (maze[place.y][place.x].visit == 1) && (maze[place.y - 1][place.x].visit == 1);
		break;
	case 1://RIGHT
		visit = (maze[place.y][place.x].visit == 1) && (maze[place.y][place.x + 1].visit == 1);
		break;
	case 2://DOWN
		visit = (maze[place.y][place.x].visit == 1) && (maze[place.y + 1][place.x].visit == 1);
		break;
	case 3://LEFT
		visit = (maze[place.y][place.x].visit == 1) && (maze[place.y][place.x - 1].visit == 1);
		break;
	}
	/*visit==0;墙两面的单元格有一个没有访问过
	visit==1;墙两面的单元格都访问过*/
	return visit;
}
int main() {
	srand(time(NULL));
	mazeSize = getSize();
	maze = (WALL **)malloc(sizeof(WALL *)*(mazeSize));
	for (int i = 0; i < mazeSize; i++) {
		maze[i] = (WALL *)malloc(sizeof(WALL)*mazeSize);
	}
	for (int i = 0; i < mazeSize; i++) {
		for (int j = 0; j < mazeSize; j++) {
			maze[i][j].visit = 0;//0代表未访问过
			maze[i][j].UP = 0;//0表示墙未打通
			maze[i][j].DOWN = 0;
			maze[i][j].LEFT = 0;
			maze[i][j].RIGHT = 0;
		}
	}
	wall = new REAList<PLACE>(mazeSize*mazeSize * 4, 10);
	initStart();
	initgraph(Width, Height);
	drawMaze(mazeSize);
	FlushBatchDraw();
	int wallLength = wall->length();
	bool judgeWhile = (0 != wallLength);
	while (judgeWhile) {
		wall->moveToPos(rand() % wall->length());
		PLACE place = wall->getValues();
		bool isWall_1 = false;
		/*isVisit()==0;墙两面的单元格有一个没有访问过
		isVisit()==1;墙两面的单元格都访问过*/
		isWall_1 = !(isVisit(place));
		if (isWall_1) {
			wall->remove();//该墙不再是unsure
			eidtPlace(place, 0);//蓝色2变成灰色0
			eidtPlace(place, 1);//蓝色2变成红色1

		}
		else {
			wall->remove();//该墙不再是unsure
			eidtPlace(place, 0);//蓝色2变成灰色0
								//修改墙
		}

		drawMaze(mazeSize);
		FlushBatchDraw();
		wallLength = wall->length();
		judgeWhile = (0 != wallLength);
	}
	drawMaze(mazeSize);
	FlushBatchDraw();
	getchar();
	closegraph();
	return 0;
}
