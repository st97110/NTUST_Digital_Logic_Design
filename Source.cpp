#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <string>
#include <sstream>
#include <cmath>
#include <map>

using namespace std;

class mini
{
public:
	map<string, bool> F; // 不包含dont care項
	map<string, char> Temp; // 包含dont care項

	int PLA_i, PLA_o;
	vector<string> names;
	string PLA_ob;

	void input(string inputFileName)
	{
		fstream inputFile(inputFileName);

		string command;
		while (inputFile >> command)
		{
			if (command == ".i")
			{
				inputFile >> PLA_i;
			}
			else if (command == ".o")
			{
				inputFile >> PLA_o;
			}
			else if (command == ".ilb")
			{
				string name;
				for (int i = 0; i < PLA_i; i++)
				{
					inputFile >> name;
					names.push_back(name);
				}
			}
			else if (command == ".ob")
			{
				inputFile >> PLA_ob;
			}
			else if (command == ".p")
			{
				int num;
				inputFile >> num;
				string key;
				char value;
				for (int i = 0; i < num; i++)
				{
					inputFile >> key >> value;
					Temp[key] = value;
				}
			}
			else if (command == ".e")
			{
				break;
			}
		}
	}

	void 
};



int main(int argc, char* argv[])
{
	mini mini1;

	mini1.input(argv[1]);
}