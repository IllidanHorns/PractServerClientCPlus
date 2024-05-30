#define WIN32_LEAN_AND_MEAN 

#include <iostream>
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

using namespace std;

int main()
{
    WSADATA wsaData; //Информация о Windows Socket
    ADDRINFO hints; // Информация о типах адресов
    ADDRINFO* addrResult; //Информация о результате поиска
    SOCKET ClientSocket = INVALID_SOCKET; //Сокет, который используется для связи с вервером

    const char* sendBuffer = "Hello from Client!"; 
    char recvBuffer[512];

    int result = WSAStartup(MAKEWORD(2, 2), &wsaData); //MAKEWORD - указываепт версию; WSAStartup - указывает на запуск работы Windows Socket
    if (result != 0) { //Проверяем правильность инициализации Windows Socket
        cout << "WSAStartup failed" << endl;
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints)); //Обнуляем hints для корректной работы
    hints.ai_family = AF_INET; // Указываем IP_V4
    hints.ai_socktype = SOCK_STREAM; // Указываем потоковую передачу данных
    hints.ai_protocol = IPPROTO_TCP; // Указываем TCP-протокол

    result = getaddrinfo("localhost", "788", &hints, &addrResult); //Заполняет адресную информацию для подключения к серверу с клиента
    if (result != 0) { //Проверяем корректность подключению по результату
        cout << "getaddrinfo failed" << endl;
        return 1;
    }

    ClientSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol); //Создаём сокет клиента с алревсными параметрами, которые задавали в getaddrinfo
    if (ClientSocket == INVALID_SOCKET) { //Проверка на правильно-созданный сокет
        cout << "socket creation with error" << endl;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    result = connect(ClientSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen); //Подключаемся к серверу с клиентского сокета, учитывая адресные настройки (addResult)
    if (result == SOCKET_ERROR) { 
        cout << "Unable connect to server" << endl;
        freeaddrinfo(addrResult);  //Память, которая выделялась на хранение адресной информации addrResult
        closesocket(ClientSocket);  //Закрытие сокета
        WSACleanup(); //Отчистка
        return 1;
    }

    result = send(ClientSocket, sendBuffer, (int)strlen(sendBuffer), 0); //Отправка первого сообщения через сокет 
    if (result == SOCKET_ERROR) {
        cout << "send FAILED" << endl;
        freeaddrinfo(addrResult); //Память, которая выделялась на хранение адресной информации addrResult
        closesocket(ClientSocket); 
        WSACleanup();
        return 1;
    }
    cout << "Sent " << result << " bytes" << endl;

    result = send(ClientSocket, sendBuffer, (int)strlen(sendBuffer), 0); //Отправка второго сообщения через сокет 
    if (result == SOCKET_ERROR) {
        cout << "send FAILED" << endl;
        freeaddrinfo(addrResult); //Память, которая выделялась на хранение адресной информации addrResult
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }
    cout << "Sent " << result << " bytes" << endl; // Кол-во байт, которые дошли

    result = shutdown(ClientSocket, SD_SEND); //Завершение отправки данных с сокета с помощью shutdown. SD_SEND - запрет сокета на отправку данных
    if (result == SOCKET_ERROR) {
        cout << "shutdown FAILED" << endl;
        freeaddrinfo(addrResult); //Память, которая выделялась на хранение адресной информации addrResult
        closesocket(ClientSocket); 
        WSACleanup();
        return 1;
    }

    do {
        ZeroMemory(recvBuffer, 512);
        result = recv(ClientSocket, recvBuffer, 512, 0); //Получение данных от сервера
        if (result > 0) {   
            cout << "Received " << result << " bytes" << endl;
            cout << "Received: " << recvBuffer << endl; // resvBuffer принимает результат передачи данных от сервера
        }
        else if (result == 0) { // Проверка на результат получения данных
            cout << "Connection closed" << endl;
        }
        else {
            cout << "recv failed with error" << endl;
        }
    } while (result > 0);

    closesocket(ClientSocket); //Закрытие сокета
    freeaddrinfo(addrResult); //Память, которая выделялась на хранение адресной информации addrResult
    WSACleanup(); //Завершение работы сокета 

    return 0;
}

