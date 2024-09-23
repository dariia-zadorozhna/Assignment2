#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
using namespace std;

class Board {
public:
	const int BOARD_WIDTH = 60;
	const int BOARD_HEIGHT = 20;

	vector<vector<char>> grid;

	Board() : grid(BOARD_HEIGHT, vector<char>(BOARD_WIDTH, ' ')) {}
	void print() {
		for (auto& row : grid) {
			for (char c : row) {
				std::cout << c;
			}
			std::cout << "\n";
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

	string trigName;

	Triangle(int coordinateX, int coordinateY, int Height) : trigX(coordinateX), trigY(coordinateY), trigHeight(Height), trigName("triangle") {}

	void draw(Board& board) const override {
		if (trigHeight <= 0) return;
		for (int i = 0; i < trigHeight; ++i) {
			int leftMost = trigX - i; 
			int rightMost = trigX + i; 
			int posY = trigY + i; 
			if (posY < board.BOARD_HEIGHT) {
				if (leftMost >= 0 && leftMost < board.BOARD_WIDTH)
				{
					board.grid[posY][leftMost] = '*';
				}
				if (rightMost >= 0 && rightMost < board.BOARD_WIDTH && leftMost != rightMost)
				{
					board.grid[posY][rightMost] = '*';
				}
			}
		}
		for (int j = 0; j < 2 * trigHeight - 1; ++j) {
			int baseX = trigX - trigHeight + 1 + j;
			int baseY = trigY + trigHeight - 1;
			if (baseX >= 0 && baseX < board.BOARD_WIDTH && baseY < board.BOARD_HEIGHT)
				board.grid[baseY][baseX] = '*';
		}
	}
};

class Square : public Figure {
public:
	int sqrX, sqrY, sqrSideLength;

	string sqrName;

	Square(int coordinateX, int coordinateY, int sideLength) : sqrX(coordinateX), sqrY(coordinateY), sqrSideLength(sideLength), sqrName("square") {}

	void draw(Board& board) const override {
		for (int i = 0; i < sqrSideLength; ++i) {
			if (sqrX + i < board.BOARD_WIDTH) {
				board.grid[sqrY][sqrX + i] = '*';
				board.grid[sqrY + sqrSideLength - 1][sqrX + i] = '*';
			}
			if (sqrY + i < board.BOARD_HEIGHT) {
				board.grid[sqrY + i][sqrX] = '*'; 
				board.grid[sqrY + i][sqrX + sqrSideLength - 1] = '*'; 
			}
		}
	}
};

class Rectangle : public Figure {
public:
	int rectX, rectY, rectWidth, rectHeight;

	string rectName;

	Rectangle(int coordinateX, int coordinateY, int width, int height) : rectX(coordinateX), rectY(coordinateY), rectWidth(width), rectHeight(height), rectName("rectangle") {}

	void draw(Board& board) const override {
		for (int i = 0; i < rectWidth; ++i) {
			if (rectX + i < board.BOARD_WIDTH) {
				board.grid[rectY][rectX + i] = '*';
				board.grid[rectY + rectHeight - 1][rectX + i] = '*';
			}
		}
		for (int j = 0; j < rectHeight; ++j) {
			if (rectY + j < board.BOARD_HEIGHT) {
				board.grid[rectY + j][rectX] = '*';
				board.grid[rectY + j][rectX + rectWidth - 1] = '*';
			}
		}
	}
};

class Circle : public Figure {
public:
	int circX, circY, circRadius;

	string circName;

	Circle(int coordinateX, int coordinateY, int radius) : circX(coordinateX), circY(coordinateY), circRadius(radius), circName("circle") {}

	void draw(Board& board) const override {
		cout << "Drawing a circle\n";
	}
};

class System {
public:
	map<int, unique_ptr<Figure>> figures;
	void run(Board& board);
	bool isNumeric(const std::string& str);
private:
	void printCommands();
	void draw(Board& board); // 3 out of 4 figures done
	void list(); // done
	void shapes(); // done
 // command "add" quite done, but not in separate function
	void undo(); // done
	void clear(Board& board); // done
	void save();
	void load();
	string input;
	string command;
	string figure;
	string strX, strY;
	int x, y = -1;
	string triangleHeight, squareLength, rectangleWidth, rectangleHeight, circleRadius;
	string filePath;
};

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
			cout << id << " circle (" << circle->circX << ", " << circle->circY << ") radius " << circle->circRadius << endl;
		}
		else if (auto* square = dynamic_cast<Square*>(figure.get())) {
			cout << id << " square (" << square->sqrX << ", " << square->sqrY << ") side length " << square->sqrSideLength << endl;
		}
		else if (auto* triangle = dynamic_cast<Triangle*>(figure.get())) {
			cout << id << " triangle (" << triangle->trigX << ", " << triangle->trigY << ") height " << triangle->trigHeight << endl;
		}
		else if (auto* rectangle = dynamic_cast<Rectangle*>(figure.get())) {
			cout << id << " rectangle (" << rectangle->rectX << ", " << rectangle->rectY << ") width " << rectangle->rectWidth << " height " << rectangle->rectHeight << endl;
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
			board.grid[i][j] = ' '; 
		}
	}
	cout << "The blackboard was cleared\n\n";
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
					sss >> triangleHeight;
					if (isNumeric(triangleHeight)) {
						int intTrigHeight = stoi(triangleHeight);
						figures[figures.size() + 1] = make_unique<Triangle>(x, y, intTrigHeight);
					}
				}
				else if (figure == "square") {
					sss >> squareLength;
					if (isNumeric(squareLength)) {
						int intSqrLength = stoi(squareLength);
						figures[figures.size() + 1] = make_unique<Square>(x, y, intSqrLength);
					}
				}
				else if (figure == "rectangle") {
					sss >> rectangleWidth >> rectangleHeight;
					if (isNumeric(rectangleWidth) && isNumeric(rectangleHeight)) {
						int intRectWidth = stoi(rectangleWidth);
						int intRectHeight = stoi(rectangleHeight);
						figures[figures.size() + 1] = make_unique<Rectangle>(x, y, intRectWidth, intRectHeight);
					}
				}
				else if (figure == "circle") {
					sss >> circleRadius;
					if (isNumeric(circleRadius)) {
						int intCircRadius = stoi(circleRadius);
						figures[figures.size() + 1] = make_unique<Circle>(x, y, intCircRadius);
					}
				}
			}
			cout << "\nThe figure was added\n\n";
		}
		else if (command == "undo") {
			undo();
		}
		else if (command == "clear") {
			clear(board);
		}
		else if (command == "save"){
			sss >> filePath;
			//save();
		}
		else if (command == "load") {
			sss >> filePath;
			//load();
		}
		else if (command == "exit") {
			return;
		}
		else {
			cout << "The input is wrong!\n";
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

