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

    WSADATA wsaData; //Информация о запуске Windows Socket
    ADDRINFO hints; //Типы адресов
    ADDRINFO* addrResult; //Результат поиска адресов
    SOCKET ClientSocket = INVALID_SOCKET; //Сокет для подключения клиента
    SOCKET ListenSocket = INVALID_SOCKET; //Сокет для прослушивания сообщений клиента


    const char* sendBuffer = "Hello from server";
    char recvBuffer[512]; //Буфер для данных (отправка и получение)

    int result = WSAStartup(MAKEWORD(2, 2), &wsaData); //Запуск/инициализация работы Windows Socket; MAKEWORD(2,2) - определяет версию

    if (result != 0) { //Проверка результата инициализации Windows Socket
        wcout << L"WSAStartup failed" << endl;
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints)); //Обнуляет hints
    hints.ai_family = AF_INET; //Задаёт IPv4
    hints.ai_socktype = SOCK_STREAM; //Тип сокета - потоковый  
    hints.ai_protocol = IPPROTO_TCP; //Указывает протокол для сокета - TCP 
    hints.ai_flags = AI_PASSIVE; //Указывате при работе текущий ip сервера

    result = getaddrinfo(NULL, "788", &hints, &addrResult); // addrResult принимает в себя адресную информацию с помощью getaddrinfo

    if (result != 0) {
        wcout << L"getaddrinfo failed" << endl;
        return 1;
    }

    ListenSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol); //Создание сокета с адресными данными из addrResult. 

    if (ListenSocket == INVALID_SOCKET) {
        wcout << L"socket creation with error" << endl;
        freeaddrinfo(addrResult);
        return 1;
    }

    result = bind(ListenSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen); //Привязка сокета с портом на компьютере; Аргументы функции -1)  сокет, который будет привязан к адресу, 2) указатель на структуру с адресом, к которому будет привязан сокет 3) Размер структуры
    //на которую указывает addrResult->ai_addr

    if (result == SOCKET_ERROR) {
        wcout << L"Ошибка подключения клиента" << endl;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    result = listen(ListenSocket, SOMAXCONN); // Сокет переходит в процесс прослушки сообщений от клиента; SOMAXCONN - максимальное кол-во ожидаемых соединений

    if (result == SOCKET_ERROR) {
        wcout << L"Listening failed" << endl;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    ClientSocket = accept(ListenSocket, NULL, NULL); // Принимает соединение клиента. Создаёт новый сокет дял работы с клиентом. NULL в addr и addrlen означает, что сервер не получает информацию о клиентах, а просто создаёт новый сокет.

    if (result == SOCKET_ERROR) {
        wcout << L"shutdown error" << endl;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    closesocket(ListenSocket); // Закрытие сокета прослушивания

    do {
        ZeroMemory(recvBuffer, 512); //Обнуление буфера приёма сообщений
        result = recv(ClientSocket, recvBuffer, 512, 0); //recv принимает данные от клиента и сохраняет их в recvBuffer
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
            wcout << L"Не получилось получить сообщение" << endl;
        }
    } while (result > 0);

    result = shutdown(ClientSocket, SD_SEND); //Завершение отправки сообщений по сокету ClientSocket
    if (result == SOCKET_ERROR) {
        cout << "send FAILED" << endl;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    };

    closesocket(ClientSocket); //Закрытие сокета клиента
    freeaddrinfo(addrResult); // освобождает память, выделенную для адресной информации.
    WSACleanup(); //завершает использование Winsock.
    return 0;
}

