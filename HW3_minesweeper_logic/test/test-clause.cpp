#include<iostream>
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<vector>
#include<algorithm>

using namespace std;

struct cell
{
	int x,y;
	int hint;
	int sign; // �ݬO-A �٬O A
	bool mine;
};

bool operator == (const cell &p1,const cell &p2)
{
	if(p1.x == p2.x && p1.y == p2.y)
		return true;
	else 
		return false;
}



class clause
{
	public:
		vector<cell>element;
		int getn();
		clause operator+(const clause&);
		bool operator==(const clause&);
		bool entail(const clause&);
		int cpm(const clause&);
		void print();
};

int clause::getn()
{
	return element.size();
}

bool clause::operator==(const clause &B)
{
	if(element.size() != B.element.size())return false;
	int index = 0,i;
	while( index < element.size() ){
		for(i = 0 ; i < B.element.size() ; i ++){
			if(element[index].x == B.element[i].x && element[index].y == B.element[i].y)
				break;
		}	
		if(i == B.element.size()) // �N��]���F���٨S��� 
			return false;
		index ++;
	}
	return true;
	/*���ˬd�Ÿ��O�_�۲� �]���~����P�]���Ψ�o�� 
	if(element[index].sign == B.element[i].sign)
	*/
}

clause clause::operator+(const clause &B)
{
	clause tmp;
	// ��this ��cell ���� tmp 
	int j,fi,fj;;
	bool flag;
	for(int i = 0 ; i < (*this).getn() ; i++){
		flag = false;
		for(j = 0 ; j < B.element.size() ; j ++){
			if( (*this).element[i] == B.element[j] )
				if( (*this).element[i].sign == -B.element[j].sign ){
					fi = i; fj = j;
					flag = true;
					break;
				}
		}
		if(flag)
			break;
	}
	
	for(int i = 0 ; i < (*this).getn(); i ++){
		if(i == fi) continue;
		tmp.element.push_back( (*this).element[i] );
	}
		
	for(int i = 0; i < B.element.size(); i++){
		if( i == fj) continue;
		tmp.element.push_back(B.element[i]);
	}
	// �p�G�y�l���� +A,+A - >> �R���䤤�@�� 
	if(tmp.getn() == 2 && tmp.element[0] == tmp.element[1])
		tmp.element.erase(tmp.element.begin()+1);
	return tmp;
}



int clause::cpm(const clause &B)
{
	int count = 0;
	int index = 0,i;
	while( index < element.size() ){
		for(i = 0 ; i < B.element.size() ; i ++){
			if(element[index].x == B.element[i].x && element[index].y == B.element[i].y)
				if(element[index].sign == -B.element[i].sign)
					count++;
		}	
		index ++;
	}
	return count;
}

bool clause::entail(const clause &B)
{
	if(element.size() < B.element.size()){
		bool conatin;
		int index = 0,i;
		while( index < element.size() ){
			for(i = 0 ; i < B.element.size() ; i ++){
				if(element[index].x == B.element[i].x && element[index].y == B.element[i].y)
					if(element[index].sign == B.element[i].sign)		
						break;
			}	
			if(i == B.element.size()) // �N��]���F���٨S���
				return false;
			index ++;
		}
		// �p�G this���Ӫ��״N����p �Bin B���Y������쪺�� �N��  this entail B
		return true;
	}
	return false;
}

void clause::print()
{
	for(int i = 0 ; i < element.size(); i ++){
		if(element[i].sign == -1)
			cout<<"-";
		else if(element[i].sign == 1)
			cout<<"+";
		cout<<"("<<element[i].x<<","<<element[i].y<<") ";
	}
	cout<<endl;
}


void pretty_print(int m, int n, vector<cell> local, int combo, int sign)
{
    for (int i = 0; i < m; ++i) {
        if ((combo >> i) & 1)
            cout<< sign <<"("<< local[i].x <<","<<local[i].y<<")"<< ' ';
    }
    cout << endl;
}

void combination(int m, int n, vector<cell>local, int sign)
{
    int combo = (1 << n) - 1;       // k bit sets
    while (combo < 1<<m) {
        pretty_print(m, n, local, combo,sign);

        int x = combo & -combo;
        int y = combo + x;
        int z = (combo & ~y);
        combo = z / x;
        combo >>= 1;
        combo |= y;
    }
}

bool in(vector<clause> K,clause c)
{
	for(int i = 0 ; i < K.size() ; i ++){
		if(K[i] == c)
			return true;
	}
	return false;
}

bool in(vector<clause> K,int index,clause c)
{
	// �ݥ����e�����S���ۦP�� 
	for(int i = 0 ; i < index ; i ++){
		if(K[i] == c)
			return true;
	}
	return false;
}

void all_in_clause(vector<clause> A)
{
	for(int i = 0 ; i < A.size() ; i ++){
		for(int j = 0 ; j < A[i].getn(); j++)
			cout<<A[i].element[j].sign<<"("<<A[i].element[j].x<<","<<A[i].element[j].y<<")";
		cout<<endl;
	}
}

vector<clause> KB;

bool check_dub_sub(clause now)
{
	// �Ĥ@���P�_ for case 3 
	bool first = true;
	for(int j = 0 ; j < KB.size() ; j++){
		//case 1 �ˬd KB�̭��|���|��same���H same�N���L
		if(KB[j] == now){
			return true;
		}
		//case 2�ˬd subsumption
		if(KB[j].entail(now)){
			return true;
		}
		//case 3�h�ݻ� B�O�_entail this �p�G�OTrue �쥻���n�R�� �s�W�o�Ӷi�h 
		if(now.entail(KB[j])){
			if(first){  //�� �@���J�� assign�������N�쥻��
				KB[j] = now; 
				first = false;	
			}
			else{
				KB.erase(KB.begin()+j);
				j--;
			}
		}
	}
	if(!first)
		return true;
	return false;
} 

int main()
{
	clause A;
	vector<cell> todo;
	
	cell a = {3,11,0,-1};
	cell b = {2,14,0,1};
	//cell c = {1,3,0,-1};
	//cell d = {1,4,0,-1};
	//cell e = {1,5,0,-1};
	todo.push_back(a);
	todo.push_back(b);
	//todo.push_back(c);
	//todo.push_back(d);
	//todo.push_back(e);
	
	A.element = todo;
	KB.push_back(A);
	
	clause B;
	//cell f = {1,9,0,-1};
	//cell g = {1,6,0,-1};
	//cell h = {1,7,0,-1};
	cell i = {2,14,0,1};
	cell j = {3,11,0,1};
	//B.element.push_back(f);
	//B.element.push_back(g);
	//B.element.push_back(h);
	B.element.push_back(i);
	B.element.push_back(j);
	
	cout<<(B == A)<<endl;
	KB.push_back(B);
	//combination(5,4,todo,1);
	//combination(5,3,todo,-1);
	
	for(int i = 0 ; i < KB.size()-1 ; i ++){
		for(int j = i + 1 ; j < KB.size() ; j ++){
			if(KB[i].getn() <= 2 && KB[j].getn() <= 2){
				cout<<i<<","<<j<<endl;
				cout<<"----\n";
				all_in_clause(KB);
				cout<<"----\n";
				cout<<"KB[i].cpm(KB[j]):"<<KB[i].cpm(KB[j])<<endl;
				if( KB[i].cpm(KB[j]) == 1 ) { // �ݦ��X��
					clause tmp = KB[i] + KB[j];
					cout<<"clause1:"; KB[i].print();
					cout<<"clause2:"; KB[j].print();
					cout<<"new clause:"; tmp.print();
					if(tmp.getn()!=0 && !check_dub_sub(tmp)) // no duplication
					{	cout<<"succesffuly upload new\n";
						KB.push_back(tmp);
					}
				}
			}
			cout<<endl;
		}
	}
	all_in_clause(KB);
	return 0;
}

