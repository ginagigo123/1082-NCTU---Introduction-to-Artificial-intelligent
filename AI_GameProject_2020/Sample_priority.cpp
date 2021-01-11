
#include "STcpClient.h"
#include <stdlib.h>
#include<math.h>
#include <iostream>
#include<vector>
#include <windows.h> // using GetTickCount
#include<queue>
#include<time.h>
#include<stack>
#define TIME 4
/*
    ���즹�{�����ʴѤl
    board : �ѽL���A(vector of vector), board[i][j] = i row, j column �ѽL���A(i, j �q 0 �}�l)
            0 = �šB1 = �¡B2 = �աB-1 = �|�Ө���
    is_black : True ��ܥ��{���O�¤l�BFalse ��ܬ��դl

    return Step
    Step : vector, Step = {r, c}
            r, c ��ܭn�U�Ѥl���y�Ц�m (row, column) (zero-base)
*/
typedef struct posnode{
	int x;
	int y;
	int utility;
	posnode(int x,int y,int u){
		this->x = x;
		this->y = y;
		this->utility = u;
	}
	posnode(int x,int y){
		this->x = x;
		this->y = y;
	}
	posnode(){
		x = y = -1;
		utility = 0;
	}
}point;

point dir[8];

bool operator<(const point &a,const point &b){
	return a.utility < b.utility;
}

bool operator>(const point &a,const point &b){
	return a.utility > b.utility;
}

typedef struct ScoreNode {
    int white;
    int black;
    ScoreNode(int white, int black) {
        this->white = white;
        this->black = black;
    }
    int get(bool is_black) {
        if (is_black == false) {
            return white;
        }
        else {
            return black;
        }
    }
}Score ;


int color(bool is_black){
	if(is_black)
		return 1;
	else 
		return 2;
}

// ------------------ MCTS
class MCTS{
	public:
		std::vector<MCTS*>child;
		MCTS* bestchild = NULL;
		int visit;
		double score;
		bool expandable;
		MCTS* father = NULL;
		bool root;
		point pos; // the corresponding position on the board
		bool is_black; // the corresponding chess tile on the board
		double C ; // arguments used when determine the best child, which can be adjusted
		float realscore;

		MCTS(point pos,bool is_black); // constructor
		void rootClear(); // function to initialize the dummy root
		double calculate(std::vector<std::vector<int> >&); // function to calculate for determining best child
		int bestChild(std::vector<std::vector<int> >&); // function to get the best child
};

void MCTS::rootClear(){
	while(child.empty() == false){
		MCTS* ele = child.back();
		child.pop_back();
		delete ele;
	}
	child.clear();
	visit = 0;
	bestchild = NULL;
	expandable = false;
	root = true;
}

MCTS::MCTS(point pos,bool is_black){
	bestchild = NULL;
	visit = 0;
	score = 0;
	C = 1.4;
	expandable = true;
	father = NULL;
	root = false;
	this->pos = pos;
	this->is_black = is_black;
}

double MCTS::calculate(std::vector<std::vector<int> > &board){
	int i = pos.x, j =pos.y; 
	if( visit == 0 )	return 1.5;
	double sum = this->score*1.0 / this->visit + this->C * sqrt(2 * log(this->father->visit / this->visit));
	if ( ( i == 0 || i == 7 ) && ( j == 1 || j == 6 ))
		sum *= 1.15;
	else if ( ( i == 1 || i == 6 ) && ( j == 0 || j == 7 ))
		sum *= 1.15;
	else if ( (i == 3 || i == 4) && (j == 0 || j == 7) )
		sum*= 1.1;
	else if ( ((i == 0 || i == 7) && (j == 3 || j == 4)))
		sum*= 1.1;
	// bad
	else if (((i == 1 || i == 6) && (j == 1 || j == 6))){
		if( i == 1 && j == 1)
			if(board[1][0] != color(is_black) || board[0][1] != color(is_black))
				sum*= 0.87;
		if( i == 1 && j == 6)
			if(board[1][7] != color(is_black) || board[0][6] != color(is_black))
				sum*= 0.87;
		if( i == 6 && j == 1)
			if(board[6][0] != color(is_black) || board[7][1] != color(is_black))
				sum*=0.87;
		if( i == 6 && j == 6)
			if(board[6][7] != color(is_black) || board[7][6] != color(is_black))
				sum*=0.87;
	}			
    else if ((( i == 2 || i == 5) && ( j == 0 || j == 7))){
    	if( i == 2 && j == 0)
    		if(board[1][0] != color(is_black))
    			sum*= 0.95;
    	if( i == 2 && j == 7)
    		if(board[1][7] != color(is_black))
    			sum*= 0.95;
		if( i == 5 && j == 0)
			if(board[6][0] != color(is_black))
    			sum*= 0.95;
		if( i == 5 && j == 7)
			if(board[6][7] != color(is_black))
    			sum*= 0.95;
	}
    else if ((( i == 0 || i == 7) && ( j == 2 || j == 5))){
    	if( i == 0 && j == 2)
    		if(board[0][1] != color(is_black))
    			sum*= 0.95;
		if( i == 0 && j == 5)
			if(board[0][6] != color(is_black))
	    			sum*= 0.95;
		if( i == 7 && j == 2)
			if(board[7][1] != color(is_black))
	    			sum*= 0.95;
		if( i == 7 && j == 5)
			if(board[7][6] != color(is_black))
	    			sum*= 0.95;
	}
	
	return sum;
}

int MCTS::bestChild(std::vector<std::vector<int> >&board){
	double max = child[0]->calculate(board);
	//std::cout << "("<< child[0]->pos.x <<"," << child[0]->pos.y << ") score:" << max << std::endl;
	int best = 0;
	for (int i = 1 ; i < child.size() ; i ++) {
		double tmp = child[i]->calculate(board);
		if(tmp > max){
			max = tmp;
			best = i;
		}
	}
	return best;
}

typedef struct ResultNode {
    MCTS* state;
    bool is_black;
    ResultNode(MCTS* state, bool is_black) {
        this->state = state;
        this->is_black = is_black;
    }
}Result;

// ------------------ Main Function 
void ini(){
	dir[0].x = -1; dir[0].y = -1;
	dir[1].x = -1; dir[1].y = 0;
	dir[2].x = -1; dir[2].y = 1;
	dir[3].x = 0; dir[3].y = -1;
	dir[4].x = 0; dir[4].y = 1;
	dir[5].x = 1; dir[5].y = -1;
	dir[6].x = 1; dir[6].y = 0;
	dir[7].x = 1; dir[7].y = 1;
}




void getBoardCopy(std::vector<std::vector<int> >&A,std::vector<std::vector<int> >&B){
	B.resize(8);
	for (int i = 0 ; i < 8 ; i ++) {
		B[i].resize(8);
		for (int j = 0 ; j < 8 ; j ++) {
			B[i][j] = A[i][j];
		}
	}
}

bool isValid(int x,int y){
	if( (x == 0 || x == 7) && (y == 0 || y == 7) )
		return false;
	return ((x >= 0 && x < 8 ) && (y >= 0 && y < 8));
}

// �� x , y �o�Ӧ�m�i���i�H�Q is_black�U 
bool isValidMove(std::vector<std::vector<int> >& board, bool is_black, int x, int y){
	// �w�g�Q�U�L�F 
	if(x == 8 || y == 8) return false;
	if (board[x][y] != 0) return false;
	// 6*6 ���Y�����i�H�U 
	if( ( x >= 1 && x <= 6 ) && (y >= 1 && y <= 6 ) )	return true;
	
	int oppo, me;
	if(is_black){
		oppo = 2; // ��� = �զ� 
		me = 1;
	}
	else{
		oppo = 1; // ��� = �¦� 
		me = 2;
	}
		
	// �ݭn�B�z�� 6*6 �H�~
	for (int i = 0 ; i < 8 ; i ++) {
		int cx = x + dir[i].x;
		int cy = y + dir[i].y;
		// �ˬd���S���W�� 
		if (!isValid(cx,cy))	continue;
		// �d�� 
		if ( board[cx][cy] == oppo ) {
			cx += dir[i].x;
			cy += dir[i].y;
			if (!isValid(cx,cy))	continue;
			if (board[cx][cy] == 0) // �S���ۤv�C��G�] 
				continue;
			while (board[cx][cy] == oppo){ // �ˬd��� 
				cx += dir[i].x;
				cy += dir[i].y;
				if (!isValid(cx,cy))	break;
				if(board[cx][cy] == 0) // ���S���H�� 
					break;
			}
			if (!isValid(cx,cy))	continue;
			if(board[cx][cy] == 0) 
				continue;
			if(board[cx][cy] == me){
				return true;
			}
		}
	}
	return false;
} 

std::vector<point> getValidMove(std::vector<std::vector<int> >& board, bool is_black,bool forpolicy = false){
	std::vector<point> validMoves;
	for (int i = 0; i < 8; i ++) {
		for ( int j = 0 ; j < 8 ; j ++){
			if( (i == 0 || i == 7) && (j == 0 || j == 7) )
				continue;
			if ( isValidMove(board,is_black,i,j) ) {
				if(forpolicy == true){
					// 8 �ӭ��n���� �̷��v����J 
					if ( ( i == 0 || i == 7 ) && ( j == 1 || j == 6 )){
	                    validMoves.push_back(point(i, j));
	                    validMoves.push_back(point(i, j));
	                }
	                if ( ( i == 1 || i == 6 ) && ( j == 0 || j == 7 )){
	                    validMoves.push_back(point(i, j));
	                    validMoves.push_back(point(i, j));
	                }
	                //  ��W���F�M�I�� => �Q�� 
	                if ( ((i == 3 || i == 4) && (j == 0 || j == 7)) ) {
	                    validMoves.push_back(point(i, j));
	                    validMoves.push_back(point(i, j));
	                }
	                if ( ((i == 0 || i == 7) && (j == 3 || j == 4))) {
	                    validMoves.push_back(point(i, j));
	                    validMoves.push_back(point(i, j));
	                }
	                // ���F�o�Ǫ����n�[�v�� �]���o�� bad :( 
	                if (!((i == 1 || i == 6) && (j == 1 || j == 6))) {
	                	if(! ( ( i == 2 || i == 5) && ( j == 0 || j == 7) ) )
	                		if(! ( ( i == 0 || i == 7) && ( j == 2 || j == 5) ) )
	                    		validMoves.push_back(point(i, j));
	                }
	                validMoves.push_back(point(i, j));
				}
				else{
					if ( ( i == 0 || i == 7 ) && ( j == 1 || j == 6 ))
						validMoves.push_back(point(i, j,100));
					else if ( ( i == 1 || i == 6 ) && ( j == 0 || j == 7 ))
						validMoves.push_back(point(i, j,100));
					else if ( (i == 3 || i == 4) && (j == 0 || j == 7) )
						validMoves.push_back(point(i, j,50));
					else if ( ((i == 0 || i == 7) && (j == 3 || j == 4))) 
						validMoves.push_back(point(i, j,50));
					// bad
					else if (((i == 1 || i == 6) && (j == 1 || j == 6)))
						validMoves.push_back(point(i, j,10));
	                else if ((( i == 2 || i == 5) && ( j == 0 || j == 7)))
	                	validMoves.push_back(point(i, j,10));
	                else if ((( i == 0 || i == 7) && ( j == 2 || j == 5)))
	                	validMoves.push_back(point(i, j,10));
	                else
	                	validMoves.push_back(point(i, j,20)); // normal
				} 
				//validMoves.push_back(point(i, j));
				//std::cout << "(" << i << "," << j << ")" << std::endl;
			}
		}
	}
	return validMoves;
}

std::priority_queue<point> getValidMove2(std::vector<std::vector<int> >& board, bool is_black,bool forpolicy = false){
	std::priority_queue<point> validMoves;
	for (int i = 0; i < 8; i ++) {
		for ( int j = 0 ; j < 8 ; j ++){
			if( (i == 0 || i == 7) && (j == 0 || j == 7) )
				continue;
			if ( isValidMove(board,is_black,i,j) ) {
				if ( ( i == 0 || i == 7 ) && ( j == 1 || j == 6 ))
					validMoves.push(point(i, j,100));
				else if ( ( i == 1 || i == 6 ) && ( j == 0 || j == 7 ))
					validMoves.push(point(i, j,100));
				else if ( (i == 3 || i == 4) && (j == 0 || j == 7) )
					validMoves.push(point(i, j,50));
				else if ( ((i == 0 || i == 7) && (j == 3 || j == 4))) 
					validMoves.push(point(i, j,50));
				// bad
				else if (((i == 1 || i == 6) && (j == 1 || j == 6)))
					validMoves.push(point(i, j,10));
                else if ((( i == 2 || i == 5) && ( j == 0 || j == 7)))
                	validMoves.push(point(i, j,10));
                else if ((( i == 0 || i == 7) && ( j == 2 || j == 5)))
                	validMoves.push(point(i, j,10));
                else
                	validMoves.push(point(i, j,20)); // normal
				
				//validMoves.push_back(point(i, j));
				//std::cout << "(" << i << "," << j << ")" << std::endl;
			}
		}
	}
	return validMoves;
}

bool isGameOver(std::vector<std::vector<int> >& board) {
    int black = 0;
    int white = 0;
    int flag = 0;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (board[i][j] == 2) {
                black++;
            }
            else if (board[i][j] == 1) {
                white++;
            }
            else {
                flag++;
            }
        }
    }

    if (black == 0 || white == 0) {
        return true;
    }

    if (flag != 0) {
        return false;
    }
    return true;
}

Score getScoreofBoard(std::vector<std::vector<int> >& board) {
    int white = 0;
    int black = 0;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (board[i][j] == 2) {
                white++;
            }
            if (board[i][j] == 1) {
                black++;
            }
        }
    }
    return Score(white, black);
}

bool makeMove(std::vector<std::vector<int> >& board,bool is_black ,int x ,int y){
	bool can = isValidMove(board,is_black,x,y);
	if (!can)	return false;
	
	int own = color(is_black);
	board[x][y] = own;
	
	/*
	while (toFlip.size()){ // �쥻��toFlip ����push�᪺�ȳ��O�@�˪� 
		point po = toflip.back();
		toflip.pop_back();
		board[po.x][po.y] = own;
	}*/
	return true;
}

ResultNode treePolicy(std::vector<std::vector<int> >& board,MCTS* state, bool is_black) {
	MCTS* expand = NULL;
    std::queue<MCTS*> q;
    q.push(state);
	while (!q.empty()) {
        MCTS* n = q.front();
        q.pop();
        // �S���Q Expand��  �⥦sample �[�W children
        if (n->expandable == false) {
            for (int i = 0; i < n->child.size(); i++) {
                q.push(n->child[i]);
            }
        }
        // �p�G���Qexpand -> ��@�ӨS���Qsampled�L�� 
        else {
            expand = n;
            break;
        }
    }
    int own = color(is_black);
    int oppo = 1;
    if(own == 1) oppo = 2;
    
    // �N�� state->expandable ���O false ���w�g�O leaf�F ? 
    if (expand == NULL) {
    	//std::cout<< "find the best child";
        expand = state->child[state->bestChild(board)];
        //std::cout <<"best " << expand->pos.x << "," << expand->pos.y << std::endl;
        makeMove(board, is_black, expand->pos.x, expand->pos.y);
        //std::cout << "make move !"<< std::endl;
        is_black = !is_black;
    }
    else {
    	//std::cout<<"expand gogo:"<<expand->pos.x<<","<<expand->pos.y<<std::endl;
        expand->expandable = false;
        std::stack<MCTS*> s;
        MCTS* node = expand;
        while (!node->root) {
            s.push(node);
            node = node->father;
        }
        while (!s.empty()) {
            node = s.top();
   			//std::cout<<"move:"<<node->pos.x<<","<<node->pos.y<<std::endl;
            makeMove(board, is_black, node->pos.x, node->pos.y);
            s.pop();
            is_black = !is_black;
        }
        std::priority_queue<point>nodes = getValidMove2(board, is_black);
        while(!nodes.empty()) {
        	point current = nodes.top();
        	nodes.pop();
            MCTS* mnode = new MCTS(current,!expand->is_black);
            mnode->father = expand;
            expand->child.push_back(mnode);
        }
    }
    return ResultNode(expand, is_black);;
}

Score defaultPolicy(std::vector<std::vector<int> >& board, MCTS* state, bool is_black) {
    int flag = 0;
    srand((unsigned)time(NULL));
    while (!isGameOver(board)) {
    	//std::cout<<"grab�i���"<<std::endl; 
        std::vector<point> nodes = getValidMove(board, is_black, true);
        //std::cout<<"size:"<<std::endl; 
		//std::cout << nodes.size() << std::endl;
        if (nodes.size() >= 1) {
            flag = 0;
            int random = rand() % (nodes.size());
            //std::cout<<"random" << random << std::endl;
            int x = nodes[random].x;
            int y = nodes[random].y;
            makeMove(board, is_black, x, y); // ���Y�|�ˬd�o�Ӧ��S���Q���L�I
            //std::cout<<"make move:(" << x << "," << y <<")";
            /*
			if(is_black)
            	std::cout<<"black"<< std::endl;
            else
            	std::cout<<"white"<< std::endl;*/
        }
        else {
            flag += 1;
            if (flag == 2) {
                //std::cout << "oooooooooooooooooo" << std::endl;
                return getScoreofBoard(board);
            }
        }
        is_black = !is_black;
    }
    return getScoreofBoard(board);
}

void backup(std::vector<std::vector<int> >& board, MCTS* bottom, bool is_black, bool player, Score score) {
    is_black = !(is_black);
    bool oppo = !(player);
    int deta = score.get(player) - score.get(oppo);

    if (deta >= 0) {
        deta = 1;
    }
    else if (deta < 0) {
        deta = 0;
    }

    MCTS* node = bottom;
    while (true) {
        node->score += deta;
        node->visit += 1;

        // test
        //if (node->child.size() > 0) {
        //	int tmp = -100000;
        //	for (int i = 0; i < node->child.size(); i++) {
        //		if (node->child[i]->score > tmp) {
        //			tmp = node->child[i]->score;
        //		}
        //	}
        //	node->score = tmp;
        //}

        if (node->root == true) {
            break;
        }

        node = node->father;
        is_black = !(is_black);
    }
}

std::vector<int> GetStep(std::vector<std::vector<int> >& board, bool is_black) {
	
	// Example:
	point start(8,8);
	MCTS* root = new MCTS(start,is_black); // ??
	root->rootClear();
	std::vector<int> step;
	step.resize(2);
	std::priority_queue<point> nodes = getValidMove2(board,is_black);
	//std::cout << "nodes:" << nodes.size() << std::endl;
	if ( nodes.size() == 0 )
		return step; // ??
	else if ( nodes.size() == 1){
		step[0] = nodes.top().x; step[1] = nodes.top().y;
		return step;
	}
	for (int i = 0; i < nodes.size(); i ++) {
		point current = nodes.top();
		nodes.pop();
		int x = current.x;
		int y = current.y;
		MCTS* mnodes =new MCTS(current,is_black);
		mnodes->father = root;
		root->child.push_back(mnodes);
		root->visit += mnodes->visit;
		//std::cout << x <<","<< y <<" u:"<< current.utility << std::endl;
	}
	double starttime = GetTickCount();
	int playtime = 0;
	while (true) {
		playtime ++;
		std::vector<std::vector<int> > dupBoard;
		//std::cout <<"----------"<< playtime <<"---------"<< std::endl;
		//std::cout << "Copy board" << std::endl;
		getBoardCopy(board,dupBoard);
		
		//std::cout << "Selection & Expansion" << std::endl;
		// selection & expansion
		Result result = treePolicy(dupBoard,root,is_black);
		//std::cout << "go::" << result.state->child.size() << std::endl;
		
		// simulation
		//std::cout << "Simulation" << std::endl;
		Score score = defaultPolicy(dupBoard, result.state, result.is_black);
		
		// back-propagation
		//std::cout << "Back-prpagation" << std::endl;
		backup(dupBoard, result.state, result.is_black, !is_black, score);
		
        double endtime = GetTickCount();
        if (endtime - starttime > TIME * 1000) {
            std::cout << playtime << std::endl;
            break;
        }		
	}
	int index = root->bestChild(board);
	step[0] = root->child[index]->pos.x;
	step[1] = root->child[index]->pos.y;
	std::cout << step[0] << "," << step[1] << std::endl;
	/*
	step[0] = rand() % (7 + 1 - 0) + 0;
	step[1] = rand() % (7 + 1 - 0) + 0;*/
	return step;
}

int main() {
	int id_package;
	std::vector<std::vector<int> > board;
	std::vector<int> step;
	
	ini(); // direction ���� 
	bool is_black;
	while (true) {
		if (GetBoard(id_package, board, is_black))
			break;

		step = GetStep(board, is_black);
		SendStep(id_package, step);
	}
}

