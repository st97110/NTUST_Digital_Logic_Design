#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <map>

using namespace std;

struct Result
{
	map<string, bool> data;
};

void show(Result result[7][7], int in, int input)
{
	cout << endl;
	for (int i = 0; i <= input; i++)
	{
		cout << i << endl;
		for (auto j = result[i][in].data.begin(); j != result[i][in].data.end(); j++)
		{
			cout << (*j).first << '\t' << (*j).second << endl;
		}
	}
}

int one(string num)
{
	int n = 0;
	for (int i = 0; i < num.length(); i++)
	{
		if (num[i] == '1')
		{
			n++;
		}
	}
	return n;
}

string next(string& num)
{
	for (int i = num.length() - 1; i >= 0; i--)
	{
		if (num[i] == '0')
		{
			num[i] = '1';
			return num;
		}
		else
		{
			num[i] = '0';
		}
	}
	return num;
}

bool tf(string num, string dcare)
{
	for (int i = 0; i < num.length(); i++)
	{
		if (num[i] != dcare[i] && dcare[i] != '-')
			return false;
	}
	return true;
}

void test0(Result result[7][7], map<string, char> data, string tnum[64], int input, int max, int& tn)
{
	string num = "0000";
	if (input == 5)
		num = "00000";
	else if (input == 6)
		num = "000000";
	for (int i = 0; i < max; i++)
	{
		for (auto j = data.begin(); j != data.end(); j++)
		{
			if (tf(num, (*j).first) && ((*j).second == '-' || (*j).second == '1'))
				result[one(num)][0].data[num] = false;
		}
		next(num);
	}
	for (int i = 0; i <= input; i++)
	{
		for (auto j = result[i][0].data.begin(); j != result[i][0].data.end(); j++)
		{
			if (data[(*j).first] != '-')
			{
				tnum[tn] = (*j).first;
				tn++;
			}
		}
	}
}

void test1(Result result[7][7], int input)
{
	for (int i = 0; i < input; i++)
	{
		for (auto j = result[i][0].data.begin(); j != result[i][0].data.end(); j++)
		{
			for (auto k = result[i + 1][0].data.begin(); k != result[i + 1][0].data.end(); k++)
			{
				string buf = "";
				int dc = 0;
				for (int l = 0; l < input; l++)
				{
					if ((*j).first[l] == (*k).first[l])
						buf += (*j).first[l];
					else
					{
						buf += '-';
						dc++;
					}
					if (dc > 1)
						break;
				}
				if (dc < 2)
				{
					result[i][1].data[buf] = false;
					(*j).second = true;
					(*k).second = true;
				}
			}
		}
	}
}

void test(Result result[7][7], int input)
{
	for (int i = 1; i < input; i++)
	{
		for (int j = 0; j < input; j++)
		{
			for (auto k = result[j][i].data.begin(); k != result[j][i].data.end(); k++)
			{
				for (auto l = result[j + 1][i].data.begin(); l != result[j + 1][i].data.end(); l++)
				{
					bool same = true;
					for (int m = 0; m < (*k).first.length(); m++)
					{
						if ((*k).first[m] == '-' && (*l).first[m] != '-')
						{
							same = false;
							break;
						}
					}
					if (same)
					{
						string buf = "";
						int dc = 0;
						for (int m = 0; m < (*k).first.length(); m++)
						{
							if ((*k).first[m] != (*l).first[m])
							{
								buf += '-';
								dc++;
							}
							else
								buf += (*k).first[m];
						}
						if (dc < 2)
						{
							result[j][i + 1].data[buf] = false;
							(*k).second = true;
							(*l).second = true;
						}
					}
				}
			}
		}
	}
}

void pl(Result result[7][7], map<string, bool[64]>& PL, string tnum[64], int input, int tn)
{
	for (int i = 0; i <= input; i++)
	{
		for (int j = 0; j <= input; j++)
		{
			for (auto k = result[j][i].data.begin(); k != result[j][i].data.end(); k++)
			{
				if (!(*k).second)
				{
					for (int l = 0; l < tn; l++)
					{
						if (tf(tnum[l], (*k).first))
							PL[(*k).first][l] = true;
						else
							PL[(*k).first][l] = false;
					}
				}
			}
		}
	}
}

void showpl(map<string, bool[64]> PL, int tn)
{
	cout << endl;
	for (auto i = PL.begin(); i != PL.end(); i++)
	{
		cout << (*i).first << '\t';
		for (int j = 0; j < tn; j++)
		{
			cout << (*i).second[j] << "   ";
		}
		cout << endl;
	}
}

void search(map<string, bool[64]> PL, map<string, bool> essen, string one[1000], int unneed[64], int tn, int unn, int i, int& onen)
{
	bool need = true;
	onen = 0;
	for (int j = 0; j < unn; j++)
	{
		if (i == unneed[j])
		{
			need = false;
			break;
		}
	}
	if (need)
	{
		for (auto j = PL.begin(); j != PL.end(); j++)
		{
			need = true;
			for (auto k = essen.begin(); k != essen.end(); k++)
			{
				if ((*j).first == (*k).first)
				{
					need = false;
					break;
				}
			}
			if (need && (*j).second[i])
			{
				one[onen] = (*j).first;
				onen++;
			}
		}
	}
}

void mix(string one[180], string two[64], string all[180], int onen, int twon, int& alln)
{
	alln = 0;
	for (int i = 0; i < onen; i++)
	{
		for (int j = 0; j < twon; j++)
		{
			if (one[i].find(two[j]) == one[i].npos)
			{
				all[alln] = one[i] + " " + two[j];
				alln++;
			}
			else
			{
				all[alln] = one[i];
				alln++;
			}
		}
	}
}

void sim(map<string, bool[64]> PL, map<string, bool>& essen, int tn)
{
	string buf;
	string one[180], two[64], all[180];
	int test = 0, unn = 0, onen = 0, twon = 0, alln = 0, min = 0, minl = 0;
	int unneed[64] = { 0 };
	for (int i = 0; i < tn; i++)
	{
		test = 0;
		for (auto j = PL.begin(); j != PL.end(); j++)
		{
			if ((*j).second[i])
			{
				buf = (*j).first;
				test++;
			}
		}
		if (test == 1)
			essen[buf] = true;
	}
	for (auto i = essen.begin(); i != essen.end(); i++)
	{
		for (int j = 0; j < tn; j++)
		{
			if (PL[(*i).first][j])
			{
				unneed[unn] = j;
				unn++;
			}
		}
	}
	int count = 0;
	do
	{
		search(PL, essen, one, unneed, tn, unn, count, onen);
		count++;
	} while (onen == 0 && count < tn);
	while (count < tn)
	{
		do
		{
			search(PL, essen, two, unneed, tn, unn, count, twon);
			count++;
		} while (twon == 0 && count < tn);
		if (count == tn)
			break;
		mix(one, two, all, onen, twon, alln);
		for (int i = 0; i < alln; i++)
		{
			one[i] = all[i];
		}
		onen = alln;
	}
	minl = all[0].length();
	min = 0;
	for (int i = 1; i < alln; i++)
	{
		if (all[i].length() < minl)
		{
			minl = all[i].length();
			min = i;
		}
	}
	for (int i = 0; i < all[min].length();)
	{
		buf = "";
		while (all[min][i] == ' ' && i < all[min].length())
			i++;
		while (all[min][i] != ' ' && i < all[min].length())
		{
			buf += all[min][i];
			i++;
		}
		essen[buf] = true;
	}
}

int main()
{
	int input = 0, p = 0, max = 0, tn = 0;
	fstream inf, outf;
	Result result[7][7];
	string num = "", name = "", com = "", in = "";
	string tnum[64];
	map<string, bool> essen;
	map<string, char> data;
	map<string, bool[64]> PL;

	cout << "File name : ";
	cin >> name;
	inf.open(name, ios::in);
	while (!inf)
	{
		cout << "File in ERROR\nFile name : ";
		cin >> name;
		inf.open(name, ios::in);
	}
	cout << "Save as : ";
	cin >> name;
	outf.open(name, ios::out);
	while (inf >> com)
	{
		if (com == ".i")
		{
			inf >> input;
			outf << com << " " << input << "\n";
		}
		else if (com == ".o")
		{
			inf >> in;
			outf << com << " " << in << "\n";
		}
		else if (com == ".ilb")
		{
			outf << com;
			for (int i = 0; i < input; i++)
			{
				inf >> in;
				outf << " " << in;
			}
			outf << "\n";
		}
		else if (com == ".ob")
		{
			inf >> in;
			outf << com << " " << in << "\n";
		}
		else if (com[0] == '0' || com[0] == '-' || com[0] == '1')
		{
			inf >> data[com];
		}
		else if (com == ".e")
		{
			break;
		}
		else
		{
			string tra;
			inf >> tra;
		}
	}
	max = pow(2, input);
	test0(result, data, tnum, input, max, tn);
	test1(result, input);
	test(result, input);
	pl(result, PL, tnum, input, tn);
	showpl(PL, tn);
	sim(PL, essen, tn);
	outf << ".p " << essen.size() << "\n";
	for (auto i = essen.begin(); i != essen.end(); i++)
		outf << (*i).first << " 1\n";
	outf << ".e\n";
	cout << "\nComplete!\n";
	return 0;
}
