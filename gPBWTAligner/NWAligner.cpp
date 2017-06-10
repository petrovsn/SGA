#include <iostream>
#include <stdio.h>
#include <vector>
#include <string>
using namespace std;


int s(char c1, char c2)
{
	if (c1 == c2)
	{
		return 1;
	}
	else 
	{
		return -1;
	}
}

int n_max(int a, int b, int c)
{
	int max1 = a;
	if (b>max1) max1=b;
	if (c>max1) max1=c;
	return max1;
}

int n_max_arr(int a, int b, int c)
{
	int max1 = a;
	if (b>=max1) max1=b;
	if (c>max1) max1=c;
	
	if (max1 == a) return 1;
	if (max1 == b) return 2;
	if (max1 == c) return 3;
}



vector<int> trace(vector<vector<int>> matr, int i_f, int j_f)
{
	vector<int> path(matr.size()+matr[0].size());
	for (int i =0; i<path.size(); i++)
	{
		path[i]=0;
	}
	int k = 0;
	int i0 = i_f;
	int j0 = j_f;
	while ((j0 != 1)&&(i0!=1))
	{
		int arr = n_max_arr(matr[i0-1][j0], matr[i0-1][j0-1], matr[i0][j0-1]);
		if (arr==1) 
			{i0--;}
		if (arr==2) 
			{j0--;
			 i0--;}
		if(arr==3) 
			{j0--;}
		path[k] = arr;
		k++;
		if (j0==1)
		{
			for (int i = 1; i<i0;i++)
			{
				path[k]=1;
				k++;
			}
			break;
		}

		if (i0==1)
		{
			for (int i = 1; i<j0;i++)
			{
				path[k]=3;
				k++;
			}
			break;
		}
	}


	/*for (int i=0; i<k; i++)
	{
		cout<<path[i]<<' ';
	}*/
	return path;
}




int NWAlign(string seq1, string seq2)
{
	int wgt = 1;
	
	vector<vector<int>> matr(seq1.size()+1);

	for (int i=0; i<matr.size(); i++)
	{
		matr[i] = vector<int>(seq2.size()+1);
	}

	matr[0][0]=0;
	for (int i=1; i<matr.size(); i++)
	{
		matr[i][0] = matr[i-1][0]-1;
	}
	for (int i=1; i<matr[0].size(); i++)
	{
		matr[0][i] = matr[0][i-1]-1;
	}


	for(int i = 1; i<matr.size(); i++)
	{
		for (int j=1; j<matr[i].size(); j++)
		{
			matr[i][j]= n_max(matr[i-1][j]-wgt,matr[i][j-1]-wgt,matr[i-1][j-1]+s(seq1[i-1],seq2[j-1]));
		}
	}

	/*cout << '\t'<<'\t';
	for (int i=0; i<seq1.size();i++)

	{
		cout<<seq1[i]<<'\t';
	}
	cout<<'\n';
	for (int j=0; j<matr[0].size(); j++)
	{
		if (j>0) {cout<<seq2[j-1];	}
		cout<<'\t';
			
	for(int i = 0; i<matr.size(); i++)
		{
			cout<<matr[i][j]<<'\t';
		}
		cout<<'\n';
	}*/


	system("pause");
	vector<int> btrace = trace(matr, matr.size()-1, matr[0].size()-1);

	cout<<'\n';
	string s1 = "";
	string s2 = "";
	int k1 = 0;
	int k2 = 0;
	for (int i = btrace.size()-1; i>=0; i--)
	{
		if (btrace[i] != 0)
		{
			switch (btrace[i])
			{
				case 1://horizontal
				{
					s1 = s1+seq1[k1];
					k1++;
					s2 = s2+'-';
				}
				break;
				case 2: //diag
				{
					s1 = s1+seq1[k1];
					k1++;
					s2 = s2+seq2[k2];
					k2++;
				}
				break;
				case 3: //vert
				{
					
					s1 = s1+'-';
					s2 = s2+seq2[k2];
					k2++;
				}
				break;
			default:
				break;
			}
		}
	}
     
	int dis = 0;
	for (int i=0; i<s1.size(); i++)
	{
		if (s1[i] == '-') dis++;
	}
	cout<<"dis1_c="<<dis<<endl;

	dis = 0;
	for (int i=0; i<s2.size(); i++)
	{
		if (s2[i] == '-') dis++;
	}
	cout<<"dis2_c="<<dis<<endl;

return 0;
}
