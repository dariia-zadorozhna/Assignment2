#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
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
};


class Figure {
public:
	virtual void draw(Board& board) const = 0;
	virtual ~Figure() = default;
};

class Triangle: public Figure {
public:
	int trigX, trigY, trigHeight;
	string color;

	Triangle(int coordinateX, int coordinateY, double Height, string Color) : trigX(coordinateX), trigY(coordinateY), trigHeight(Height), color(Color) {}

	void draw(Board& board) const override {
		if (trigHeight <= 0) return;
		for (int i = 0; i < trigHeight; ++i) {
			int leftMost = trigX - i; 
			int rightMost = trigX + i; 
			int posY = trigY + i; 
			if (posY < board.BOARD_HEIGHT) {
				if (leftMost >= 0 && leftMost < board.BOARD_WIDTH)
				{
					board.grid[posY][leftMost] = ColoredChar('*', color);
				}
				if (rightMost >= 0 && rightMost < board.BOARD_WIDTH && leftMost != rightMost)
				{
					board.grid[posY][rightMost] = ColoredChar('*', color);
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
};

class Square : public Figure {
public:
	int sqrX, sqrY, sqrSideLength;
	string color;

	Square(int coordinateX, int coordinateY, double sideLength, string Color) : sqrX(coordinateX), sqrY(coordinateY), sqrSideLength(sideLength), color(Color) {}

	void draw(Board& board) const override {
		for (int i = 0; i < sqrSideLength; ++i) {
			if (sqrX + i < board.BOARD_WIDTH) {
				board.grid[sqrY][sqrX + i] = ColoredChar('*', color);
				board.grid[sqrY + sqrSideLength - 1][sqrX + i] = ColoredChar('*', color);
			}
			if (sqrY + i < board.BOARD_HEIGHT) {
				board.grid[sqrY + i][sqrX] = ColoredChar('*', color);
				board.grid[sqrY + i][sqrX + sqrSideLength - 1] = ColoredChar('*', color);
			}
		}
	}
};

class Rectangle : public Figure {
public:
	int rectX, rectY, rectWidth, rectHeight;
	string color;

	Rectangle(int coordinateX, int coordinateY, double width, double height, string Color) : rectX(coordinateX), rectY(coordinateY), rectWidth(width), rectHeight(height), color(Color) {}

	void draw(Board& board) const override {
		for (int i = 0; i < rectWidth; ++i) {
			if (rectX + i < board.BOARD_WIDTH) {
				board.grid[rectY][rectX + i] = ColoredChar('*', color);
				board.grid[rectY + rectHeight - 1][rectX + i] = ColoredChar('*', color);
			}
		}
		for (int j = 0; j < rectHeight; ++j) {
			if (rectY + j < board.BOARD_HEIGHT) {
				board.grid[rectY + j][rectX] = ColoredChar('*', color);
				board.grid[rectY + j][rectX + rectWidth - 1] = ColoredChar('*', color);
			}
		}
	}
};

class Circle : public Figure {
public:
	int circX, circY, circRadius;
	string color;

	Circle(int coordinateX, int coordinateY, double radius, string Color) : circX(coordinateX), circY(coordinateY), circRadius(radius), color(Color) {}

	void draw(Board& board) const override {
		int x = 0;
		int y = circRadius;
		int p = 1 - circRadius; 

		while (x <= y) {
			if (circX + x < board.BOARD_WIDTH && circY + y < board.BOARD_HEIGHT) board.grid[circY + y][circX + x] = ColoredChar('*', color);
			if (circX - x >= 0 && circY + y < board.BOARD_HEIGHT) board.grid[circY + y][circX - x] = ColoredChar('*', color);
			if (circX + x < board.BOARD_WIDTH && circY - y >= 0) board.grid[circY - y][circX + x] = ColoredChar('*', color);
			if (circX - x >= 0 && circY - y >= 0) board.grid[circY - y][circX - x] = ColoredChar('*', color);
			if (circX + y < board.BOARD_WIDTH && circY + x < board.BOARD_HEIGHT) board.grid[circY + x][circX + y] = ColoredChar('*', color);
			if (circX - y >= 0 && circY + x < board.BOARD_HEIGHT) board.grid[circY + x][circX - y] = ColoredChar('*', color);
			if (circX + y < board.BOARD_WIDTH && circY - x >= 0) board.grid[circY - x][circX + y] = ColoredChar('*', color);
			if (circX - y >= 0 && circY - x >= 0) board.grid[circY - x][circX - y] = ColoredChar('*', color);

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
};

class System {
public:
	map<int, unique_ptr<Figure>> figures;
	void run(Board& board);
	bool isNumeric(const std::string& str);
private:
	void printCommands();
	void draw(Board& board); // 4 figures done
	void list(); // done
	void shapes(); // done
 // command "add" quite done, but not in separate function
	void undo(); // done
	void clear(Board& board); // done
	void save(); // done
	void load(); // done
	bool checkCircle(Board& board);
	bool checkSquare(Board& board);
	bool checkRectangle(Board& board);
	bool checkTriangle(Board& board);
	string input;
	string command;
	string figure;
	string strX, strY;
	string color;
	int x, y = -1;
	string triangleHeight, squareLength, rectangleWidth, rectangleHeight, circleRadius;
	string filePath;
	double doubleTrigHeight, doubleCircRadius, doubleRectWidth, doubleRectHeight, doubleSqrLength;
};

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
void System::shapes() {
	cout << "\nShapes:\n"
		<< "triangle coordinates height\n"
		<< "square coordinates sideLength\n"
		<< "rectangle coordinates width height\n"
		<< "circle coordinates radius\n";
}

void System::draw(Board& board) {
	for (const auto& pair : figures) {
		pair.second->draw(board);
	}
	board.print();
}

void System::list() {
	for (const auto& pair : figures) {
		int id = pair.first;
		const auto& figure = pair.second;

		if (auto* circle = dynamic_cast<Circle*>(figure.get())) {
			cout << id << " circle (" << circle->circX << ", " << circle->circY << ") radius " << circle->circRadius << " color " << circle->color << endl;
		}
		else if (auto* square = dynamic_cast<Square*>(figure.get())) {
			cout << id << " square (" << square->sqrX << ", " << square->sqrY << ") side length " << square->sqrSideLength << " color " << square->color << endl;
		}
		else if (auto* triangle = dynamic_cast<Triangle*>(figure.get())) {
			cout << id << " triangle (" << triangle->trigX << ", " << triangle->trigY << ") height " << triangle->trigHeight << " color " << triangle->color << endl;
		}
		else if (auto* rectangle = dynamic_cast<Rectangle*>(figure.get())) {
			cout << id << " rectangle (" << rectangle->rectX << ", " << rectangle->rectY << ") width " << rectangle->rectWidth << " height " << rectangle->rectHeight << " color " << rectangle->color << endl;
		}
	}
	cout << endl;
}

void System::undo() {
	int lastId = figures.size();
	figures.erase(lastId);
	cout << "The last added shape was removed from the blackboard\n\n";
}

void System::clear(Board& board) {
	figures.clear();
	for (int i = 0; i < board.BOARD_HEIGHT; ++i) {
		for (int j = 0; j < board.BOARD_WIDTH; ++j) {
			board.grid[i][j] = ColoredChar(' ', "white");
		}
	}
	cout << "The blackboard was cleared\n\n";
}

void System::save() {
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

void System::load() {
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

		forLoad >> id >> shape >> x >> y;

		if (shape == "circle") {
			int radius;
			forLoad >> radius >> color;
			figures[id] = make_unique<Circle>(x, y, radius, color);
		}
		else if (shape == "square") {
			int sideLength;
			forLoad >> sideLength >> color;
			figures[id] = make_unique<Square>(x, y, sideLength, color);
		}
		else if (shape == "triangle") {
			int height;
			forLoad >> height >> color;
			figures[id] = make_unique<Triangle>(x, y, height, color);
		}
		else if (shape == "rectangle") {
			int width, height;
			forLoad >> width >> height >> color;
			figures[id] = make_unique<Rectangle>(x, y, width, height, color);
		}
	}
	infile.close();
	cout << "Board was loaded from " << filePath << endl;
}

void System::run(Board& board) {
	while (true)
	{
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
			sss >> figure >> strX >> strY;
			if (isNumeric(strX) && isNumeric(strY)) {
				x = stoi(strX);
				y = stoi(strY);
			}
			if (x >= 0 && y >= 0) {
				if (figure == "triangle") {
					sss >> triangleHeight >> color;
					if (isNumeric(triangleHeight)) {
						doubleTrigHeight = stoi(triangleHeight);
						if (checkTriangle(board)==true) {
							figures[figures.size() + 1] = make_unique<Triangle>(x, y, doubleTrigHeight, color);
							cout << "\nThe figure was added\n\n";
						}
						else {
							cout << "\nThe figure is bigger than the board:(\n\n";
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
						if (checkSquare(board) == true) {
							figures[figures.size() + 1] = make_unique<Square>(x, y, doubleSqrLength, color);
							cout << "\nThe figure was added\n\n";
						}
						else {
							cout << "\nThe figure is bigger than the board:(\n\n";
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
						if (checkRectangle(board) == true)
							{
								figures[figures.size() + 1] = make_unique<Rectangle>(x, y, doubleRectWidth, doubleRectHeight, color);
								cout << "\nThe figure was added\n\n";
							}
						else {
							cout << "\nThe figure is bigger than the board:(\n\n";
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
						if (checkCircle(board) == true) {
							figures[figures.size() + 1] = make_unique<Circle>(x, y, doubleCircRadius, color);
							cout << "\nThe figure was added\n\n";
						}
						else {
							cout << "\nThe figure is bigger than the board:(\n\n";
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
		else if (command == "undo") {
			undo();
		}
		else if (command == "clear") {
			clear(board);
		}
		else if (command == "save"){
			sss >> filePath;
			save();
		}
		else if (command == "load") {
			sss >> filePath;
			load();
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
		<< "exit - to leave\n\n";
}

int main()
{
	Board board;
	System system;
	system.run(board);
}

