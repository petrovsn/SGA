/*#include <string>
#include <vector>
using namespace std;



class gPBWT_Graph
{
public:
	gPBWT_Graph(string ref);

	void addvar(int pos1, int pos2, string altseq);

	string exthread(int num);

private:
	int locate(int glob_pos);

	vector<gPBWT_Node> Nodes;
	int NodeCount;
};


class gPBWT_Node
{
public:
	int ID;
	
	gPBWT_Node(int newID, string seq);

	int c_func(int insID);

	vector<int[2]> getSide;

	void InsToEnd();  ////// \\\\\-> /////////
	
	void InsToBegin();  ////// <-\\\\\ ////////

protected:
	int smth1;
private:

	string sequence;
	void Split(int pos);

	vector<int[2]> ls;
	vector<int[2]> rs;

};*/