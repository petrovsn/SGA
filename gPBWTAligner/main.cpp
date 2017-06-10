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




class SA_IS
{
	

public:

	string sample;
	vector<int> ind;
	SA_IS(string S, int alphabet)
	{
		sample = S+'$';
		vector<int> str(S.size());

		for (int i=0; i<S.size();i++) str[i] = S[i];

		ind = makeSuffixArrayByInduceSorting(str,alphabet);
	};
	
	void print_full()
	{
		int L = ind.size();
		for (int i=0; i<L; i++)
		{
			cout<<i<<'\t'<<ind[i]<<'\t'<<sample.substr(ind[i],L-ind[i])<<sample.substr(0,ind[i])<<'\n';
		}

	}

	vector<char> buildTypeMap(vector<int> S)
	{
		int L = S.size();
		vector<char> res(L+1);
		res[L]='u';
		res[L-1]='d';
		for (int i=L-2; i>=0; i--)
		{
			if (S[i]<S[i+1]) res[i]= 'u';
			if (S[i]>S[i+1]) res[i]= 'd';
			if (S[i]==S[i+1]) res[i]=res[i+1];
		}


		for (int i=1; i<L; i++)
		{
			if ((res[i-1]=='d')&&(res[i]=='u')) res[i]='w';
		}
		return res;
	}

	bool isLMSchar(int i, vector<char> typemap)
	{
		if (i==0) return false;
	//	if ((typemap[i]=='u')&&(typemap[i-1]=='d')) return true;
		if (typemap[i]=='w') return true;
		return false;
	}

	bool lmsSubstringsAreEqual(int i1, int i2, vector<int>* S, vector<char>* typemap)
	{
		int L = (*S).size();
		if ((i1 == L)||(i2==L)) return false;
		if (i1==i2) return true;

		int i=0;
		for(;;)
		{
			bool aIsLMS = ((*typemap)[i+i1]=='w');
			bool bIsLMS = ((*typemap)[i+i2]=='w');

			if((i>0)&&(aIsLMS)&&(bIsLMS)) return true;

			if (aIsLMS != bIsLMS) return false;

			if ((*S)[i+i1] != (*S)[i+i2]) return false;

			i+=1;

		}
	}

	vector<int> findBucketSizes(vector<int> str, int abcSize = 256)
	{
		vector<int>res(abcSize);
		for(int i=0; i<str.size();i++)
		{
			res[str[i]]++;
		}
		return res;
	}

	vector<int>findBucketTails(vector<int> BucketSize)
	{
		int offset=1;
		vector<int>res(BucketSize.size());

		for (int i=0; i<BucketSize.size(); i++)
		{
			offset+=BucketSize[i];
			res[i]=offset-1;
		}
		return res;
	}

	vector<int>findBucketHeads(vector<int> BucketSize)
	{
		int offset=1;
		vector<int>res(BucketSize.size());

		for (int i=0; i<BucketSize.size(); i++)
		{
			res[i]=offset;
			offset+=BucketSize[i];
		}
		return res;
	}

	vector<int> guessLMSSort(vector<int> str, vector<int> bucketSizes, vector<char> typemap)
	{
		vector<int>gSuffArr(str.size()+1);
		for(int i=0; i<gSuffArr.size();i++) gSuffArr[i]=-1;

		vector<int> buckTails = findBucketTails(bucketSizes);

		int L = str.size();

		for (int i=0; i<L;i++)
		{
			
			if (typemap[i]=='w'){
			
			int buckIndex = str[i];
	
			gSuffArr[buckTails[buckIndex]]=i;

			buckTails[buckIndex]-=1;
			

			
			}
		}

		gSuffArr[0]=str.size();


		buckTails.clear();

		return gSuffArr;
	}

	vector<int> induceSortL(vector<int> str, vector<int> gsuffArr, vector<int> buckSizes, vector<char> typemap)
	{
		vector<int> buckHeads = findBucketHeads(buckSizes);

		for (int i=0; i<gsuffArr.size(); i++)
		{
			if(gsuffArr[i]==-1) continue;

			int j = gsuffArr[i]-1;
			if ((j<0)||(typemap[j]!='d')) continue;

			int buckIndex=str[j];

			gsuffArr[buckHeads[buckIndex]]=j;
			buckHeads[buckIndex]+=1;
		}

		buckHeads.clear();
		return gsuffArr;
	}

	vector<int> induceSortS(vector<int> str, vector<int> gsuffArr, vector<int> buckSizes, vector<char> typemap)
	{
		vector<int> buckTails = findBucketTails(buckSizes);

		for (int i=gsuffArr.size()-1; i>=0; i--)
		{
			int j=gsuffArr[i]-1;

			if ((j<0)||((typemap[j] !='u')&&(typemap[j] !='w'))) continue;

			int buckIndex = str[j];

			gsuffArr[buckTails[buckIndex]] = j;
			buckTails[buckIndex]-=1;
		}

		buckTails.clear();
		return gsuffArr;
	}

	struct SSS
	{
		vector<int> sStr;
		vector<int> sSuffOff;
		int sAlBethSize;

		void clear()
		{
			sStr.clear();
			sSuffOff.clear();
		}
	};

	SSS summSuffArray(vector<int> str, vector<int> gsuffArr, vector<char> typemap)
	{
		vector<int>lmsNames(str.size()+1);
		for(int i=0; i<lmsNames.size();i++) lmsNames[i]=-1;

		int currentName=0;
		lmsNames[gsuffArr[0]]=currentName;
		int lastLMSsuffoff = gsuffArr[0];

		for(int i=1; i<gsuffArr.size();i++)
		{
			int suffOffs = gsuffArr[i];
			if (!(typemap[suffOffs]=='w')) continue;

			if(!lmsSubstringsAreEqual(lastLMSsuffoff,suffOffs,&str,&typemap)) currentName+=1;
			
			lastLMSsuffoff = suffOffs;

			lmsNames[suffOffs]=currentName;
		}


		vector<int> summSuffOff(0);
		vector<int> summString(0);

		for (int i=0; i<lmsNames.size(); i++)
		{
			if (lmsNames[i]==-1) continue;

			summSuffOff.push_back(i);
			summString.push_back(lmsNames[i]);
		}

		int sumABCSize = currentName+1;

		SSS res;

		res.sStr=summString;
		res.sAlBethSize = sumABCSize;
		res.sSuffOff = summSuffOff;


		lmsNames.clear();


		return res;
	}

	vector<int> accurateLMSSort(vector<int> str, vector<int> buckSizes, vector<char> typemap,
								vector<int> summarSuffArray, vector<int> summSuffOffs)
	{
		vector<int>suffixOff(str.size()+1);
		for (int i=0; i<suffixOff.size(); i++)suffixOff[i]=-1;

		vector<int> buckTails = findBucketTails(buckSizes);

		for(int i=summarSuffArray.size()-1;i>1;i--)
		{
			int strIndex = summSuffOffs[summarSuffArray[i]];

			int buckIndex = str[strIndex];

			suffixOff[buckTails[buckIndex]] = strIndex;
			buckTails[buckIndex] -=1;
		}

		suffixOff[0]= str.size();
		buckTails.clear();

		return suffixOff;
	}

	
	vector<int> makeSummSuffArr(vector<int> summStr, int summAlphSize)
	{
		vector<int> summarySuffixArray;
		if (summAlphSize == summStr.size())
		{
			summarySuffixArray = vector<int>(summStr.size()+1);
			for (int i=0; i<summarySuffixArray.size();i++)summarySuffixArray[i]=-1;

			summarySuffixArray[0]=  summStr.size();

			for (int i=0; i<summStr.size();i++)
			{
				int y = summStr[i];
				summarySuffixArray[y+1]=i;
			}
		}
		else
		{
			cout<<"recurcive"<<'\n';
			summarySuffixArray = makeSuffixArrayByInduceSorting(summStr, summAlphSize);
		}

		return summarySuffixArray;
	}



	vector<int> makeSuffixArrayByInduceSorting(vector<int> str, int summAlphaSize)
	{
		clock_t t1 = clock();
		vector<char> typemap = buildTypeMap(str);
		vector<int>BucketSizes = findBucketSizes(str,summAlphaSize);

		vector<int> gSuffixArray = guessLMSSort(str,BucketSizes,typemap);

		gSuffixArray= induceSortL(str, gSuffixArray, BucketSizes, typemap);
		gSuffixArray= induceSortS(str, gSuffixArray, BucketSizes, typemap);

		clock_t t2 = clock();
		cout<<"gSuffArray builded"<<'\t'<<t2-t1<<'\n';
		SSS inf = summSuffArray(str, gSuffixArray, typemap);

		gSuffixArray.clear();

		vector<int> summarySuffixArray = makeSummSuffArr(inf.sStr, inf.sAlBethSize);
		clock_t t3 = clock();
		cout<<"resstarted"<<'\t'<<t3-t2<<'\n';
		vector<int> res = accurateLMSSort(str, BucketSizes, typemap,summarySuffixArray,inf.sSuffOff);

		res = induceSortL(str, res, BucketSizes, typemap);
		res = induceSortS(str, res, BucketSizes, typemap);

		typemap.clear();
		BucketSizes.clear();
		inf.clear();
		summarySuffixArray.clear();

		return res;
	}
};


class BWT_Al
{
public:
	string T;
	string Bstr;
	string Brev;
	vector<int> index_algn;
	map<char, int> C;

	BWT_Al()
	{

	}
	BWT_Al(string reference)
	{
		T = reference;

		SA_IS s1(reference,256);
		Bstr = Bfromind(s1.ind,T);

		reverse(reference.begin(), reference.end());

		SA_IS s2(reference, 256);

		Brev = Bfromind(s2.ind,reference);

		index_algn = s1.ind;


		T=T+'$';
		for each(char c in "atcg")
			if (c != '\0')
				C.insert(pair<char,int>(c,C_gen(c)));

		cout<<T<<'\n'<<Bstr<<'\n';
	}
	
	void ExactSearch(string sample)
	{
		int L = sample.length();
		int rl = 0;
		int rr = T.length()-1;
		int z=0;
		vector<int>D = vector<int>(rr);
     //   cout<<"start_let:"<<sample[L-1]<<":"<<rl<<"--"<<rr<<'\n';

		for(int i = L-1; i>=0; i--)
		{
	//		cout<<"rl="<<C[sample[i]]<<'+'<<O(sample[i], rl-1)<<'+'<<'1'<<'\n';
			rl = C[sample[i]] + O(sample[i], rl-1) + 1; 
			

		//	cout<<"rr="<<C[sample[i]]<<'+'<<O(sample[i], rr)<<'\n';
			rr = C[sample[i]] + O(sample[i], rr);
			
		/*	if (index_algn[rl]==0)
			{
				cout<<"s";
			}*/
			
			if (rl>rr)
			{
				rl = 0;
		        rr = T.length()-1;
				z++;
			}
			D[i]=z;
			//cout<<"temp:"<<sample.substr(i,L-1)<<":"<<rl<<"--"<<rr<<'\n';
		}
		
		for(int i = rl; i<=rr; i++)
		{
			cout<<index_algn[i]<<'\n';
		}
		//cout<<"FINAL:"<<ind[rl]<<"--"<<ind[rr]<<'\n';
   	}

	bool Search(string W, int z=0)
	{
		cout<<"start inex search"<<'\n';
		int lW = W.length();
		int lT = T.length();
		int* D = CalculateD(W);
		cout<<"errorC\n";
		for (int i=0; i<W.length(); i++)
		{
			cout<<D[i]<<'\t';
		}
		cout<<'\n';
		//-------------------------
		vector<pair<int,int>> I1;
		int k = 1;
		bool IsBackAl = false;

		InexRecur(W, D, lW-1, z,k, lT-1, I1, IsBackAl);
		

		cout<<"out_data:\n";
		for (int i=0; i<I1.size(); i++)
			for (int j=I1[i].first; j<=I1[i].second;j++)
			{
			cout<< index_algn[j]<<'\n';
			}
		delete[] D;
		return IsBackAl;
	}

	~BWT_Al()
	{
		index_algn.clear();
	}

private:
	int C_gen(char a)
	{
		int C = 0;
		for (int i=0; i<T.length()-1; i++)
		{
			if (T[i]<a) C++;
		}
		return C;
	}

	int O(char a, int R)
	{
		int O = 0;
		for (int i=0; i<=R; i++)
		{
			if (Bstr[i]==a) O++;
		}
		return O;
	}
	int O_sh(char a, int R)
	{
		int O = 0;
		for (int i=0; i<=R; i++)
		{
			if (Brev[i]==a) O++;
		}
		return O;
	}
 
	string      Bfromind(vector<int> index, string S)
	{
		int L = index.size();
		string B = ""; 
		for(long long int i=0; i<L; i++) 
		{
			if (index[i] == 0)
			{
				B+='$';
			}
			else
			{
				B += S[index[i]-1];
			}
		}
		return B;
	}

	int* CalculateD(string W)
	{
		int* Dl = new int[W.length()];
		vector<int>Dl1 = vector<int>(W.length());
		int L = T.length()-1;
		int k=0;
		int l = L;
		int z = 0;
		for (int i=0; i<W.length(); i++)
		//for (int i=W.length()-1; i>=0; i--)
		{
			k = C[W[i]]+O_sh(W[i],k-1)+1;
			l = C[W[i]]+O_sh(W[i],l);

			if (k>l)
			{
				k=0;
				l=T.length()-1;
				z=z+1;
			}
			Dl[i] = z;
			Dl1[i]=z;
		}
		return Dl;
	}

    void        InexRecur(const string W, const int* D, int i, int z, int k, int l,
							vector<pair<int,int>>& I, bool& IsZeroBack)
	{

		if ((z<D[i]))
			return;

		if (i<0)
		{
			I.push_back(pair<int,int>(k,l));
			return;
		}

	//	InexRecur(W,D, i-1,z-1,k,l,I);
		int k1 = k;
		int l1 = l;
		for each(char b in "acgt")
 		{
			if(b !='\0')
			{
			k1 = C[b]+O(b,k-1)+1;
			l1 = C[b]+O(b,l);

			if (k1<=l1)
			{
		//		InexRecur(W, D, i,z-1,k,l,I);
				if (b==W[i])
				{
					for (int q = k1; q<=l1; q++)
					{
						if(index_algn[q]==0) 
     					{
						IsZeroBack = true;
						}

					}
					InexRecur(W, D, i-1,z,k1,l1,I, IsZeroBack);
				}
				else
					InexRecur(W, D, i-1,z-1,k1,l1,I, IsZeroBack);
			}
			}
		}
	}
};




unsigned int fhash(
    const char* s,
    unsigned int seed = 0)
{
    unsigned int hash = seed;
    while (*s)
    {
        hash = hash * 101  +  *s++;
    }
    return hash;
}

class Node
{
public:

	BWT_Al aligner;

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

	void Activate_SAIS()
	{
	}

	struct Aln_Data
	{
		int ID;
		int pos;
		vector<int>thread;
	};

	void Align(string str, Aln_Data info)
	{
		bool isBack = aligner.Search(str);
		if (isBack)
		{
			for (auto t1: Prev)
		{
			t1.second->aligner.Search(str);
			}
		}
		else
		{

		}
		

		return;
	}

	void Map(unsigned int pos, unsigned int len)
	{
		for (int i =pos; i<min(len, dens.size()-1);i++)
		{
			dens[i]++;
		}
	}

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

	long long currhash;
	int curr_len;
	
	string str;
	int id;
	int pos;
	int m_pos;

	bool formed;

	int ord(char c)
	{
		return ((int)c - (int)'A'+1);
	}


	GenerHash()
	{

	}
	GenerHash(int base, int len)
	{
		this->base = base;
		this->len = len;
		for (int i=0; i<len; i++)
		{
			p.push_back(pow(base,i));
		}
	}

	void Init(int base, int len)
	{
		this->base = base;
		this->len = len;
		for (int i=0; i<len; i++)
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
	long long Inc(char c)
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

	long long Next(char c1)
	{
		long long res;
		if (!formed)
		{
			res = Inc(c1);
		}
		else
		{
			if (pos<m_pos)
			{
				currhash = (currhash-ord(str[pos]))/base + ord(c1)*p[curr_len-1];
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

class AutoAlign1
{
public:
	pair<int,int> start;
	pair<int,int> state;
	pair<int,int> end;

	AutoAlign1()
	{
	}

	void Reinit(pair<int,int> p)
	{
		state = p;
		start = p;
		end = p;
	}

	bool Next(vector<pair<int,int>> p, vector<PoAl> &data)
	{
		pair<int,int> hipotetical = pair<int,int>(state.first,state.second+1);
		if (find(p.begin(), p.end(), hipotetical) != p.end())
		{
			state = hipotetical;
			end = hipotetical;
			return true;
		}
		else
		{
		//	cout<<"Align block: "<<start.first<<"."<<start.second<<"->"<<end.first<<"."<<end.second<<'\n';
			PoAl tmp;
			tmp.ID = start.first;
			tmp.start = start.second;
			tmp.offset = end.second - tmp.start;
			data.push_back(tmp);
			return false;
		}
		return false;
	}

};


class Graph
{
public:
	unordered_map<int, Node> Body;


	unordered_map<long long, vector<pair<int,int>>> hashtable;

	Graph()
	{

	}

	GenerHash GH;
	void LoadVCF(string filename)//test function
	{
	}

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
					ID2 = Body[tmp_pair1.first].Next[ThreadID1]->ID;
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

	vector<pair<char,int>> ExtractThread()
	{
	};

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
		long long res = 0;
		
		for (int i=0; i<n1.str.length(); i++)
		{
			res = GH.Next(n1.str[i]);
			if (res == -1)//not formed
			{

			}
			else if (res == -2)//pointer to position at the end of node. impossible.
			{

			}
			else
			{
				AddHash(n1.ID, GH.pos, GH.currhash);
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
		long long res = 0;
		for (int i =0; i<n1.str.length(); i++)
		{
			res = GH.Next(n1.str[i]);
			if (res == -1)//not formed
			{

			}
			else if (res == -2)//pointer to position at the end of node. return;
			{
				return;
			}
			else
			{
				AddHash(GH.id, GH.pos, GH.currhash);
			}
		}

		for (auto p1: n1.Next)
		{
			CallNode(*(p1.second), GH2);
		}
	}

	void AddHash(int ID, int pos, long long hash)
	{
		unordered_map<long long, vector<pair<int,int>>>::iterator it;
		it = hashtable.find(hash);
		if (it != hashtable.end())
		{
			hashtable[hash].push_back(pair<int,int>(ID, pos));
		}
		else
		{
			vector<pair<int,int>> tmp;
			tmp.push_back(pair<int,int>(ID, pos));
			hashtable.insert(pair<long long,vector<pair<int,int>>>(hash,tmp));
		}
	}

	void printhash()
	{
		for (auto p1: hashtable)
		{
			cout<<p1.first<<'\n';
			for (int i =0; i<p1.second.size(); i++)
			{
				cout<<"-----"<<p1.second[i].first<<' '<<p1.second[i].second<<'\n';
			}
		}
	}




	void SimpleFinder(string str)
	{
		long long hash=0;
		for (int i=0; i<GH.len; i++)
		{
			hash+=((int)str[i]-(int)'A'+1)*GH.p[i];
		}

		try
		{
			vector<pair<int,int>>tmp = hashtable[hash];
			for (int i=0; i<tmp.size(); i++)
			{
				cout<<tmp[i].first<<'.'<<tmp[i].second<<'\n';
			}
		}
		catch(...)
		{
			cout<<"oops!";
		}
	}

	void NotSimpleFinderSNAP(string str)
	{
		vector<long long> hash_seeds;
		GH.ReInit(-2,str);

		for (int i=0; i<str.length(); i++)
		{
			int res = GH.Next(str[i]);
			if (res != -1)
			{
				hash_seeds.push_back(GH.currhash);
			}
		}



		vector<vector<pair<int,int>>> hit_place = vector<vector<pair<int,int>>>(hash_seeds.size());

		for(int i=0; i<hash_seeds.size(); i++)
		{
			{
			    vector<pair<int,int>> tmp = hashtable[hash_seeds[i]];
				hit_place[i] = hashtable[hash_seeds[i]];
			}
		}


	/*	for(int i=0; i<hit_place.size(); i++)
		{ 
			cout<<hash_seeds[i]<<'\t';
			for(int j =0; j<hit_place[i].size(); j++)
			{
				cout<<hit_place[i][j].first<<'.'<<hit_place[i][j].second<<'\t';
			}
			cout<<'\n';
		}*/

		vector<vector<PoAl>>  map_blocks(str.length());
		
		AlignStepOne(hit_place,map_blocks);

		AlignStepTwo(map_blocks,str);

	}

	void AlignStepOne(vector<vector<pair<int,int>>> hit_place, vector<vector<PoAl>>  &map_blocks)
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
	}

	void AlignStepTwo(vector<vector<PoAl>>  map_blocks, string str)
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
	}
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

	
	//string ref3 = loadfasta("C:/Users/Sergey/Documents/Visual Studio 2012/Projects/gPBWTAligner/Debug/seq2.fasta");
	//vector<tuple<int,int,string>> VCF = loadVCF("C:/Users/Sergey/Documents/Visual Studio 2012/Projects/gPBWTAligner/Debug/variants.vcf");
	//string test = "ACGTAACCGGTTAAACCCGGGTTTACGTAACCGGTTAAACCCGGGTTT";
	clock_t t1 = clock();
	string ref3 = loadfasta("C:/Users/Sergey/Documents/Visual Studio 2012/Projects/gPBWTAligner/Debug/seq2.fasta");
	cout<<endl<<"load fasta file:"<<clock()-t1<<endl;
	t1 = clock();
	vector<tuple<int,int,string>> VCF = genVCF(ref3,1000,5);
	cout<<"gen variation:"<<clock()-t1<<endl;
	Graph  testGraph;
	t1 = clock();
	testGraph.LoadReference(ref3);
	cout<<"load reference:"<<clock()-t1<<endl;
/*	vector<tuple<int,int,string>> VCF;
	VCF.push_back(tuple<int,int,string>(4,5,"T"));
	VCF.push_back(tuple<int,int,string>(7,8,"T"));
	VCF.push_back(tuple<int,int,string>(10,20,"ATATATATAT"));
	VCF.push_back(tuple<int,int,string>(34,40,"CCCCCCC"));*/
	t1 = clock();
	testGraph.LoadVariance(0,1,VCF);
	cout<<"load variance:"<<clock()-t1<<endl;
//	testGraph.RecurcivePrint();

//	testGraph.print();

	string example = ref3.substr(10000,1000);

	t1 = clock();
	testGraph.GenHash(5,15);
	cout<<"generating hash:"<<clock()-t1<<endl;
//	testGraph.printhash();
	//testGraph.NotSimpleFinder("AACCGG");
	t1 = clock();
	testGraph.NotSimpleFinderSNAP(example);
	cout<<"SNAP:"<<clock()-t1<<endl;

	system("pause");
	
}