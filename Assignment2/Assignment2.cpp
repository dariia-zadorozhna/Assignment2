#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
using namespace std;

class System {
public:
	/*map<int, Figure> figures;*/
	void run();
	bool isNumeric(const std::string& str);
private:
	void printCommands();
	void draw();
	void list();
	void shapes();
	void add();
	void undo();
	void clear();
	void save();
	void load();
	string input;
	string command;
	string figure;
	string strX, strY;
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
	cout << "Shapes:\n"
		<< "triangle coordinates height\n"
		<< "square coordinates sideLength\n"
		<< "rectangle coordinates width height\n"
		<< "circle coordinates radius\n";
}

void System::run() {
	while (true)
	{
		printCommands();
		getline(cin, input);
		stringstream sss(input);
		sss >> command;
		if (command == "draw")
		{
			draw();
		}
		else if (command == "list")
		{
			list();
		}
		else if (command == "shapes")
		{
			shapes();
		}
		else if (command == "add")
		{
			sss >> figure >> strX >> strY;
			if (isNumeric(strX) && isNumeric(strY)) {
				int x = stoi(strX);
				int y = stoi(strY);
			}
			if (figure == "triangle") {
				sss >> triangleHeight;
				// create triangle object and add it to the map with figures
			}
			else if (figure == "square") {
				sss >> squareLength;
				// create square object and add it to the map with figures
			}
			else if (figure == "rectangle") {
				sss >> rectangleWidth >> rectangleHeight;
				// create rectangle object and add it to the map with figures
			}
			else if (figure == "circle") {
				sss >> circleRadius;
				// create circle object and add it to the map with figures
			}
		}
		else if (command == "undo") {
			undo();
		}
		else if (command == "clear") {
			clear();
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
    std::cout << "Hello World!\n";
}

