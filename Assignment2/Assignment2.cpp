#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <unordered_set>
using namespace std;

#define reset "\033[0m"

class ColoredChar {
public:
	char character;
	string color;
	ColoredChar(char ch, const string& clr) : character(ch), color(clr) {}

	string getAnsiColorCode(const string& color) {
		if (color == "red") return "\033[31m";
		if (color == "green") return "\033[32m";
		if (color == "yellow") return "\033[33m";
		if (color == "blue") return "\033[34m";
		if (color == "magenta") return "\033[35m";
		if (color == "cyan") return "\033[36m";
		if (color == "white") return "\033[37m";
		return ""; 
	}
};

class Board {
public:
	const int BOARD_WIDTH = 60;
	const int BOARD_HEIGHT = 20;

	int boardArea = BOARD_HEIGHT * BOARD_WIDTH;

	vector<vector<ColoredChar>> grid;

	Board() : grid(BOARD_HEIGHT, vector<ColoredChar>(BOARD_WIDTH, ColoredChar(' ', "white"))) {}
	void print() {
		for (auto& row : grid) {
			for (ColoredChar clrCh : row) {
				cout << clrCh.getAnsiColorCode(clrCh.color) << clrCh.character << reset;
			}
			cout << "\n";
		}
	}

	void cleanBoard() {
		for (int i = 0; i < BOARD_HEIGHT; ++i) {
			for (int j = 0; j < BOARD_WIDTH; ++j) {
				grid[i][j] = ColoredChar(' ', "white");
			}
		}
	}
};


class Figure {
public:
	virtual void draw(Board& board) = 0;
	virtual void drawFilled(Board& board) = 0;
	virtual void getFigure(int id) const = 0;
	virtual bool getFilledState() const = 0;
	virtual void setColor(string newColor) = 0;
	virtual const vector<pair<int, int>>& getPoints() const = 0;
	virtual ~Figure() = default;
protected:
	vector<pair<int, int>> points;
};

class Triangle: public Figure {
public:
	int trigX, trigY, trigHeight;
	string color;
	bool fill = false;

	Triangle(int coordinateX, int coordinateY, double Height, string Color, bool Fill) : trigX(coordinateX), trigY(coordinateY), trigHeight(Height), color(Color), fill(Fill) {}

	void draw(Board& board) override {
		if (trigHeight <= 0) return;
		for (int i = 0; i < trigHeight; ++i) {
			int leftMost = trigX - i; 
			int rightMost = trigX + i; 
			int posY = trigY + i; 
			if (posY < board.BOARD_HEIGHT) {
				if (leftMost >= 0 && leftMost < board.BOARD_WIDTH)
				{
					board.grid[posY][leftMost] = ColoredChar('*', color);
					points.emplace_back(leftMost, posY);
				}
				if (rightMost >= 0 && rightMost < board.BOARD_WIDTH && leftMost != rightMost)
				{
					board.grid[posY][rightMost] = ColoredChar('*', color);
					points.emplace_back(rightMost, posY);
				}
			}
		}
		for (int j = 0; j < 2 * trigHeight - 1; ++j) {
			int baseX = trigX - trigHeight + 1 + j;
			int baseY = trigY + trigHeight - 1;
			if (baseX >= 0 && baseX < board.BOARD_WIDTH && baseY < board.BOARD_HEIGHT)
				board.grid[baseY][baseX] = ColoredChar('*', color);
		}
	}

	void drawFilled(Board& board) override {
		if (trigHeight <= 0) return; 

		for (int i = 0; i < trigHeight; ++i) {
			int leftMost = trigX - i;
			int rightMost = trigX + i;
			int posY = trigY + i;

			if (posY < board.BOARD_HEIGHT) {
				for (int x = leftMost; x <= rightMost; ++x) {
					if (x >= 0 && x < board.BOARD_WIDTH) {
						board.grid[posY][x] = ColoredChar('*', color);
						points.emplace_back(x, posY);
					}
				}
			}
		}

		int baseY = trigY + trigHeight - 1;
		if (baseY < board.BOARD_HEIGHT) {
			for (int j = 0; j < 2 * trigHeight - 1; ++j) {
				int baseX = trigX - trigHeight + 1 + j;
				if (baseX >= 0 && baseX < board.BOARD_WIDTH) {
					board.grid[baseY][baseX] = ColoredChar('*', color);
					points.emplace_back(baseX, baseY);
				}
			}
		}
	}

	void getFigure(int id) const override {
		cout << id << " triangle (" << trigX << ", " << trigY << ") height " << trigHeight << " " << color << endl;
	}

	bool getFilledState() const override {
		return fill;
	}

	const vector<pair<int, int>>& getPoints() const override {
		return points; 
	}

	void setColor(string newColor) override {
		color = newColor;
	}

};

class Square : public Figure {
public:
	int sqrX, sqrY, sqrSideLength;
	string color;
	bool fill;

	Square(int coordinateX, int coordinateY, double sideLength, string Color, bool Fill) : sqrX(coordinateX), sqrY(coordinateY), sqrSideLength(sideLength), color(Color), fill(Fill) {}

	void draw(Board& board) override {
		for (int i = 0; i < sqrSideLength; ++i) {
			if (sqrX + i < board.BOARD_WIDTH) {
				board.grid[sqrY][sqrX + i] = ColoredChar('*', color);
				board.grid[sqrY + sqrSideLength - 1][sqrX + i] = ColoredChar('*', color);
				points.emplace_back(sqrX+i, sqrY);
				points.emplace_back(sqrX + i, sqrY + sqrSideLength - 1);
			}
			if (sqrY + i < board.BOARD_HEIGHT) {
				board.grid[sqrY + i][sqrX] = ColoredChar('*', color);
				board.grid[sqrY + i][sqrX + sqrSideLength - 1] = ColoredChar('*', color);
				points.emplace_back(sqrX, sqrY + i);
				points.emplace_back(sqrX + sqrSideLength - 1, sqrY + i);
			}
		}
	}

	void drawFilled(Board& board) override {
		for (int i = 0; i < sqrSideLength; ++i) {
			for (int j = 0; j < sqrSideLength; ++j) {
				if ((sqrX + j < board.BOARD_WIDTH) && (sqrY + i < board.BOARD_HEIGHT)) {
					board.grid[sqrY + i][sqrX + j] = ColoredChar('*', color);
					points.emplace_back(sqrX+j, sqrY+i);
				}
			}
		}
	}

	void getFigure(int id) const override {
		cout << id << " square (" << sqrX << ", " << sqrY << ") side length " << sqrSideLength << " " << color << endl;
	}

	bool getFilledState() const override {
		return fill;
	}

	const vector<pair<int, int>>& getPoints() const override {
		return points;
	}

	void setColor(string newColor) override {
		color = newColor;
	}
};

class Rectangle : public Figure {
public:
	int rectX, rectY, rectWidth, rectHeight;
	string color;
	bool fill;

	Rectangle(int coordinateX, int coordinateY, double width, double height, string Color, bool Fill) : rectX(coordinateX), rectY(coordinateY), rectWidth(width), rectHeight(height), color(Color), fill(Fill){}

	void draw(Board& board) override {
		for (int i = 0; i < rectWidth; ++i) {
			if (rectX + i < board.BOARD_WIDTH) {
				board.grid[rectY][rectX + i] = ColoredChar('*', color);
				board.grid[rectY + rectHeight - 1][rectX + i] = ColoredChar('*', color);
				points.emplace_back(rectX + i, rectY);
				points.emplace_back(rectX + i, rectY + rectHeight - 1);
			}
		}
		for (int j = 0; j < rectHeight; ++j) {
			if (rectY + j < board.BOARD_HEIGHT) {
				board.grid[rectY + j][rectX] = ColoredChar('*', color);
				board.grid[rectY + j][rectX + rectWidth - 1] = ColoredChar('*', color);
				points.emplace_back(rectX, rectY+j);
				points.emplace_back(rectX + rectWidth - 1, rectY + j);
			}
		}
	}

	void drawFilled(Board& board) override {
		for (int j = 0; j < rectHeight; ++j) {
			for (int i = 0; i < rectWidth; ++i) {
				if ((rectX + i < board.BOARD_WIDTH) && (rectY + j < board.BOARD_HEIGHT)) {
					board.grid[rectY + j][rectX + i] = ColoredChar('*', color);
					points.emplace_back(rectX+i, rectY +j);
				}
			}
		}
	}

	void getFigure(int id) const override {
		cout << id << " rectangle (" << rectX << ", " << rectY << ") width " << rectWidth << " height " << rectHeight << " " << color << endl;
	}

	bool getFilledState() const override {
		return fill;
	}

	const vector<pair<int, int>>& getPoints() const override {
		return points;
	}

	void setColor(string newColor) override {
		color = newColor;
	}
};

class Circle : public Figure {
public:
	int circX, circY, circRadius;
	string color;
	bool fill;

	Circle(int coordinateX, int coordinateY, double radius, string Color, bool Fill) : circX(coordinateX), circY(coordinateY), circRadius(radius), color(Color), fill(Fill) {}

	void draw(Board& board) override {
		int x = 0;
		int y = circRadius;
		int p = 1 - circRadius; 

		while (x <= y) {
			if (circX + x < board.BOARD_WIDTH && circY + y < board.BOARD_HEIGHT) {
				board.grid[circY + y][circX + x] = ColoredChar('*', color);
				points.emplace_back(circX + x, circY + y); 
			}
			if (circX - x >= 0 && circY + y < board.BOARD_HEIGHT) {
				board.grid[circY + y][circX - x] = ColoredChar('*', color);
				points.emplace_back(circX - x, circY + y); 
			}
			if (circX + x < board.BOARD_WIDTH && circY - y >= 0) {
				board.grid[circY - y][circX + x] = ColoredChar('*', color);
				points.emplace_back(circX + x, circY - y); 
			}
			if (circX - x >= 0 && circY - y >= 0) {
				board.grid[circY - y][circX - x] = ColoredChar('*', color);
				points.emplace_back(circX - x, circY - y); 
			}
			if (circX + y < board.BOARD_WIDTH && circY + x < board.BOARD_HEIGHT) {
				board.grid[circY + x][circX + y] = ColoredChar('*', color);
				points.emplace_back(circX + y, circY + x); 
			}
			if (circX - y >= 0 && circY + x < board.BOARD_HEIGHT) {
				board.grid[circY + x][circX - y] = ColoredChar('*', color);
				points.emplace_back(circX - y, circY + x); 
			}
			if (circX + y < board.BOARD_WIDTH && circY - x >= 0) {
				board.grid[circY - x][circX + y] = ColoredChar('*', color);
				points.emplace_back(circX + y, circY - x); 
			}
			if (circX - y >= 0 && circY - x >= 0) {
				board.grid[circY - x][circX - y] = ColoredChar('*', color);
				points.emplace_back(circX - y, circY - x); 
			}

			x++;

			if (p < 0) {
				p += 2 * x + 1; 
			}
			else {
				y--; 
				p += 2 * (x - y) + 1; 
			}
		}
	}

	void drawFilled(Board& board) override {
		int x = 0;
		int y = circRadius;
		int p = 1 - circRadius;

		while (x <= y) {
			for (int i = -x; i <= x; ++i) {
				if (circY + y < board.BOARD_HEIGHT) {
					if (circX + i < board.BOARD_WIDTH && circX + i >= 0) {
						board.grid[circY + y][circX + i] = ColoredChar('*', color);
						points.emplace_back(circX + i, circY + y);
					}
					if (circX + i < board.BOARD_WIDTH && circX + i >= 0) {
						board.grid[circY - y][circX + i] = ColoredChar('*', color);
						points.emplace_back(circX + i, circY - y);
					}
				}
			}
			for (int i = -y; i <= y; ++i) {
				if (circY + x < board.BOARD_HEIGHT) {
					if (circX + i < board.BOARD_WIDTH && circX + i >= 0) {
						board.grid[circY + x][circX + i] = ColoredChar('*', color);
						points.emplace_back(circX + i, circY + x);
					}
					if (circX + i < board.BOARD_WIDTH && circX + i >= 0) {
						board.grid[circY - x][circX + i] = ColoredChar('*', color);
						points.emplace_back(circX + i, circY - x);
					}
				}
			}
			x++;
			if (p < 0) {
				p += 2 * x + 1;
			}
			else {
				y--;
				p += 2 * (x - y) + 1;
			}
		}
	}

	void getFigure(int id) const override {
		cout << id << " circle (" << circX << ", " << circY << ") radius " << circRadius << " " << color << endl;
	}

	bool getFilledState() const override {
		return fill;
	}

	const vector<pair<int, int>>& getPoints() const override {
		return points;
	}

	void setColor(string newColor) override {
		color = newColor;
	}
};

class System {
public:
	map<int, unique_ptr<Figure>> figures;
	void run(Board& board);
	bool isNumeric(const string& str);
	bool isValidColor(const string& color);
private:
	void printCommands();
	void draw(Board& board); 
	void list(); 
	void shapes(); 
	void add(Board& board, stringstream& sss);
	void undo(Board& board); 
	void clear(Board& board); 
	void save(stringstream& sss);
	void load(stringstream& sss);
	Figure* select(stringstream& sss);
	void remove(Figure* figure);
	void edit(Figure* figure, stringstream& sss);
	void paint(Figure* figure, stringstream& sss);
	bool checkCircle(Board& board);
	bool checkSquare(Board& board);
	bool checkRectangle(Board& board);
	bool checkTriangle(Board& board);
	bool isFigureNew();
	void cleanData();
	string input;
	string command;
	string fill;
	string figure;
	string strX, strY;
	string color;
	int x, y = -1;
	int ID;
	string triangleHeight, squareLength, rectangleWidth, rectangleHeight, circleRadius;
	string filePath;
	double doubleTrigHeight, doubleCircRadius, doubleRectWidth, doubleRectHeight, doubleSqrLength;
};

bool System::isFigureNew() {
	for (const auto& pair : figures) {
		const auto& fig = pair.second;

		if (figure == "triangle") {
			if (auto* triangle = dynamic_cast<Triangle*>(fig.get())) {
				if (triangle->trigX == x && triangle->trigY == y && triangle->trigHeight == doubleTrigHeight && triangle->color == color) {
					return false;
				}
			}
		}
		else if (figure == "square") {
			if (auto* square = dynamic_cast<Square*>(fig.get())) {
				if (square->sqrX == x && square->sqrY == y && square->sqrSideLength == doubleSqrLength && square->color == color) {
					return false;
				}
			}
		}
		else if (figure == "rectangle") {
			if (auto* rectangle = dynamic_cast<Rectangle*>(fig.get())) {
				if (rectangle->rectX == x && rectangle->rectY == y && rectangle->rectWidth == doubleRectWidth && rectangle->rectHeight == doubleRectHeight && rectangle->color == color) {
					return false;
				}
			}
		}
		else if (figure == "circle") {
			if (auto* circle = dynamic_cast<Circle*>(fig.get())) {
				if (circle->circX == x && circle->circY == y && circle->circRadius == doubleCircRadius && circle->color == color) {
					return false;
				}
			}
		}
	}
	return true;
}

void System::cleanData() {
	x = y = -1;
	color = "";
	strX, strY = "";
	triangleHeight = squareLength = rectangleWidth = rectangleHeight = circleRadius = "";
}

bool System::checkTriangle(Board& board) {
	double trigArea = doubleTrigHeight * doubleTrigHeight/2;
	if (board.boardArea <= trigArea) {
		return false;
	}
	else {
		return true;
	}
}

bool System::checkCircle(Board& board) {
	double circArea = doubleCircRadius * doubleCircRadius * 22 / 7;
	if (board.boardArea <= circArea) {
		return false;
	}
	else {
		return true;
	}
}

bool System::checkSquare(Board& board) {
	double sqrArea = doubleSqrLength*doubleSqrLength;
	if (board.boardArea <= sqrArea) {
		return false;
	}
	else {
		return true;
	}
}

bool System::checkRectangle(Board& board) {
	double rectArea = doubleRectHeight*doubleRectWidth;
	if (board.boardArea <= rectArea) {
		return false;
	}
	else {
		return true;
	}
}

bool System::isNumeric(const std::string& str) {
	if (str.empty()) return false;

	for (char c : str) {
		if (!isdigit(c)) {
			return false;
		}
	}
	return true;
}

bool System::isValidColor(const string& color) {
	unordered_set<string> validColors = { "red", "green", "blue", "yellow", "black", "white" };

	return validColors.find(color) != validColors.end();
}

void System::shapes() {
	cout << "\nShapes:\n"
		<< "triangle coordinates height (color)\n"
		<< "square coordinates sideLength (color)\n"
		<< "rectangle coordinates width height (color)\n"
		<< "circle coordinates radius (color)\n";
}

void System::draw(Board& board) {
	for (const auto& pair : figures) {
		if (pair.second->getFilledState())
		{
			pair.second->drawFilled(board);
		}
		else
		{
			pair.second->draw(board);
		}
	}
	board.print();
}

void System::list() {
	for (const auto& pair : figures) {
		int id = pair.first;
		const auto& figure = pair.second;

		figure->getFigure(id);
	}
	cout << endl;
}

void System::undo(Board& board) {
	int lastId = figures.size();
	figures.erase(lastId);
	cout << "\nLast figure was erased\n\n";
}

void System::clear(Board& board) {
	figures.clear();
	board.cleanBoard();
	cout << "The blackboard was cleared\n\n";
}

void System::add(Board& board, stringstream& sss) {
	sss >> fill;
	if (fill == "fill") {
		sss >> figure >> strX >> strY;
	}
	else if (fill == "triangle" || fill == "square" || fill == "rectangle" || fill == "circle") {
		figure = fill;
		fill.clear();
		sss >> strX >> strY;
	}
	else 
	{
		cout << "The input is incorrect.\n\n";
	}

	if (isNumeric(strX) && isNumeric(strY)) {
		x = stoi(strX);
		y = stoi(strY);
	}
	if (x >= 0 && y >= 0) {
		if (figure == "triangle") {
			sss >> triangleHeight >> color;
			if (isNumeric(triangleHeight)) {
				doubleTrigHeight = stoi(triangleHeight);
				if (checkTriangle(board) && isFigureNew()) {
					if (fill.empty()) {
						figures[figures.size() + 1] = make_unique<Triangle>(x, y, doubleTrigHeight, color, false);
					}
					else {
						figures[figures.size() + 1] = make_unique<Triangle>(x, y, doubleTrigHeight, color, true);
					}
					cout << "\nThe figure was added\n\n";
				}
				else {
					cout << "\nThe figure is bigger than the board or the figure was already added:(\n\n";
				}
			}
			else {
				cout << "\nThe input is incorrect!\n\n";
			}
		}
		else if (figure == "square") {
			sss >> squareLength >> color;
			if (isNumeric(squareLength)) {
				doubleSqrLength = stoi(squareLength);
				if (checkSquare(board) && isFigureNew()) {
					if (fill.empty()) {
						figures[figures.size() + 1] = make_unique<Square>(x, y, doubleSqrLength, color, false);
					}
					else {
						figures[figures.size() + 1] = make_unique<Square>(x, y, doubleSqrLength, color, true);
					}
					cout << "\nThe figure was added\n\n";
				}
				else {
					cout << "\nThe figure is bigger than the board or the figure was already added:(\n\n";
				}
			}
			else {
				cout << "\nThe input is incorrect!\n\n";
			}
		}
		else if (figure == "rectangle") {
			sss >> rectangleWidth >> rectangleHeight >> color;
			if (isNumeric(rectangleWidth) && isNumeric(rectangleHeight)) {
				doubleRectWidth = stoi(rectangleWidth);
				doubleRectHeight = stoi(rectangleHeight);
				if (checkRectangle(board) && isFigureNew())
				{
					if (fill.empty()) {
						figures[figures.size() + 1] = make_unique<Rectangle>(x, y, doubleRectWidth, doubleRectHeight, color, false);
					}
					else {
						figures[figures.size() + 1] = make_unique<Rectangle>(x, y, doubleRectWidth, doubleRectHeight, color, true);
					}
					cout << "\nThe figure was added\n\n";
				}
				else {
					cout << "\nThe figure is bigger than the board or the figure was already added:(\n\n";
				}
			}
			else {
				cout << "\nThe input is incorrect!\n\n";
			}
		}
		else if (figure == "circle") {
			sss >> circleRadius >> color;
			if (isNumeric(circleRadius)) {
				doubleCircRadius = stoi(circleRadius);
				if (checkCircle(board) && isFigureNew()) {
					if (fill.empty()) {
						figures[figures.size() + 1] = make_unique<Circle>(x, y, doubleCircRadius, color, false);
					}
					else {
						figures[figures.size() + 1] = make_unique<Circle>(x, y, doubleCircRadius, color, true);
					}
					cout << "\nThe figure was added\n\n";
				}
				else {
					cout << "\nThe figure is bigger than the board or the figure was already added:(\n\n";
				}
			}
			else {
				cout << "\nThe input is incorrect!\n\n";
			}
		}
	}
	else {
		cout << "\nThe input is incorrect\n\n";
	}
}

void System::save(stringstream& sss) {
	sss >> filePath;
	ofstream outFile(filePath);

	if (!outFile) {
		cerr << "Error opening file for saving!" << endl;
		return;
	}

	for (const auto& pair : figures) {
		int id = pair.first;
		const auto& figure = pair.second;

		if (auto* circle = dynamic_cast<Circle*>(figure.get())) {
			outFile << id << " circle " << circle->circX << " " << circle->circY << " " << circle->circRadius << " " << circle->color << "\n";
		}
		else if (auto* square = dynamic_cast<Square*>(figure.get())) {
			outFile << id << " square " << square->sqrX << " " << square->sqrY << " " << square->sqrSideLength << " " << square->color << "\n";
		}
		else if (auto* triangle = dynamic_cast<Triangle*>(figure.get())) {
			outFile << id << " triangle " << triangle->trigX << " " << triangle->trigY << " " << triangle->trigHeight << " " << triangle->color << "\n";
		}
		else if (auto* rectangle = dynamic_cast<Rectangle*>(figure.get())) {
			outFile << id << " rectangle " << rectangle->rectX << " " << rectangle->rectY << " " << rectangle->rectWidth << " " << rectangle->rectHeight << " " << rectangle->color << "\n";
		}
	}
	outFile.close();
	cout << "Board was saved to file: " << filePath << "\n\n";
}

void System::load(stringstream& sss) {
	sss >> filePath;
	ifstream infile(filePath);

	if (!infile) {
		cerr << "Error opening file for loading!" << endl;
		return;
	}

	figures.clear();

	string line;
	while (getline(infile, line)) {
		stringstream forLoad(line);
		int id;
		string shape;
		int x, y;

		forLoad >> fill;
		if (fill == "fill") {
			forLoad >> shape >> x >> y;
		}
		else if (fill == "triangle" || fill == "square" || fill == "rectangle" || fill == "circle") {
			shape = fill;
			fill.clear();
			forLoad >> x >> y;
		}

		if (shape == "circle") {
			int radius;
			forLoad >> radius >> color;
			if (fill.empty()) {
				figures[id] = make_unique<Circle>(x, y, radius, color, false);
			}
			else {
				figures[id] = make_unique<Circle>(x, y, radius, color, true);
			}
		}
		else if (shape == "square") {
			int sideLength;
			forLoad >> sideLength >> color;
			if (fill.empty()) {
				figures[id] = make_unique<Square>(x, y, sideLength, color, false);
			}
			else {
				figures[id] = make_unique<Square>(x, y, sideLength, color, true);
			}
		}
		else if (shape == "triangle") {
			int height;
			forLoad >> height >> color;
			if (fill.empty()) {
				figures[id] = make_unique<Triangle>(x, y, height, color, false);
			}
			else {
				figures[id] = make_unique<Triangle>(x, y, height, color, true);
			}
		}
		else if (shape == "rectangle") {
			int width, height;
			forLoad >> width >> height >> color;
			if (fill.empty()) {
				figures[id] = make_unique<Rectangle>(x, y, width, height, color, false);
			}
			else {
				figures[id] = make_unique<Rectangle>(x, y, width, height, color, true);
			}
		}
	}
	infile.close();
	cout << "Board was loaded from " << filePath << endl;
}

Figure* System::select(stringstream& sss) {
	sss >> strX >> strY;
	if (strY == "") {
		if (isNumeric(strX)) {
			ID = stoi(strX);
			for (const auto& pair : figures) {
				if (pair.first == ID) {
					const auto& figure = pair.second;
					figure->getFigure(ID);
					cout << endl;
					return figure.get();
				}
			}
		}
	}
	else {
		if (isNumeric(strX) && isNumeric(strY)) {
			x = stoi(strX);
			y = stoi(strY);
		}
		bool found = false;
		for (auto it = figures.rbegin(); it != figures.rend(); ++it) {
			const auto& figure = it->second;
			const auto& points = figure->getPoints();
			for (const auto& point : points) {
				if (point.first == x && point.second == y) {
					found = true;
					ID = it->first;
					figure->getFigure(it->first);
					cout << endl;
					return figure.get();
					break;
				}
			}
			if (found) {
				break;
			}
		}
		if (!found) {
			cout << "Point (" << x << ", " << y << ") not found in any figure." << endl;
		}
	}
}

void System::remove(Figure* figure) {
		figure->getFigure(ID);
		cout << "was removed\n\n";
		figures.erase(ID);
}

void System::edit(Figure* figure, stringstream& sss) {
	string par1;
	string par2;
	int intPar1, intPar2;
	sss >> par1 >> par2;
	if (auto* circle = dynamic_cast<Circle*>(figure)) {
		if (par2.empty()) {
			if (isNumeric(par1)) {
				intPar1 = stoi(par1);
				circle->circRadius = intPar1;
				cout << "\nRadius of circle was changed\n\n";
			}
			else {
				cout << "\nParameter is not numeric\n\n";
			}
		}
		else {
			cout << "\nWrong number of arguments\n\n";
		}
	}
	else if (auto* rectangle = dynamic_cast<Rectangle*>(figure)) {
		if (!par1.empty() && !par2.empty()) {
			if (isNumeric(par1) || isNumeric(par2)) {
				intPar1 = stoi(par1);
				intPar2 = stoi(par2);
				rectangle->rectWidth = intPar1;
				rectangle->rectHeight = intPar2;
				cout << "\nWidth and height of rectangle were changed\n\n";
			}
			else {
				cout << "\nParameter is not numeric\n\n";
			}
		}
		else {
			cout << "\nWrong number of arguments\n\n";
		}
	}
	else if (auto* triangle = dynamic_cast<Triangle*>(figure)) {
		if (par2.empty()) {
			if (isNumeric(par1)) {
				intPar1 = stoi(par1);
				triangle->trigHeight = intPar1;
				cout << "\nHeight of triangle was changed\n\n";
			}
			else {
				cout << "\nParameter is not numeric\n\n";
			}
		}
		else {
			cout << "\nWrong number of arguments\n\n";
		}
	}
	else if (auto* square = dynamic_cast<Square*>(figure)) {
		if (par2.empty()) {
			if (isNumeric(par1)) {
				intPar1 = stoi(par1);
				square->sqrSideLength = intPar1;
				cout << "\nSide length of square was changed\n\n";
			}
			else {
				cout << "\nParameter is not numeric\n\n";
			}
		}
		else {
			cout << "\nWrong number of arguments\n\n";
		}
	}
}

void System::paint(Figure* figure, stringstream& sss) {
	string newColor;
	sss >> newColor;
	if (isValidColor(newColor)) {
		figure->setColor(newColor);
		figure->getFigure(ID);
	}
	else {
		cout << "\nThere is no such color\n\n";
	}
}

void System::run(Board& board) {
	while (true)
	{
		cleanData();
		board.cleanBoard();
		printCommands();
		getline(cin, input);
		stringstream sss(input);
		sss >> command;
		if (command == "draw")
		{
			draw(board);
		}
		else if (command == "list")
		{
			list();
		}
		else if (command == "shapes")
		{
			shapes();
			cout << endl;
		}
		else if (command == "add")
		{
			add(board, sss);
		}
		else if (command == "undo") {
			undo(board);
		}
		else if (command == "clear") {
			clear(board);
		}
		else if (command == "save"){
			save(sss);
		}
		else if (command == "load") {
			load(sss);
		}
		else if (command == "select"){
			const auto& figure = select(sss);
			command.clear();
			getline(cin, input);
			stringstream sss(input);
			sss >> command;
			if (command == "remove")
			{
				remove(figure);
			}
			else if (command == "edit")
			{
				edit(figure, sss);
			}
			else if (command == "paint")
			{
				paint(figure, sss);
			}
			else if (command == "move")
			{

			}
		}
		else if (command == "exit") {
			return;
		}
		else {
			cout << "The input is wrong!\n\n";
		}

    }
}

void System::printCommands() {
	cout << "Choose a command:\n"
		<< "draw - to draw blackboard to the console\n"
		<< "list - to print all added shapes with their parameters\n"
		<< "shapes - to print a list of all available shapes and parameters for add call\n"
		<< "add - to add shape to the blackboard\n"
		<< "undo - to remove the last added shape from the blackboard\n"
		<< "clear - to remove all shapes from blackboard\n"
		<< "save - to save the blackboard to the file\n"
		<< "load - to load the blackboard from the file\n"
		<< "paint - to paint the selected shape\n"
		<< "exit - to leave\n\n";
}

int main()
{
	Board board;
	System system;
	system.run(board);
}

