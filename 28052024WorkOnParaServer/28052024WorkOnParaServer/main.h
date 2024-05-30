#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <Windows.h>
#include <fcntl.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

using namespace std;

int main() {
    setlocale(LC_ALL, "Rus");
    SetConsoleCP(1251);

    WSADATA wsaData; //���������� � ������� Windows Socket
    ADDRINFO hints; //���� �������
    ADDRINFO* addrResult; //��������� ������ �������
    SOCKET ClientSocket = INVALID_SOCKET; //����� ��� ����������� �������
    SOCKET ListenSocket = INVALID_SOCKET; //����� ��� ������������� ��������� �������


    const char* sendBuffer = "Hello from server";
    char recvBuffer[512]; //����� ��� ������ (�������� � ���������)

    int result = WSAStartup(MAKEWORD(2, 2), &wsaData); //������/������������� ������ Windows Socket; MAKEWORD(2,2) - ���������� ������

    if (result != 0) { //�������� ���������� ������������� Windows Socket
        wcout << L"WSAStartup failed" << endl;
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints)); //�������� hints
    hints.ai_family = AF_INET; //����� IPv4
    hints.ai_socktype = SOCK_STREAM; //��� ������ - ���������  
    hints.ai_protocol = IPPROTO_TCP; //��������� �������� ��� ������ - TCP 
    hints.ai_flags = AI_PASSIVE; //��������� ��� ������ ������� ip �������

    result = getaddrinfo(NULL, "788", &hints, &addrResult); // addrResult ��������� � ���� �������� ���������� � ������� getaddrinfo

    if (result != 0) {
        wcout << L"getaddrinfo failed" << endl;
        return 1;
    }

    ListenSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol); //�������� ������ � ��������� ������� �� addrResult. 

    if (ListenSocket == INVALID_SOCKET) {
        wcout << L"socket creation with error" << endl;
        freeaddrinfo(addrResult);
        return 1;
    }

    result = bind(ListenSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen); //�������� ������ � ������ �� ����������; ��������� ������� -1)  �����, ������� ����� �������� � ������, 2) ��������� �� ��������� � �������, � �������� ����� �������� ����� 3) ������ ���������
    //�� ������� ��������� addrResult->ai_addr

    if (result == SOCKET_ERROR) {
        wcout << L"������ ����������� �������" << endl;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    result = listen(ListenSocket, SOMAXCONN); // ����� ��������� � ������� ��������� ��������� �� �������; SOMAXCONN - ������������ ���-�� ��������� ����������

    if (result == SOCKET_ERROR) {
        wcout << L"Listening failed" << endl;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    ClientSocket = accept(ListenSocket, NULL, NULL); // ��������� ���������� �������. ������ ����� ����� ��� ������ � ��������. NULL � addr � addrlen ��������, ��� ������ �� �������� ���������� � ��������, � ������ ������ ����� �����.

    if (result == SOCKET_ERROR) {
        wcout << L"shutdown error" << endl;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    closesocket(ListenSocket); // �������� ������ �������������

    do {
        ZeroMemory(recvBuffer, 512); //��������� ������ ����� ���������
        result = recv(ClientSocket, recvBuffer, 512, 0); //recv ��������� ������ �� ������� � ��������� �� � recvBuffer
        if (result > 0) {
            wcout << L"Send" << result << L"bite" << endl;
            wcout << L"Send" << recvBuffer << endl;

            result = send(ClientSocket, sendBuffer, (int)strlen(sendBuffer), MSG_DONTROUTE);
            if (result == SOCKET_ERROR) {
                cout << "send FAILED" << endl;
                freeaddrinfo(addrResult);
                WSACleanup();
                return 1;
            }
        }
        else if (result == 0) {
            wcout << L"Connection closed" << endl;
        }
        else {
            wcout << L"�� ���������� �������� ���������" << endl;
        }
    } while (result > 0);

    result = shutdown(ClientSocket, SD_SEND); //���������� �������� ��������� �� ������ ClientSocket
    if (result == SOCKET_ERROR) {
        cout << "send FAILED" << endl;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    };

    closesocket(ClientSocket); //�������� ������ �������
    freeaddrinfo(addrResult); // ����������� ������, ���������� ��� �������� ����������.
    WSACleanup(); //��������� ������������� Winsock.
    return 0;
}

