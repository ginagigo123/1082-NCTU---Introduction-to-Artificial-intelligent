#include<iostream>
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<math.h>
#include<iomanip>
#include<vector>


using namespace std;

/*
	author : ���ɧg 0613413 
	content : Minesweeper solution 
			using propositional logic
*/

// mine Map Small / Medium / Big
int MAP[16][30]={0}, R, C, num_mines ;
int GameMAP[16][30];

struct point
{
	int x,y;
	int hint;
	bool mine;
}dir[8];

vector<point>hint; // ���Y�����u����m 
//vector<point>safe;

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
	bool flag = false;
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
	return true;
	/*���ˬd�Ÿ��O�_�۲� �]���~����P�]���Ψ�o�� 
	if(element[index].sign == B.element[i].sign)
	*/
}

clause clause::operator+(const clause &B)
{
	clause tmp;
	// �� this ��cell ���� tmp 
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

 // �d�� complementary �X�ӥy�l 
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

// ��this�O�_entail B ����Y�ԡH 
bool clause::entail(const clause &B)
{
	if(element.size()==0) return false;
	if(element.size() < B.element.size()){
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
	//cout<<endl;
}


vector<clause> KB;
vector<clause> KB0;

void initial()
{
	dir[0]={-1,-1};
	dir[1]={-1,0};
	dir[2]={-1,1};
	dir[3]={0,-1};
	dir[4]={0,1};
	dir[5]={1,-1};
	dir[6]={1,0};
	dir[7]={1,1};
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

bool isVaild(int i,int j)
{
	//if(MAP[i][j] == -1) return false;
	return ( (i >= 0 && i < R ) && (j >= 0 && j < C) );
}

// ----------- control mode start ------------
// - create map
void mode(int mode)
{
	int mine = 0;
	// control
	srand( time(NULL) );
	if(mode == 1){
		R = 9; C = 9;
		num_mines = 10;
		while( mine < num_mines){
			int x = rand()%R;
			int y = rand()%C;
			if(MAP[x][y] == 0){
				MAP[x][y] = -1; // mean mines
				mine++;
				point h = {x,y,MAP[x][y],true};
				hint.push_back(h);
			}
		}
	}
	else if(mode == 2){
		R = 16; C = 16;
		num_mines = 25;
		while( mine < num_mines){
			int x = rand()%R;
			int y = rand()%C;
			if(MAP[x][y] == 0){
				MAP[x][y] = -1; // mean mines
				mine++;
				point h = {x,y,MAP[x][y],true};
				hint.push_back(h);
			}
		}
	}
	else{
		R = 16; C =30;
		num_mines = 99;
		while( mine < num_mines){
			int x = rand()%R;
			int y = rand()%C;
			if(MAP[x][y] == 0){
				MAP[x][y] = -1; // mean mines
				mine++;
				point h = {x,y,MAP[x][y],true};
				hint.push_back(h);
			}
		}
	}
	
	// game
	for(int i = 0 ; i < R ; i ++)
		for(int j = 0 ; j < C ; j ++)
			GameMAP[i][j] = -1;
}

void fillin(vector<point> hhint)
{
	// �h�� Hint �P�򪺭� 
	for(int i = 0 ; i < hhint.size() ; i ++){
		// check hint ���K�� 
		for(int j = 0 ; j < 8 ; j ++){
			int px = hhint[i].x + dir[j].x;
			int py = hhint[i].y + dir[j].y;
			if(isVaild(px , py) && MAP[px][py] != -1){
				MAP[px][py] += 1;
			} 
		}
	}
}

void put_safe_cell(int safe)
{
	// game
	for(int i = 0 ; i < R ; i ++)
		for(int j = 0 ; j < C ; j ++)
			GameMAP[i][j] = -1;
	KB.clear();
	KB0.clear();
	// *** �h input initial safe cells �i�z�L�h�ܳo�̥h���R 
	int i = 0;
	while(i < safe){
		int x = rand()%R , y = rand()%C;
		// ���O���u
		if(MAP[x][y] != -1){
			cell s ={x,y,MAP[x][y],-1,false};
			clause tmp;
			tmp.element.push_back(s);
			if(!in(KB,tmp)){
				KB.push_back(tmp);
				i++;
				//cout<<"x:"<<x<<" y:"<<y<<endl;
			}
		}
	}	
	
}

// ----------- control mode end ------------

// ----------- game mode start ------------

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
/* using for dubug
/*cout<<"\n-------- case1 KB = now ------\n";
					cout<<"KB:";
					KB[j].print();
					cout<<"now";
					now.print();
					cout<<endl;
					
					cout<<"\n-------- case2 KB entail now ------\n";
					cout<<"KB:";
					KB[j].print();
					cout<<"now";
					now.print();
					cout<<endl;	


cout<<"\n-------- case3 Now entail KB ------";
					cout<<"\nKB:";
					KB[j].print();
					cout<<"\nnow";
					now.print();
					cout<<endl;
					//cout<<"KB(After):";
					//KB[j].print();
					
					// �A push ���e�N�n�ˬd�� �o�� cell���S�� in KB0 ���Y�I 
        	// �p�G������ �N��������ҥH���[�i�h 
        	// do resolution of the new clause with all the clause in KB0
			/*for(int j = 0 ; j < KB0.size() ; j ++){
				if(KB0[j].element[0] == local[i] ){
					if(KB0[j].element[0].sign == - local[i].sign){
							flag = true;
							cout<<"X"; 
					} 
				}
			}
*/


void single_all_in_clause(vector<clause> A)
{
	for(int i = 0 ; i < A.size() ; i ++){
		if(A[i].getn() == 1)
			cout<<A[i].element[0].sign<<"("<<A[i].element[0].x<<","<<A[i].element[0].y<<")";
	}
	cout<<endl;
}

void all_in_clause(vector<clause> A)
{
	for(int i = 0 ; i < A.size() ; i ++){
		for(int j = 0 ; j < A[i].getn(); j++)
			cout<<A[i].element[j].sign<<"("<<A[i].element[j].x<<","<<A[i].element[j].y<<")";
		cout<<endl;
	}
}

void pretty_print(int m, vector<cell> local, int combo, int sign)
{
	clause now;
    for (int i = 0; i < m; ++i) {
        if ((combo >> i) & 1){
        	if(sign == 1) { local[i].sign = 1;  }  //cout<<"+";
        	else if(sign == -1) { local[i].sign = -1; }   //cout<<"-";
        	// �����ˬd KB0 �]�� KB0 = true���ܦ��N�Q���ȤF 
			now.element.push_back(local[i]);
			//cout<<"("<< local[i].x <<","<<local[i].y<<")"<< ' ';
		}
		if( i == m-1){
			if(!check_dub_sub(now))
				KB.push_back(now);
		}
    }
	//cout << endl;
}

void combination(int m, int n, vector<cell>local, int sign)
{
    int combo = (1 << n) - 1;       // k bit sets
    while (combo < 1<<m) {
        pretty_print(m,local,combo,sign);

        int x = combo & -combo;
        int y = combo + x;
        int z = (combo & ~y);
        combo = z / x;
        combo >>= 1;
        combo |= y;
    }
}

int go_single=0,go_nonsingle=0;
void game_mode()
{
	int total_mines = 0;
	int stucktime = 0;
	//num_mines
	while(total_mines <= num_mines){
		// if there's a single-lateral clause
		int i;
		bool single = false;
		/*
		cout<<"KB:";
		all_in_clause(KB);
		cout<<"KB0:";
		single_all_in_clause(KB0);
		cout<<endl;*/
		for( i = 0 ; i < KB.size() ; i ++){
			if(KB[i].getn() == 1){
				single = true; go_single++;
				clause current = KB[i];
				int cx = current.element[0].x;
				int cy = current.element[0].y;
				if(current.element[0].sign == -1){
					current.element[0].mine = false; // �S���a�p
					current.element[0].hint = MAP[cx][cy];
					GameMAP[cx][cy] = MAP[cx][cy];  // ��hint��
				}
				else if (current.element[0].sign == 1){
					current.element[0].mine = true; // ���a�p 
					current.element[0].hint = MAP[cx][cy];
					GameMAP[cx][cy] = 666; //means ���a�p
					total_mines++;
				}
				KB.erase(KB.begin()+i);
				//i--;
				KB0.push_back(current);
				// matching �sclause�P�_���� & ��L��KB�y�l ��� 
				for(int a = 0 ; a < KB.size(); a ++){
					if( current.entail(KB[a]) || current == KB[a] ){ 
						// �R���o�� KB[a]
						KB.erase(KB.begin()+a);
						a--;
						continue;
					}
					for(int b = 0 ; b < KB[a].getn() ; b ++){
						if(KB[a].element[b] == current.element[0] && KB[a].getn()!= 1)
							// ��P 
							if(KB[a].element[b].sign == - current.element[0].sign){
								/*
								cout<<"-----"<<endl;
								cout<<"KB:"; KB[a].print();
								cout<<"current:"; current.print();*/
								KB[a].element.erase(KB[a].element.begin()+b);
								//cout<<"�R���������y�l"<<endl;
								
								// �h�ˬd�s���y�l���S���b KB0������ �R�� 
								if( in(KB0,KB[a]) ){
									//cout<<"already in KB0";
									//KB[a].print(); cout<<"-----"<<endl; 
									KB.erase(KB.begin()+a);
									a--;
								}
								// �h�ˬd�s���y�l���S���bKB(index = a���e)�̦����ƪ��I(���n) 
								else if(in(KB,a,KB[a])){
									//cout<<"already in KB";
									//KB[a].print(); cout<<"-----"<<endl; 
									KB.erase(KB.begin()+a);
									a--;
								}
								//else if()
									//KB[a].print(); cout<<"-----"<<endl;
								break;
							}
							// entail
					}
					
				}
				// if the cell is safe
				if(!current.element[0].mine){
					// query game control
					cell now = current.element[0];
					vector<cell> todo;
					int local_mine = now.hint , unmarked = 0;
					// �]�F�~ �O�_�����u �B marked  
					for(int a = 0 ; a < 8 ; a ++){
						int tmpx = now.x + dir[a].x ;
						int tmpy = now.y + dir[a].y;
						if(isVaild(tmpx, tmpy)){
							if(GameMAP[tmpx][tmpy] == 666){ // means���a�p 
								local_mine--;
							} 
							if(GameMAP[tmpx][tmpy] == -1){ // unmarked
								cell tmp = {tmpx,tmpy};
								unmarked ++;
								todo.push_back(tmp);
							}
						}
					}
					//cout<<"local_mine:"<<local_mine<<" unmarked:"<<unmarked<<endl;
					//cout<<"center ("<<cx<<","<<cy<<") "<<now.hint<<" --->";
					// insert the clauses regarding its unmarked neighbor
					if(local_mine == 0 && unmarked == 0) continue;
					if(local_mine == unmarked && unmarked !=0){ // n = m
						//cout<<"positive:";
						while(!todo.empty()){
							clause positive;	
							cell po = todo.back();
							todo.pop_back();
							po.sign = 1;
							positive.element.push_back(po);
							if(!in(KB,positive) && !in(KB0,positive)){
								//cout<<"O";
								KB.push_back(positive);
							} 
							//else	cout<<"X";	
							//cout<<"+"<<"("<<po.x<<","<<po.y<<") ";
						}
						//cout<<endl;
					}
					if(local_mine == 0){ // n = 0
						//cout<<"negative:"<<endl;
						while(!todo.empty()){
							clause negative;	
							cell ne = todo.back();
							todo.pop_back();
							ne.sign = -1;
							negative.element.push_back(ne);
							if(!in(KB,negative) && !in(KB0,negative)){
								//cout<<"O";
								KB.push_back(negative);
							}
							//else cout<<"X";	
							//cout<<"-"<<"("<<ne.x<<","<<ne.y<<") ";
						}
						//cout<<endl;
					}
					if(unmarked > local_mine && local_mine > 0){ // m > n > 0
						combination(unmarked,unmarked-local_mine+1,todo,1);
						combination(unmarked,local_mine+1,todo,-1);
						//todo.clear();
					}
				}	
				break;
			}
		}
	//}
		
		// global constraints
		//if(go_single > R*C - round(sqrt(R*C))){
			//combination();
		//}
		bool make_single = false;
		if(!single && stucktime == 10)
			break;
		// �S��single-lateral clause 
		if(!single){
			cout<<"�h��h�X���o�I\n";
			go_nonsingle ++;
			int Before = KB.size();
			// pairwise "matching" of all clause �h���h 
			if(KB.size() == 0) break;
			for(int i = 0 ; i < KB.size()-1 ; i ++){
				for(int j = i + 1 ; j < KB.size() ; j ++){
					// cpm ��complementary �y�l���X�� 
					if(KB[i].cpm(KB[j]) == 1 && KB[i].getn() <= 2 && KB[j].getn() <= 2){
						clause tmp = KB[i] + KB[j];
						
						// ���h�� KB0 check ���S��
						for(int a = 0 ; a < tmp.element.size();a ++)
							for(int b = 0 ; b < KB0.size() ; b ++)
								if(KB0[b].element[0] == tmp.element[a] )
									if(KB0[b].element[0].sign == - tmp.element[a].sign){
										tmp.element.erase(tmp.element.begin()+a);
										a--;
										cout<<"Haha�Q��P�F\n";
									}
						if(tmp.element.size() == 1)
							make_single = true;
						// no duplication & subsumption
						if(tmp.element.size()!=0 && !check_dub_sub(tmp))
						{	
							//cout<<"----succesffuly upload new----\n";
							cout<<"clause1:"; KB[i].print();
							cout<<" clause2:"; KB[j].print();
							cout<<" new clause"; tmp.print();
							//cout<<"-------------\n";
							KB.push_back(tmp);
						}
					} 
				}
			}
			int After = KB.size();
			if(Before == After && make_single == false)
				stucktime ++;
		}
	}
	cout<<"--------------Last Time ----------------\n";
	cout<<"KB:";
	all_in_clause(KB);
	cout<<KB0.size()<<"."<<"KB0:";
	single_all_in_clause(KB0);
	cout<<endl;
	
	//cout<<"\nKB size"<<KB.size()<<endl;
	/*
	cout<<"------ assigned -----\n";
	cout<<"total:"<<KB0.size()<<endl;
	for(int i = 0 ; i < KB0.size(); i ++){
		for(int j = 0 ; j < KB0[i].getn(); j ++){
			cell now = KB0[i].element[j];
			cout<<now.sign<<"("<<now.x<<","<<now.y<<") :"<<now.hint;
		}
		cout<<endl;
	}*/
}


// ----------- game mode end ------------

void setColor(int color);

void print_map()
{
	for(int i = 0 ; i < R ;i ++){
		for(int j = 0 ; j < C ; j ++){
			if(MAP[i][j] == -1)
				cout<<"*"<<" ";
			else if(MAP[i][j] != 0)
				cout<<MAP[i][j]<<" ";
			else
				cout<<"  ";
		}
		cout<<endl;
	}
}

void print_GameMAP()
{
	for(int i = 0 ; i < R ;i ++){
		for(int j = 0 ; j < C ; j ++){
			if(GameMAP[i][j] == 666)
				cout<<setw(2)<<"*"<<" ";
			else if(GameMAP[i][j] != 0)
				cout<<setw(2)<<GameMAP[i][j]<<" ";
			else
				cout<<"   ";
		}
		cout<<endl;
	}
}


int main()
{
	int level;
	time_t start,end;
	double t;
	cout << "LeveL :1.Easy 2.Medium 3.Hard" << endl;
	cout << "plz enter level:";
	cin >> level;
	
	// control mode
	initial(); // put value to 8 direcction
	mode(level); // set mode
	fillin(hint); // fill in the neibor of hint
	
	int i = 0;
	//for(int i = 0; i < 10 ; i ++){
		cout<<"safe cell's num:" << round(sqrt(R*C))+i << endl;
		put_safe_cell(round(sqrt(R*C))+i);
		// game mode
		start = clock();
		game_mode();
		end = clock();
		t = ((double)(end-start))/CLOCKS_PER_SEC;
		printf("Time : %fs\n",t);		
	//}
	

	print_map(); // print it out
	cout<<endl;
	print_GameMAP();
	cout<<endl;
	cout<<"single�P�_:"<<go_single<<" nonSingle�P�_:"<<go_nonsingle<<endl; 
	return 0;
}

// ����u�C�� 
void setColor(int color){
    HANDLE hConsole;
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole,color);
}

/*
	// safe node 
	for(int i = 0 ; i < s.size() ; i ++){
		if( s[i].hint == 0 ){
			// �o�a��S�����u �P�򳣶� 0 
			for(int i = 0; i < 8 ; i ++){
				int tmpx = s[i].x + dir[i].x;
				int tmpy = s[i].y + dir[i].y;
				if(isVaild( tmpx,tmpy ) ){
					point a = {tmpx,tmpy};
					vector<point>row;
						
				}
			}
		}
	}*/

