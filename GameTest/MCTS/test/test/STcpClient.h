//
//	Copyright c 2019 by Phillip Chang
//
#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib,"Ws2_32.lib")

#include <cstdio>
#include <vector>

SOCKET socketServer = INVALID_SOCKET;
const char *infoServer[] = { "localhost", "8888" };
/*
	�бN idTeam �令�էO
*/
int idTeam = 29;

class _WSAData {
public:
	_WSAData() {
		if (WSAStartup(MAKEWORD(2, 2), &wsadata_) != 0) {
			printf("WSAStartup Fail %d", WSAGetLastError());
		}
	}
	~_WSAData() {
		WSACleanup();
	}
private:
	WSADATA wsadata_;
}initializerWSA;

SOCKET _CreateConnectSocket(const char* strIp, const char* strPort) {
	addrinfo addrinfoHint, *listDestAddrinfo = NULL, *addrinfoIterator = NULL;
	ZeroMemory(&addrinfoHint, sizeof(addrinfo));
	addrinfoHint.ai_family = AF_INET;
	addrinfoHint.ai_socktype = SOCK_STREAM;
	addrinfoHint.ai_protocol = IPPROTO_TCP;
	int errorApi = getaddrinfo(strIp, strPort, &addrinfoHint, &listDestAddrinfo);
	if (errorApi != 0) return INVALID_SOCKET;
	//	create socket
	SOCKET socketResult = socket(listDestAddrinfo->ai_family, listDestAddrinfo->ai_socktype, listDestAddrinfo->ai_protocol);
	if (socketResult == INVALID_SOCKET) {
		freeaddrinfo(listDestAddrinfo);
		return INVALID_SOCKET;
	}
	//	try connect
	errorApi = SOCKET_ERROR;
	addrinfoIterator = listDestAddrinfo;
	while (addrinfoIterator != NULL) {
		errorApi = connect(socketResult, addrinfoIterator->ai_addr, (int)addrinfoIterator->ai_addrlen);
		if (errorApi != SOCKET_ERROR) break;
		addrinfoIterator = addrinfoIterator->ai_next;
	}
	freeaddrinfo(listDestAddrinfo);
	if (errorApi == SOCKET_ERROR) {
		closesocket(socketResult);
		socketResult = INVALID_SOCKET;
	}
	return socketResult;
}
bool _SendToSocket(SOCKET socketDest, size_t lengthSend, const BYTE* rbSend) {
	return send(socketDest, (const char*)rbSend, (int)lengthSend, 0) == lengthSend;
}
bool _RecvFromSocket(SOCKET socketSrc, size_t lengthRecv, BYTE* rbRecv) {
	return recv(socketSrc, (char*)rbRecv, (int)lengthRecv, MSG_WAITALL) == lengthRecv;
}

void _ConnectToServer(int cntRecursive = 0) {
	if (cntRecursive > 3) {
		printf("[Error] : maximum connection try reached!\n");
		return;
	}
	while (socketServer == INVALID_SOCKET)
		socketServer = _CreateConnectSocket(infoServer[0], infoServer[1]);

	if (!_SendToSocket(socketServer, sizeof(int), (BYTE*)&idTeam)) {
		closesocket(socketServer);
		socketServer = INVALID_SOCKET;
		_ConnectToServer(cntRecursive + 1);
	}
}

void _ReconnectToServer() {
	if (socketServer != INVALID_SOCKET) {
		closesocket(socketServer);
		socketServer = INVALID_SOCKET;
	}

	_ConnectToServer();
}

/*
	���o��e�C�����A

	return (stop_program), (id_package, board, is_black)
	stop_program : True ��ܷ�e���ߧY�����{���AFalse ��ܷ�e����ۤv�U��
	id_package : ��e�ѽL���A�� id�A�^�ǲ��ʰT���ɻݭn�ϥ�
	board : ��e�ѽL���A board[i][j] ��ܴѽL�� i row, j column �W�\���Ѥl�A0 ��ܨS���Ѥl�B1 ��ܶ¤l�B2 ��ܥդl
	is_black : true ��ܷ�e�O����¤l�Aclient �u���b����ۤv���ɭԤ~�|����C�����A�T��
*/
bool GetBoard(int& id_package, std::vector<std::vector<int> >& board, bool& is_black) {
	if (socketServer == INVALID_SOCKET) {
		_ConnectToServer();
		if (socketServer == INVALID_SOCKET) {
			return true;
		}
	}
	BYTE rbHeader[8];
	if (!_RecvFromSocket(socketServer, 8, rbHeader)) {
		printf("[Error] : Connection lose, trying to reconnect...\n");
		closesocket(socketServer);
		socketServer = INVALID_SOCKET;
		return GetBoard(id_package, board, is_black);
	}
	int codeHeader = *((int*)rbHeader);
	id_package = *((int*)(rbHeader + 4));
	if (codeHeader == 0) return true;

	board.resize(8);
	for (int i = 0; i < 8; ++i) {
		board[i].resize(8);
		for (int j = 0; j < 8; ++j) {
			if (!_RecvFromSocket(socketServer, 4, (BYTE*)&(board[i][j]))) {
				printf("[Error] : Connection lose, trying to reconnect...\n");
				closesocket(socketServer);
				socketServer = INVALID_SOCKET;
				return GetBoard(id_package, board, is_black);
			}
		}
	}

	BYTE rbBlack[4];
	if (!_RecvFromSocket(socketServer, 4, rbBlack)) {
		printf("[Error] : Connection lose, trying to reconnect...\n");
		closesocket(socketServer);
		socketServer = INVALID_SOCKET;
		return GetBoard(id_package, board, is_black);
	}
	is_black = (*((int*)rbBlack)) == 1;

	return false;
}
/*
	�V server �ǹF���ʰT��
	id_package : �Q�n�^�_���T���� id_package
	listStep = <r, c>
	r, c ��ܭn���ʪ��Ѥl�y�� (row, column) (zero-base)
*/
void SendStep(int id_package, std::vector<int>& Step) {
	if (socketServer == INVALID_SOCKET) {
		printf("[Error] : trying to send step before connection is established\n");
		return;
	}

	std::vector<BYTE> rbData;
	rbData.resize(16);
	int* rbInsert = (int*)&(rbData[0]);
	*rbInsert++ = 1;
	*rbInsert++ = id_package;
	*rbInsert++ = Step[0];
	*rbInsert++ = Step[1];

	if (!_SendToSocket(socketServer, rbData.size(), &(rbData[0]))) {
		printf("[Error] : Connection lose, trying to reconnect\n");
		_ReconnectToServer();
	}
}

