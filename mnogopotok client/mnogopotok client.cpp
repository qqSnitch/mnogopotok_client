#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <string>
#include <windows.h>
#include <iostream>
#include <queue>
#include <thread>
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable: 4996)
using namespace std;
#define SRV_HOST "26.194.218.76"  //адрес сервера
#define SRV_PORT 1234 //порт сервера
#define CLNT_PORT 1235  //порт клиента
#define BUF_SIZE 64  //размер сообщений

struct Message {
    char name[20];
    char msg[120];
} getmsg, sendmsg;

SOCKET Connection;
char buff[10 * 1014];

void getMessage() {
    while (1) 
    {
        int size = recv(Connection, (char*)&getmsg, sizeof(getmsg), 0);
        if (size >= 0) 
        {
            if (string(getmsg.name).compare(string(sendmsg.name)))
                cout << getmsg.name <<": " << getmsg.msg << endl;
        }
    }
    closesocket(Connection);
    Connection = INVALID_SOCKET;
    return;
}

int connectToServer() {
    if (WSAStartup(0x202, (WSADATA*)&buff))
    {
        cout << "WSASTARTUP ERROR: " << WSAGetLastError() << "\n";
        return 1;
    }
    int from_len;
    char buf[BUF_SIZE] = { 0 };
    hostent* hp;
    sockaddr_in clnt_sin, srv_sin;
    Connection = socket(AF_INET, SOCK_STREAM, 0);
    clnt_sin.sin_family = AF_INET;
    clnt_sin.sin_addr.s_addr = 0;
    clnt_sin.sin_port = htons(CLNT_PORT);
    bind(Connection, (sockaddr*)&clnt_sin, sizeof(clnt_sin));
    hp = gethostbyname(SRV_HOST);
    srv_sin.sin_port = htons(SRV_PORT);
    srv_sin.sin_family = AF_INET;
    ((unsigned long*)&srv_sin.sin_addr)[0] = ((unsigned long**)hp->h_addr_list)[0][0];
    connect(Connection, (sockaddr*)&srv_sin, sizeof(srv_sin));

    return 0;
}

int main()
{
    if (connectToServer())
        return -1;
    cout << "Name: ";
    cin >> sendmsg.name;
    thread tread(getMessage);
    tread.detach();

    while (1)
    {
        cin >> sendmsg.msg;
        send(Connection, (char*)&sendmsg, sizeof(sendmsg), 0);
        if (!string(sendmsg.msg).compare(string("!exit")))
        {
            break;
        }
    }
    closesocket(Connection);
    WSACleanup();
    return 0;
}
