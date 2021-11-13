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

	map<string, bool> Minterm;
	map<string, bool> PrimeImplicant;
	map<string, bool> Origin;
	map<string, bool> Temp; // 包含dont care項
	map<string, char> Input;
	vector<int> Origin_num; // 數字紀錄1 2 10 13項


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
					Input[key] = value;
				}
			}
			else if (command == ".e")
			{
				break;
			}
		}
	}

	void change() // --11 轉成最小項 0011 0111 1011 1111
	{
		for (auto it = Input.begin(); it != Input.end(); it++)
		{
			for (int j = 0; j < pow(2, PLA_i); j++)
			{
				bool flag = true;
				string key = intToString(j);
				for (int k = 0; k < PLA_i; k++)
				{
					if (it->first[k] != '-' && key[k] != it->first[k]) // -111 != 0110
					{
						flag = false;
						break;
					}
				}
				//if (flag)
				//{
				//	Temp[key] = true; // dont care項也算進去
				//	Origin[key] = true;
				//	Origin_num.push_back(j);
				//}

				if (flag && it->second != '-')
				{
					Temp[key] = true;
				}
				else if (flag && it->second == '-')
				{
					Origin[key] = true;
				}
			}
		}
	}

	map<string, bool> reduce(map<string, bool> temps)
	{
		auto tempsEnd1 = --temps.end();
		auto tempsBegin1 = ++temps.begin();
		map<string, bool> checked;

		map<string, bool> newTemp;
		for (auto i = temps.begin(); i != tempsEnd1; i++)
		{
			checked[i->first] = false;
		}

		for (auto i = temps.begin(); i != tempsEnd1; i++)
		{
			for (auto j = tempsBegin1; j != temps.end(); j++)
			{
				int bit = IsGrayCode(i->first, j->first);
				if (bit != -1)
				{
					checked[i->first] = true;
					checked[j->first] = true;

					string insert = i->first;
					insert[bit] = '-';

					newTemp[insert] = true;
				}
			}
		}

		for (auto i = temps.begin(); i != temps.end(); i++)
		{
			if (!checked[i->first])
			{
				PrimeImplicant[i->first] = true;
			}
		}
		return newTemp;
	}

	void minterm()
	{
		EPI();
	}

	string intToString(int num) // 3 to 0011 
	{
		string s;
		if (PLA_i == 4)
		{
			s = "0000";
		}
		else if (PLA_i == 5)
		{
			s = "00000";
		}
		else if (PLA_i == 6)
		{
			s = "000000";
		}
		int n = pow(2, PLA_i - 1);
		for (int i = 0; i < PLA_i; i++)
		{
			if (n <= num)
			{
				s[i] = '1';
				num -= n;
			}
			else
			{
				s[i] = '0';
			}
			
			n /= 2;
		}
		return s;
	}

	int IsGrayCode(string a, string b)
	{
		if (a.length() != b.length())
		{
			return -1;
		}
		int bit = -1;
		for (int i = 0; i < a.length(); i++)
		{
			if (a[i] != b[i])
			{
				if (bit != -1)
				{
					return -1;
				}
				bit = i;
			}
		}
		return bit;
	}

	void EPI()
	{
		set<int> EPIs;

		for (auto O_it = Origin.begin(); O_it != Origin.end(); O_it++)
		{
			int EPI_num = -1;
			for (map<string, bool>::iterator PI_it = PrimeImplicant.begin(), int i = 0; PI_it != PrimeImplicant.end(); PI_it++, i++)
			{
				bool IsEPI = true;
				for (int j = 0; j < PI_it->first.size(); j++)
				{
					if (PI_it->first[j] != '-' && O_it->first[j] != PI_it->first[j])
					{
						IsEPI = false;
						break;
					}
				}
				if (IsEPI)
				{
					if (EPI_num != -1)
					{
						EPI_num = -1;
						break;
					}
					EPI_num = i;
				}
			}
			if (EPI_num != -1)
			{
				EPIs.insert(EPI_num);
			}
		}
		for (set<int>::reverse_iterator i = EPIs.rbegin(), map<string, bool>::iterator PI_it = PrimeImplicant.begin(); i != EPIs.rend(); i++)
		{
			Minterm[PrimeImplicant[*i]]
		}
	}
};



int main(int argc, char* argv[])
{
	mini mini1;
	mini1.PLA_i = 4;

	/*mini1.Temp["11-1"] = '-';
	mini1.Temp["---0"] = '1';
	mini1.change();*/
	/*string test;
	test = mini1.intToString(0);
	cout << test << endl;*/

	mini1.input(argv[1]);
	mini1.change();

	do
	{
		mini1.Temp = mini1.reduce(mini1.Temp);
	} while (mini1.Temp.size() >= 1);
	
	mini1.minterm();
}
