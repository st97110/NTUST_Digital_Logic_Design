#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
using namespace std;

class Node
{
public:
	int else_Edge, then_Edge;
	string value = "";
	bool redundent = false;

	bool operator==(Node& i)
	{
		return (this->value == i.value) && (this->else_Edge == i.else_Edge) && (this->then_Edge == i.then_Edge);
	}
	bool IsEdgeSame()
	{
		return this->else_Edge == this->then_Edge;
	}
};

class ROBDD
{
public:
	ROBDD() {}
	~ROBDD() { delete[] btree; }
	
	void CreateTree()
	{
		size = (1 << i_count) + 1;
		btree = new Node[size];
		btree[0].value = "0";
		btree[size - 1].value = "1";
	}
	int LayerStart(int L)
	{
		return (1 << L);
	}
	int LayerEnd(int L)
	{
		return (1 << (L + 1));
	}
	void CreateLayer(string value, int layer_index)
	{
		for (int i = LayerStart(layer_index); i < LayerEnd(layer_index); i++)
		{
			btree[i].value = value;
			if (layer_index == i_count - 1) continue;
			btree[i].else_Edge = i * 2;
			btree[i].then_Edge = i * 2 + 1;
		}
	}
	void output(ofstream& bdd, ofstream& robdd)
	{
		for (int i = LayerStart(i_count - 1), j = 0; i < LayerEnd(i_count - 1); i++, j += 2)
		{
			btree[i].else_Edge = IsBooleanTrue(j) ? size - 1 : 0;
			btree[i].then_Edge = IsBooleanTrue(j + 1) ? size - 1 : 0;
		}

		bdd << "digraph OBDD" << endl << "{" << endl;
		for (int i = 0; i < i_count; i++)
		{
			bdd << "\t{rank=same";
			for (int j = LayerStart(i); j < LayerEnd(i); j++)
			{
				if (btree[j].redundent)
				{
					continue;
				}
				else
				{
					bdd << " " << j;
				}
			}
			bdd << "}" << endl;
		}
		bdd << endl;
		bdd << "\t0 [label=\"0\", shape = box]" << endl;
		for (int i = 1; i < size - 1; i++)
		{
			if (btree[i].redundent) continue;
			bdd << "\t" << i << " [label=\"" << btree[i].value << "\"]" << endl;
		}
		bdd << "\t" << size - 1 << " [label = \"1\", shape = box]" << endl;

		bdd << endl;
		for (int i = 1; i < size - 1; i++)
		{
			if (btree[i].redundent) continue;
			bdd << "\t" << i << " -> " << btree[i].else_Edge << " [label=\"0\", style=dotted]" << endl;
			bdd << "\t" << i << " -> " << btree[i].then_Edge << " [label=\"1\", style=solid]" << endl;
		}
		bdd << "}" << endl;
		ReducedOrder();
		// output to file
		robdd << "digraph ROBDD" << endl << "{" << endl;
		for (int i = 0; i < i_count; i++)
		{
			robdd << "\t{rank=same";
			for (int j = LayerStart(i); j < LayerEnd(i); j++)
			{
				if (btree[j].redundent)
				{
					continue;
				}
				else
				{
					robdd << " " << j;
				}
			}
			robdd << "}" << endl;
		}
		robdd << endl;
		robdd << "\t0 [label=\"0\", shape = box]" << endl;
		for (int i = 1; i < size - 1; i++)
		{
			if(btree[i].redundent) continue;
			robdd << "\t" << i << " [label=\"" << btree[i].value << "\"]" << endl;
		}
		robdd << "\t" << size - 1 << " [label = \"1\", shape = box]" << endl;

		robdd << endl;
		for (int i = 1; i < size - 1; i++)
		{
			if(btree[i].redundent) continue;
			robdd << "\t" << i << " -> " << btree[i].else_Edge << " [label=\"0\", style=dotted]" << endl;
			robdd << "\t" << i << " -> " << btree[i].then_Edge << " [label=\"1\", style=solid]" << endl;
		}
		robdd << "}" << endl;
	}
	bool IsBooleanTrue(int num)
	{
		string binary_num;
		// get binary 3 -> 0011
		for (int i = i_count - 1; i >= 0; i--)
		{
			if (num >= 1 << i)
			{
				binary_num += '1';
				num -= 1 << i;
			}
			else
			{
				binary_num += '0';
			}
		}
		
		for (int i = 0; i < product_terms.size(); i++)
		{
			bool product_Flag = true;
			for (int j = 0; j < i_count; j++)
			{
				if (product_terms[i][j] != '-' && binary_num[j] != product_terms[i][j])
				{
					product_Flag = false;
					break;
				}
			}
			if(product_terms[i].back() == '0')
				product_Flag = !product_Flag;

			if (product_Flag)
			{
				return true;
			}
		}
		return false;
	}
	void ReducedOrder()
	{
		PrintTree();
		for (int i = i_count - 1; i >= 0; i--) // from down to top
		{
			int start = LayerStart(i), end = LayerEnd(i);
			bool changed = false;
			for (int j = start; j < end; j++)
			{
				if(btree[j].redundent)
					continue;
				if (btree[j].IsEdgeSame()) // Rule 1. the same edge remove
				{
					if (j % 2 == 0)
						btree[j / 2].else_Edge = btree[j].else_Edge;
					else
						btree[j / 2].then_Edge = btree[j].else_Edge;
					btree[j].redundent = true;
					changed = true;
					continue;
				}
				for (int k = j + 1; k < end; k++)
				{
					if (btree[k].redundent) continue;
					if (btree[j] == btree[k])
					{
						if (k % 2 == 0)
							btree[k / 2].else_Edge = j;
						else
							btree[k / 2].then_Edge = j;
						btree[k].redundent = true;
						changed = true;
					}
				}
			}
			if (changed) 
				PrintTree();
		}
	}
	void PrintTree()
	{
		cout << "index" << "\t\t" << "Variable" << "\t" << "Else-edge" << "\t" << "Then-edge" << "\t" << "Comment" << endl;
		cout << "0\t\t-\t\t-\t\t-\t\tBoolean 0" << endl;
		for (int i = 1; i < size - 1; i++) {
			cout << i << "\t\t" << btree[i].value << "\t\t" << btree[i].else_Edge << "\t\t" << btree[i].then_Edge << "\t\t" << (btree[i].redundent ? "redundent" : "") << endl;
		}
		cout << size - 1 << " \t\t-\t\t-\t\t-\t\tBoolean 1" << endl;
		cout << "----------------------------------------------------------------------------" << endl;
	}

	// Variable
	int i_count = 0, o_count = 0, size = 0;
	Node* btree = nullptr;
	vector<string> product_terms;
};

int main(int argc, char* argv[])
{
	ifstream input;
	ofstream bdd;
	ofstream output;
	input.open(argv[1]);
	bdd.open(argv[2]);
	output.open(argv[3]);

	/*ifstream input("input.pla");
	ofstream dotfile("output.pla");*/
	ROBDD robdd;

	string command;
	while (input >> command)
	{
		if (command == ".i")
		{
			input >> robdd.i_count;
			robdd.CreateTree();
		}
		else if (command == ".o") {
			input >> robdd.o_count;
		}
		else if (command == ".ilb") {
			string tmp;
			for (int i = 0; i < robdd.i_count; i++) {
				input >> tmp;
				robdd.CreateLayer(tmp, i);
			}
		}
		else if (command == ".ob") {
			string tmp;
			input >> tmp;
		}
		else if (command == ".p")
		{
			string p_Line;
			int p_Count;
			input >> p_Count;
			input.ignore();
			for (int i = 0; i < p_Count; i++)
			{
				getline(input, p_Line);
				robdd.product_terms.push_back(p_Line);
			}
		}
		else if (command == ".e")
		{
			break;
		}
		else {
			cout << "error PLA file!" << endl;
			return 0;
		}
	}
	robdd.output(bdd, output);
	input.close();
	output.close();
	return 0;
}