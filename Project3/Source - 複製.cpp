#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <string>
#include <cmath>
#include <cstdlib>
#include <algorithm>

using namespace std;

vector<string> Origin_Function; 
vector<string> Temp_Function; 
vector<string> Dont_Care; 
vector<string> Prime_Implicant; 
vector<string> Minterm; 

vector<char> ilb;
char ob;

/*
	EPI must be minterm.
	會修改到 Prime_Implicant, Origin_Function
	找到 EPI 的會丟到 Minterm
	並將此 EPI 覆蓋到的項從 Origin_Function 移除
	並將其從 Prime_Implicant 刪除
*/
void selectEPI()
{
	// 哪些Prime Implicant是 EPI
	set<int> EPIs;

	for (int i = 0; i < Origin_Function.size(); ++i)
	{
		// 紀錄哪個Prime Implicant為 EPI
		int EPI = -1;

		for (int j = 0; j < Prime_Implicant.size(); ++j)
		{
			bool termInPI = true;

			for (int k = 0; k < Prime_Implicant[j].size(); ++k)
			{
				if (Prime_Implicant[j][k] != '-' && Prime_Implicant[j][k] != Origin_Function[i][k])
				{
					termInPI = false;
					break;
				}
			}

			if (termInPI)
			{
				if (EPI != -1)
				{
					// 此項沒有只被一個Prime Implicant包住
					EPI = -1;
					break;
				}

				EPI = j;
			}
		}

		if (EPI != -1)
		{
			// Prime_Implicant[EPI] 為 EPI
			EPIs.insert(EPI);
		}
	}

	for (auto i = EPIs.rbegin(); i != EPIs.rend(); i++)
	{
		// 找到 EPI 的會丟到 Minterm
		Minterm.push_back(Prime_Implicant[*i]);

		// 並將此 EPI 覆蓋到的項從 Origin_Function 移除
		for (int j = 0; j < Origin_Function.size(); ++j)
		{
			bool termInEPI = true;

			for (int k = 0; k < Origin_Function[j].size(); ++k)
			{
				if (Prime_Implicant[*i][k] != '-' && Prime_Implicant[*i][k] != Origin_Function[j][k])
				{
					termInEPI = false;
					break;
				}
			}

			if (termInEPI)
			{
				Origin_Function.erase(Origin_Function.begin() + j);
				--j;
			}
		}

		// 並將其從 Prime_Implicant 刪除
		Prime_Implicant.erase(Prime_Implicant.begin() + *i);
	}
}

/*
	Implement https://en.wikipedia.org/wiki/Petrick%27s_method
	要涵蓋的是 Origin_Function，從 Prime_Implicant 找需要的推進 Minterm
*/
void petrick()
{
	vector<vector<set<int>>> T;
	bool notSimplest = true;
	int min;

	// Initial T
	for (int i = 0; i < Origin_Function.size(); ++i)
	{
		T.push_back(vector<set<int>>());

		for (int j = 0; j < Prime_Implicant.size(); ++j)
		{
			bool termInPI = true;
			for (int k = 0; k < Prime_Implicant[j].length(); ++k)
			{
				if (Prime_Implicant[j][k] != '-' && Prime_Implicant[j][k] != Origin_Function[i][k])
				{
					termInPI = false;
					break;
				}
			}

			if (termInPI)
			{
				T[T.size() - 1].push_back(set<int>());
				T[T.size() - 1][T[T.size() - 1].size() - 1].insert(j);
			}
		}
	}

	/*
	Simplify T with the following equivalences until T cannot be simplified:
		X+XY = X
		XX = X (By using set, this situation can be prevented)
	*/
	while (notSimplest)
	{
		// X + XY = X
		for (int i = 0; i < T.size(); ++i)
		{
			for (int j0 = 0; j0 < T[i].size(); ++j0)
			{
				for (int j1 = j0 + 1; j1 < T[i].size(); ++j1)
				{
					if (T[i][j0].size() > T[i][j1].size())
					{
						auto temp = T[i][j0];
						T[i][j0] = T[i][j1];
						T[i][j1] = temp;
					}

					if (includes(T[i][j1].begin(), T[i][j1].end(), T[i][j0].begin(), T[i][j0].end()))
					{
						// Match equivalences:
						// T[i][j0] + T[i][j0] * T[i][j1] = T[i][j0]
						// Delete T[i][j1]
						T[i].erase(T[i].begin() + j1);
						--j1;
					}
				}
			}
		}

		// distributive law, preventing situation that making XX e.g. (X + ...)(X + ...) = XX + ...
		if (T.size() > 1)
		{
			// T[i - 1] = T[i - 1] * T[i]
			for (int i = 1; i < T.size(); i += 2)
			{
				vector<set<int>> temp;
				for (int j0 = 0; j0 < T[i - 1].size(); ++j0)
				{
					for (int j1 = 0; j1 < T[i].size(); ++j1)
					{
						set<int> tempTerm = T[i - 1][j0];

						tempTerm.insert(T[i][j1].begin(), T[i][j1].end());

						temp.push_back(tempTerm);
					}
				}

				T[i - 1] = temp;
			}

			// Delete T[i]
			for (int i = 1; i < T.size(); ++i)
			{
				T.erase(T.begin() + i);
			}
		}
		else
		{
			notSimplest = false;
		}
	}

	/*
		挑項數最少的Term
	*/
	min = 0;
	for (int i = 0; i < T[0].size(); ++i)
	{
		if (T[0][min].size() > T[0][i].size())
		{
			min = i;
		}
	}

	/*
		將此Term存到Minterm
	*/
	for (int const& i : T[0][min])
	{
		Minterm.push_back(Prime_Implicant[i]);
	}
}

/*
	Check if the difference between string a & b is just a bit.
	If true, return the position of the bit,
	else return -1.
*/
int isGrayCode(string a, string b)
{
	int diff = -1;

	if (a.size() != b.size())
	{
		// Error
		return -1;
	}

	for (int i = 0; i < a.size(); ++i)
	{
		if (a[i] != b[i])
		{
			if (diff != -1)
			{
				return -1;
			}
			diff = i;
		}
	}

	return diff;
}

/*
	expand terms into ints
*/
vector<int> stringToInts(string a)
{
	vector<int> posStack;
	vector<int> result;

	string temp = a;
	int pos = 0;

	// Expand string
	while (posStack.size() != 0 || pos < a.length())
	{
		if (a[pos] == '-')
		{
			if (temp[pos] == '-' || temp[pos] == '1')
			{
				temp[pos] = '0';
				posStack.push_back(pos);
			}
			else
			{
				temp[pos] = '1';
			}
		}

		++pos;

		if (pos == a.length())
		{
			int num = 0;
			for (int i = 0; i < temp.length(); ++i)
			{
				num <<= 1;
				if (temp[i] == '1')
				{
					num += 1;
				}
			}
			result.push_back(num);

			if (posStack.size() > 0)
			{
				pos = posStack[posStack.size() - 1];
				posStack.pop_back();
			}
		}
	}

	return result;
}

/*
	挑最小項
*/
void selectMinterms()
{
	vector<string> Temp = Origin_Function;

	// 先選 EPI
	selectEPI();

	// 選完還是有沒選到的 再用 Petrick's method
	if (Origin_Function.size() > 0)
	{
		petrick();
	}
	Origin_Function = Temp;
}

/*
	Push element into F after checking there isn't a already existed element in F.
*/
void pushWithoutRepeated(vector<string>& F, string element)
{
	if (std::find(F.begin(), F.end(), element) == F.end())
	{
		F.push_back(element);
	}
}

vector<string> reduce(vector<string> F)
{
	bool* checked = new bool[F.size()];
	vector<string> newF;
	vector<vector<int>> groups; // group end location

	// Initial checked
	for (int i = 0; i < F.size(); ++i)
	{
		checked[i] = false;
	}

	// Initial groups
	for (int i = 0; i <= ilb.size(); ++i)
	{
		groups.push_back(vector<int>());
	}

	for (int i = 0; i < F.size(); ++i)
	{
		int count = 0;
		for (int j = 0; j < F[i].length(); ++j)
		{
			if (F[i][j] == '1')
			{
				++count;
			}
		}

		groups[count].push_back(i);
	}

	// Find pair between groups
	for (int g = 0; g < groups.size() - 1; ++g)
	{
		for (int i = 0; i < groups[g].size(); ++i)
		{
			for (int j = 0; j < groups[g + 1].size(); ++j)
			{
				int wdBit;
				if ((wdBit = isGrayCode(F[groups[g][i]], F[groups[g + 1][j]])) != -1)
				{
					// Replace with wildcard, pushing into newF after checking there is not a repeated element in newF, setting checked and flag
					checked[groups[g][i]] = true;
					checked[groups[g + 1][j]] = true;

					string temp = F[groups[g][i]];
					temp[wdBit] = '-';

					pushWithoutRepeated(newF, temp);
				}
			}
		}
	}

	// Push minterm
	for (int i = 0; i < F.size(); ++i)
	{
		if (!checked[i])
		{
			pushWithoutRepeated(Prime_Implicant, F[i]);
		}
	}

	delete[] checked;

	return newF;
}

/*
	transfer i to string of length digit
*/
string intToString(int i, int digit)
{
	string result = "", temp = "";

	while (i)
	{
		if (i % 2)
		{
			temp += '1';
		}
		else
		{
			temp += '0';
		}

		i /= 2;
	}

	for (int i = 0; i < (digit - temp.length()); ++i)
	{
		result += '0';
	}

	for (int i = temp.length() - 1; i >= 0; --i)
	{
		result += temp[i];
	}

	return result;
}

/*
	Get user input from file, 展開 Boolean Funciton 後 storing into Origin_Function, Dont_Care.
	Origin_Function 跟 Dont_Care 也都存到 Temp_Function
*/
void input(char* filename)
{
	ifstream input(filename);
	string line;
	int libNum;
	int max = -1;
	int pNum;

	// getline first
	while (getline(input, line))
	{
		stringstream ss(line);
		string buf;

		ss >> buf;

		if (buf == ".i")
		{
			ss >> libNum;
			max = pow(2, libNum);
		}
		else if (buf == ".o")
		{
			continue;
		}
		else if (buf == ".ilb")
		{
			char temp;
			for (int i = 0; i < libNum; ++i)
			{
				ss >> temp;
				ilb.push_back(temp);
			}
		}
		else if (buf == ".ob")
		{
			ss >> ob;
		}
		else if (buf == ".p")
		{

			bool* Origin_Function_terms = new bool[max];
			bool* Dont_Care_terms = new bool[max];

			for (int i = 0; i < max; ++i)
			{
				Origin_Function_terms[i] = false;
				Dont_Care_terms[i] = false;
			}

			ss >> pNum;

			for (int i = 0; i < pNum; ++i)
			{
				string lineP, // e.g. 01-0 -
					   bfunc; // e.g. 01-0
				char value;   // e.g. -
				vector<int> table;

				getline(input, lineP);

				stringstream tempss(lineP);

				tempss >> bfunc >> value;

				table = stringToInts(bfunc);

				for (int i = 0; i < table.size(); ++i)
				{
					if (value == '1')
					{
						Origin_Function_terms[table[i]] = true;
					}
					else if (value == '-')
					{
						Dont_Care_terms[table[i]] = true;
					}
				}
			}

			// Set false to i-th item of Dterms when i-th item of OFterms is true
			for (int i = 0; i < max; ++i)
			{
				if (Origin_Function_terms[i])
				{
					Dont_Care_terms[i] = false;
				}
			}

			// Store into G_OF, G_F, G_D
			for (int i = 0; i < max; ++i)
			{
				if (Origin_Function_terms[i])
				{
					Origin_Function.push_back(intToString(i, libNum));
					Temp_Function.push_back(intToString(i, libNum));
				}
				else if (Dont_Care_terms[i])
				{
					Dont_Care.push_back(intToString(i, libNum));
					Temp_Function.push_back(intToString(i, libNum));
				}
			}

			delete[] Origin_Function_terms;
			delete[] Dont_Care_terms;
		}
		else if (buf == ".e")
		{
			continue;
		}
	}

	input.close();
}

/*
	Output data that project required into file.
*/
void output(char* filename)
{
	ofstream output(filename);
	int lit = 0;

	output << ".i " << ilb.size() << endl;
	output << ".o 1" << endl;
	output << ".ilb";

	for (int i = 0; i < ilb.size(); ++i)
	{
		output << ' ' << ilb[i];
	}
	output << endl;

	output << ".ob " << ob << endl;
	output << ".p " << Minterm.size() << endl;

	for (int i = 0; i < Minterm.size(); ++i)
	{
		output << Minterm[i] << " 1" << endl;
		for (int j = 0; j < Minterm[i].size(); ++j)
		{
			if (Minterm[i][j] != '-')
			{
				++lit;
			}
		}
	}

	output << ".e";

	output.close();

	cout << "Total number of terms: " << Minterm.size() << endl;
	cout << "Total number of literals: " << lit << endl;
}

int main(int argc, char* argv[])
{
	// Input
	input(argv[1]);

	// 算 PI
	do {
		Temp_Function = reduce(Temp_Function);
	} while (Temp_Function.size() > 0);

	// 選擇哪些Terms
	selectMinterms();

	// Output
	output(argv[2]);
}
