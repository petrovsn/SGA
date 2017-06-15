#include <iostream>
#include <stdio.h>
#include <vector>
#include <string>
#include <fstream>
#include <cstdio>
#include <time.h>
#include <map>
#include <set>
#include <math.h>
#include <unordered_map>
#include <tuple>
#include <math.h>

using namespace std;


class Node
{
public:


	bool End;
	map<int, Node*> Prev;
	map<int, Node*> Next;

	string str;
	vector<int> dens;

	int ID;

	unordered_map<int,int> B;
	Node()
	{
	    this->ID = -1;
		this->str = "";
		this->End = false;
	}

	Node(int id)
	{
		this->ID = id;
		this->str = "";
		this->End = false;
	}

	Node(string str, int id)
	{
		this->str = str;
		this->ID = id;
		this->End = false;
	}



	const Node operator = (Node& n)
	{
		this->ID = n.ID;
		this->str = n.str;

		this->B = n.B;
		this->Next = n.Next;
		this->Prev = n.Prev;
		
		return *this;
	}



	Node Split(int pos, int id)
	{
		string tmp = this->str.substr(pos,str.length()-pos);
		this->str = this-> str.substr(0,pos);

		Node n_pr(tmp,id);

		(n_pr.Prev).insert(pair<int,Node*>(this->ID,this));
		

		
		for(auto t1 : this->Next)
        {
            t1.second->Prev.erase(this->ID);
			t1.second->Prev.insert(pair<int,Node*>(id,&n_pr));
			n_pr.Next.insert(t1);
        }



		Next.clear();
		Next.insert(pair<int,Node*>(id,&n_pr));



		return n_pr;
	}

	void Split(int pos, Node* n_pr)
	{
		string tmp = this->str.substr(pos,str.length()-pos);
		this->str = this-> str.substr(0,pos);

		n_pr->str = tmp;

		(n_pr->Prev).insert(pair<int,Node*>(this->ID,this));
				
		for(auto t1 : this->Next)
        {
            t1.second->Prev.erase(this->ID);
			t1.second->Prev.insert(pair<int,Node*>(n_pr->ID,n_pr));
			n_pr->Next.insert(t1);
        }

		Next.clear();
		Next.insert(pair<int,Node*>(n_pr->ID,n_pr));



		for (auto t2: this->B)
		{
			n_pr->B.insert(t2);
			B[t2.first]=n_pr->ID;
		}
		
	}

	struct Aln_Data
	{
		int ID;
		int pos;
		vector<int>thread;
	};


	vector<string> genPosPath(int pos, int len)
	{
		vector<string> res;
		if (len>this->str.length())
		{
			for (auto n1: Next)
			{
				vector<string>tmp = n1.second->genPosPath(0,len - (this->str.length()-pos));
				res.insert(res.end(),tmp.begin(),tmp.end());
			}
		}
		else
		{
			res.push_back(str.substr(pos,len));
		}
		return res;
	}

	void print()
	{
		cout<<"ID= "<<ID<<'\n';
		cout<<"str= "<<str<<'\n';
		cout<<"lenNext= "<<Next.size()<<'\n';
		cout<<"lenPrev= "<<Prev.size()<<'\n';
	}
};



class GenerHash
{
public:
	vector<unsigned long> p;
	int base;
	int len; 

	unsigned long long currhash;
	int curr_len;
	
	string str;
	int id;
	int pos;
	int m_pos;

	bool formed;

	GenerHash()
	{

	}

	GenerHash(int base, int len)
	{
		this->base = base;
		this->len = len;
		for (int i=len-1; i>=0; i--)
		{
			p.push_back(pow(base,i));
		}
	}

	int ord(char c)
	{
		switch (c)
		{
		case 'a': return 1;
			break;
		case 'c': return 2;
			break;
		case 'g': return 3;
			break;
		case 't': return 4;
			break;
		default:
			cout<<"ooops";
			break;
		}
	
	}

	void Init(int base, int len)
	{
		this->base = base;
		this->len = len;
		for (int i=len-1; i>=0; i--)
		{
			p.push_back(pow(base,i));
		}
	}

	void ReInit(int Id, string str)
	{
		this->str = str;
		this->m_pos = str.length()-1;

		this->curr_len = 0;
		this->currhash = 0;

		this->pos = 0;
		this->id = Id;
		this->formed = false;
	}

	unsigned long long Inc(char c)
	{
		currhash += ord(c)*p[curr_len];
		curr_len++;
		if (curr_len == len) 
		{
			formed = true;
			return currhash;
		}
		return -1;
	}

	unsigned long long Next(char c1)
	{
		unsigned long long res;
		if (!formed)
		{
			res = Inc(c1);
		}
		else
		{
			if (pos<m_pos)
			{
				currhash = (currhash-ord(str[pos])*p[0])*base + ord(c1);
				pos++;
				res = currhash;
			}
			else
			{
				res = -2;
			}
		}
		return res;
	}

	int left()
	{
		return len - curr_len;
	}
};

struct PoAl
{
	int ID;
	int start;
	int offset;
};

struct WArray {int value[4];};

class Graph
{
public:
	unordered_map<int, Node> Body;


	unordered_map<unsigned long long, vector<WArray>> hashtable;

	Graph()
	{

	}

	GenerHash GH;

//================================================================================================
	void LoadReference(string ref)
	{
		Node startNode(ref,0);
		startNode.B.insert(pair<int,int>(0,1));
		Body.insert(pair<int,Node>(0, startNode));
		Node endNode("",1);
		endNode.End = true;

		Body.insert(pair<int,Node>(0, endNode));


//		Body[0].B.insert(pair<int,int>(0,1));
		Body[1].End = true;

		Link(&Body[0],&Body[1]);
	//	GlobCount++;
	}
	
	void Add(string str, int ThreadID, int pos1)
	{
		int pos2 = pos1+str.length();
		pair<int,int> tmp_pair1 = defNodePos(ThreadID,pos1);
		Node tmpNode1(Body.size());
		Body.insert(pair<int,Node>(tmpNode1.ID,tmpNode1));
		Body[tmp_pair1.first].Split(tmp_pair1.second,&Body[tmpNode1.ID]);
		
		int ID1 = tmp_pair1.first;

		tmp_pair1 = defNodePos(ThreadID,pos2);
		Node tmpNode2(Body.size());
		Body.insert(pair<int,Node>(tmpNode2.ID,tmpNode2));
		Body[tmp_pair1.first].Split(tmp_pair1.second,&Body[tmpNode2.ID]);
		
		int ID2 = tmpNode2.ID;

		Node incNode(str,Body.size());
		int incID = incNode.ID;

		Body.insert(pair<int,Node>(incID,incNode));
		Link(&Body[ID1],&Body[incID]);
		Link(&Body[incID],&Body[ID2]);
	}      //!!!!

	pair<int,int> defNodePos(int ThreadId, int pos)
	{
		
		Node curr_Node = Body[0];
		int curr_len = curr_Node.str.length();
		int curr_pos = 0;
		
		int f_ID = 0;

		while (pos>curr_len)
		{
			curr_Node.Next[ThreadId]->print();
		//	system("pause");
			int tmp = curr_Node.Next[ThreadId]->ID;
			curr_Node = Body[tmp];
			curr_len+=curr_Node.str.length();
		}

		return pair<int,int>(curr_Node.ID, curr_Node.str.length()-1-(curr_len-pos));
	}
	
	pair<int,int> defNodePos(int ThreadId, int pos, Node curr_Node1, int curr_pos1)
	{
		int curr_ID = curr_Node1.ID;
		int curr_len = Body[curr_ID].str.length();
		int curr_pos = curr_pos1;

		do
		{
			if ((curr_pos<pos)&&(pos<=curr_pos+curr_len))
			{
				return pair<int,int>(curr_ID,(pos-curr_pos));
			}
			else
			{
				curr_ID = Body[curr_ID].B[ThreadId];
				curr_pos+=curr_len;
				curr_len = Body[curr_ID].str.length();
			}

		}while (!Body[curr_ID].End);
		
		return pair<int,int>(-1,-1);
	}
		
	void LoadVariance(int ThreadID1, int ThreadID2, vector<tuple<int, int, string>> VCF)
	{
		//walk down the thread1, incepting variance node from position list.
		//ints in VCF should be sorted

		int curr_ID = 0;
		int curr_len = Body[curr_ID].str.length();
		int curr_pos = 0;
		
		int vcf_id = 0;


		do
		{
			int pos1 = get<0>(VCF[vcf_id]);
			int pos2 = get<1>(VCF[vcf_id]);
			string str = get<2>(VCF[vcf_id]);
			int ID1 = 0;
			int ID2 = 0;

			if ((curr_pos<pos1)&&(pos1<=curr_pos+curr_len))
			{

				if (pos1==curr_pos+curr_len) //начало вариации со стыка нодов
				{
					ID1 = Body[curr_ID].ID;
				}
				else 
				{
					ID1 = Body[curr_ID].ID;
					Node tmpNode1(Body.size()); 
					Body.insert(pair<int,Node>(tmpNode1.ID,tmpNode1));
					Body[curr_ID].Split(pos1-curr_pos,&Body[tmpNode1.ID]);
				}
				
				
				pair<int,int> tmp_pair1 = defNodePos(ThreadID1, pos2, Body[curr_ID], curr_pos);

				if(Body[tmp_pair1.first].str.length()==tmp_pair1.second) //конец вариации на стык нодов
				{
					Node tmp = Body[tmp_pair1.first];
					ID2 = Body[tmp_pair1.first].B[ThreadID1];
				}
				else
				{
					Node tmpNode1(Body.size());
					ID2 = tmpNode1.ID;
					Body.insert(pair<int,Node>(tmpNode1.ID,tmpNode1));
					Body[tmp_pair1.first].Split(tmp_pair1.second,&Body[tmpNode1.ID]);
				}


			//	RecurcivePrint();
				Node incNode(str,Body.size());
				int incID = incNode.ID;

				Body.insert(pair<int,Node>(incID,incNode));
				Link(&Body[ID1],&Body[incID]);
				Link(&Body[incID],&Body[ID2]);
				if(vcf_id<VCF.size()-1) vcf_id++;
			}
			else
			{
			//	Body[curr_ID].Next.insert(pair<int,Node*>((ThreadID2), Body[curr_ID].Next[ThreadID1]));
				Body[curr_ID].B.insert(pair<int,int>(ThreadID2, Body[curr_ID].B[ThreadID1]));
			}
			curr_pos+=Body[curr_ID].str.length();
			curr_ID = Body[curr_ID].B[ThreadID1];
			curr_len = Body[curr_ID].str.length();
		}
		while (Body[curr_ID].End != true);
	}	

	void Link(Node* N1, Node* N2)
	{
		(N1->Next).insert(pair<int,Node*>(N2->ID,N2));
		(N2->Prev).insert(pair<int,Node*>(N1->ID,N1));
		
	}

//================================================================================================


	void RecurcivePrint()
	{
		for (int i =0; i<Body.size(); i++)
		{
			cout<<Body[i].ID<<": "<<Body[i].str<<endl;
			for (auto t: Body[i].Next)
			{
				cout<<"==>"<<t.second->ID<<": "<<t.second->str<<endl;
			}
		}
	}

	void print()
	{
		for (auto n1: Body)
		{
			cout<<n1.second.ID<<'\t'<<n1.second.str<<'\n';
		}
	}
//================================================================================================
	
	void GenHash(int base, int len)
	{
		GH.Init(base,len);
		for(auto t1: Body)
		{
			if (t1.second.ID!=(-1))
			{
			GenPrimeTable(t1.second,base,len);
			}
		}
	}


	void GenPrimeTable(Node n1, int base, int len)
	{

		GH.ReInit(n1.ID, n1.str);
		int pos = 0;
		unsigned long long res = 0;
		
		for (int i=0; i<n1.str.length(); i++)
		{
			if (GH.Next(n1.str[i])>0)
			{
				AddHash(n1.ID, GH.pos, n1.ID, i, GH.currhash);
			}
			else
			{
			//	cout<<"oooops"<<endl;
			}
		}


		for (auto node: n1.Next)
		{
			CallNode(*(node.second),GH);
		}
	}

	void CallNode(Node n1, GenerHash GH2)
	{
		if (n1.ID==-1) return;
		unsigned long long res = 0;
		for (int i =0; i<n1.str.length(); i++)
		{
			res = GH.Next(n1.str[i]);
			if (res == -2)//pointer to position at the end of node. return;
			{
				return;
			}
			else
			{
				AddHash(GH.id, GH.pos, n1.ID, i, GH.currhash);
			}
		}

		for (auto p1: n1.Next)
		{
			CallNode(*(p1.second), GH2);
		}
	}

	void AddHash(int ID, int pos, int fID, int fpos, unsigned long long hash)
	{
		WArray tmp;
		tmp.value[0] = ID;
		tmp.value[1] = pos;
		tmp.value[2] = fID;
		tmp.value[3] = fpos;

		try
		{
			hashtable[hash].push_back(tmp);
		}
		catch(...)
		{
			//cout<<"oooooops, exeption"<<endl;
		}
	}

	
//================================================================================================

	void NotSimpleFinderSNAP(string str)
	{
		vector<unsigned long long> hash_seeds;
		GH.ReInit(-2,str);

		for (int i=0; i<str.length(); i++)
		{
			int res = GH.Next(str[i]);
			if (res > 0)
			{
				hash_seeds.push_back(GH.currhash);
			}
		}



		vector<vector<WArray>> hit_place; //(hash_seeds.size());
		
		for(int i=0; i<hash_seeds.size(); i++)
		{
			vector<WArray> tmp;
			hit_place.push_back(tmp);
			for(int j = 0; j<hashtable[hash_seeds[i]].size(); j++)
			{
				hit_place[i].push_back(hashtable[hash_seeds[i]][j]);
			}
		}


		/*for(int i=0; i<hit_place.size(); i++)
		{ 
			cout<<hash_seeds[i]<<'\t';
			for(int j = 0; j<hit_place[i].size(); j++)
			{
				cout<<hit_place[i][j].value[0]<<'.'<<hit_place[i][j].value[1]<<"->"<<hit_place[i][j].value[2]<<'.'<<hit_place[i][j].value[3]<<'\t';
			}
			cout<<'\n';
		}*/
		
		for(int i =0; i<hit_place[0].size(); i++)
		{
			ASTRun(i, str.length(), hit_place);
		}
	
	}

	void ASTRun(int i_start, int pos_end, vector<vector<WArray>> hit_place)
	{
		int pos = GH.len;
		int state[2] = {hit_place[0][i_start].value[2],hit_place[0][i_start].value[3]};
		do
		{
			
			int tmp = NextPos(state[0],state[1],hit_place[pos]);
			if (tmp==-1) return;
			state[0] = hit_place[pos][tmp].value[2];
			state[1] = hit_place[pos][tmp].value[3];
			pos+=GH.len;
		}
		while (pos<pos_end-GH.len);

		cout<<"ASTRun completed"<<endl;
		
	}

	int NextPos(int node, int pos, vector<WArray> hits)
	{
		int res = -1;
		int hip_n = node, hip_p = pos+1;
		for(int i =0; i<hits.size(); i++)
		{
			if((hip_n==hits[i].value[0])&&(hip_p==hits[i].value[1])) return i;
		}
		for(auto n1: Body[node].Next)
		{
			hip_n = n1.second->ID;
			for(int i =0; i<hits.size(); i++)
			{
				if((hip_n==hits[i].value[0])&&(0==hits[i].value[1])) return i;
			}
		}
		return res;
	}
/*
	{
		AutoAlign1 AA1;
		
		for(int i=0; i<hit_place.size(); i++)
		{
			for(int j=0; j<hit_place[i].size(); j++)
			{
				AA1.Reinit(hit_place[i][j]);
				int q=1;
				while ((i+q<hit_place.size())&&(AA1.Next(hit_place[i+q], (map_blocks[i]))))
				{
					q++;
				}
				if (i+q==hit_place.size())
				{
					PoAl tmp;
					tmp.ID = AA1.start.first;
					tmp.start = AA1.start.second;
					tmp.offset = AA1.end.second - tmp.start;
					map_blocks[i].push_back(tmp);
				}
			}
		}
	}*/

/*	void AlignStepTwo(vector<vector<PoAl>>  map_blocks, string str)
	{
		for(int i =0; i<map_blocks[0].size(); i++)
		{
			AST_run(str, map_blocks[0][i],map_blocks);
		}
	}

	void AST_run(string str, PoAl s_node, vector<vector<PoAl>>  map_blocks)
	{
		//cout<<"start run\n";

	
		PoAl c_node = s_node;

		int c_pos = c_node.offset;
		int cn_pos = c_node.start+c_node.offset;

		do
		{

		if (Body[c_node.ID].str.length() == cn_pos+1)
		{
			c_node = AST_next(map_blocks[c_pos+1],c_node.ID);
		}
		else if ((Body[c_node.ID].str.length() > cn_pos+1)&&(c_pos+GH.len == str.length()))
		{
	//		cout<<"end"<<endl;
			return;
		}
		else return;


		if (c_node.ID != -2)
		{
			cn_pos = c_node.start+c_node.offset;
			c_pos+=(1+c_node.offset);
		}
		else return;
		
	//	cout<<"align:"<<c_node.ID<<'\n';
		}
		while(c_pos<str.length());
	}


	PoAl AST_next(vector<PoAl> m_block, int ID)
	{
		for(auto n1: Body[ID].Next)
		{
				for(auto p2: m_block)
				{
					if((n1.second->ID == p2.ID) && (p2.start == 0))
					{
						return p2;
					}
				}
		}
		PoAl tmp;
		tmp.ID=-2;
		tmp.offset = -1;
		tmp.start = -1;
		return tmp;
	}*/
};

string loadfasta(string path)
{
	string buf;
	ifstream fin(path);

	string head;
	char c;
	getline(fin,head);
	while (!fin.eof())
	{
		c = tolower(fin.get());
		if ((c=='a')||(c=='c')||(c=='g')||(c=='t')) buf+=c;
	}
	cout<<head<<'\t'<<buf.size()<<'\t';
	return buf;
}

vector<tuple<int,int,string>> loadVCF(string path)
{
	vector<tuple<int,int,string>> VCF;
	string buf;
	ifstream fin(path);
	for(int i = 0; i<1; i++)
	{
		getline(fin, buf);
	}
	while (!fin.eof())
	{
		getline(fin, buf, '\t');//#chrom
		getline(fin,buf,'\t');//#pos
		int pos1 = stoi(buf);
		getline(fin,buf,'\t'); //#ID
		getline(fin,buf,'\t'); //ref
		string alt;
		getline(fin,alt,'\t'); //ref
		int pos2 = pos1+alt.size();

		VCF.push_back(tuple<int,int, string>(pos1, pos2, alt));
		getline(fin,alt);
	}


	return VCF;
}

string trygen(int len)
{
	string buf = "";
	for(int i=0; i<len; i++)
	{
		int q = rand()%4;
		switch (q)
		{
		case 0:
			buf+='a';
			break;
		case 1:
			buf+='c';
			break;
		case 2:
			buf+='g';
			break;
		case 3:
			buf+='t';
			break;
		}
	}
	return buf;
}

vector<tuple<int,int,string>> genVCF(string ref, int dens, int length)
{
	vector<tuple<int,int,string>> VCF;
	for(int i =0; i<ref.length(); i+=dens)
	{
		int rand0 = rand()%dens;
		int rand1 = rand()%length+1;
		string buf = "";
		for(int i=0; i<rand1; i++)
		{
			int q = rand()%4;
			switch (q)
			{
			case 0:
				buf+='a';
				break;
			case 1:
				buf+='c';
				break;
			case 2:
				buf+='g';
				break;
			case 3:
				buf+='t';
				break;
			}
		}
		int pos1 = i+rand0;
		int pos2 = pos1+buf.length();
		VCF.push_back(tuple<int,int, string>(pos1, pos2, buf));
	}
	return VCF;
}

int main( int argc, const char* argv[] )
{
	clock_t t1 = clock();
	string ref3 = loadfasta("C:/Users/Sergey/Documents/Visual Studio 2012/Projects/gPBWTAligner/Debug/seq2.fasta");
	//string test = "ACGTAACCGGTTAAACCCGGGTTTACGTAACCGGTTAAACCCGGGTTT";
	
	cout<<endl<<"load fasta file:"<<clock()-t1<<endl;
	t1 = clock();
	vector<tuple<int,int,string>> VCF = genVCF(ref3,100,5);
	cout<<"gen variation:"<<clock()-t1<<endl;
	Graph  testGraph;
	t1 = clock();
	testGraph.LoadReference(ref3);
	cout<<"load reference:"<<clock()-t1<<endl;

	t1 = clock();
	testGraph.LoadVariance(0,1,VCF);
	cout<<"load variance:"<<clock()-t1<<endl;

	string example = ref3.substr(10000,10000);

	t1 = clock();
	testGraph.GenHash(5,13);
	cout<<"generating hash:"<<clock()-t1<<endl;

	t1 = clock();
	testGraph.NotSimpleFinderSNAP(example);
	cout<<"SNAP:"<<clock()-t1<<endl;

	system("pause");
	
/*	string ref3 = "ACGTAACCGGTTAAACCCGGGTTTACGTAACCGGTTAAACCCGGGTTT";
	string example = "AAACCCGGGTTTACGTAACCGG";
    vector<tuple<int,int,string>> VCF;
	VCF.push_back(tuple<int,int,string>(4,5,"T"));
	VCF.push_back(tuple<int,int,string>(7,8,"T"));
	VCF.push_back(tuple<int,int,string>(10,20,"ATATATATAT"));
	VCF.push_back(tuple<int,int,string>(34,40,"CCCCCCC"));

	Graph  testGraph;
	clock_t t1 = clock();
	testGraph.LoadReference(ref3);
	cout<<"load reference:"<<clock()-t1<<endl;

	t1 = clock();
	testGraph.LoadVariance(0,1,VCF);
	cout<<"load variance:"<<clock()-t1<<endl;
	t1 = clock();
	testGraph.GenHash(5,5);
	cout<<"generating hash:"<<clock()-t1<<endl;

	t1 = clock();
	testGraph.NotSimpleFinderSNAP(example);
	cout<<"SNAP:"<<clock()-t1<<endl;

	system("pause");*/
}