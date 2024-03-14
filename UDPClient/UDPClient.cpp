#pragma comment(lib, "Ws2_32.lib")
#include <WinSock2.h>
#include <WS2tcpip.h>

#include <iostream>
using namespace std;

bool isValidNick(const char* nick) {
    for (int i = 0; nick[i] != '\0'; i++) {
        if (isspace(nick[i]))
            return false;
    }
    return true;
}

int main()
{
    WSADATA wsadata;

    int res = WSAStartup(MAKEWORD(2, 2), &wsadata);
    if (res != NO_ERROR)
    {
        cout << "WSAStartup failked with error " << res << endl;
        return 1;
    }

    SOCKET udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (udpSocket == INVALID_SOCKET)
    {
        cout << "socket failed with error " << WSAGetLastError() << endl;
        return 2;
    }  

    sockaddr_in addrTo;
    addrTo.sin_family = AF_INET;
    addrTo.sin_port = htons(23000);
    inet_pton(AF_INET, "127.0.0.1", &addrTo.sin_addr.s_addr);

    const size_t sendBufSize = 1024;
    char sendBuf[sendBufSize];
    char originalSendBuf[sendBufSize];
    do {
        cout << "Enter data (nickname, number from 0 to 15, text): ";
        cin.getline(sendBuf, sendBufSize);
        strcpy_s(originalSendBuf, sendBuf);

        char nickname[sendBufSize], numberStr[sendBufSize];

        char* nextToken;
        char* token = strtok_s(sendBuf, " ", &nextToken);
        int partsCount = 0;
        while (token != NULL && partsCount < 3) {
            if (partsCount == 0)
                strcpy_s(nickname, token);
            else if (partsCount == 1)
                strcpy_s(numberStr, token);

            partsCount++;
            token = strtok_s(NULL, " ", &nextToken);
        }

        if (partsCount != 3) {
            cout << "Error: Enter a nickname, a digit from 0 to 15, and text separated by spaces\n";
            continue;
        }

        if (!isValidNick(nickname)) {
            cout << "Error: the nickname must be a whole word without spaces\n";
            continue;
        }

        int number = atoi(numberStr);
        if (number < 0 || number > 15) {
            cout << "Error: the second part must be a digit from 0 to 15\n";
            continue;
        }

        break;

    } while (true);

    cout << "Sending data...." << endl;
    int sendResult = sendto(udpSocket, originalSendBuf, strlen(originalSendBuf), 0, (SOCKADDR*)&addrTo, sizeof(addrTo));
    if (sendResult == SOCKET_ERROR)
    {
        cout << "sendto failed with error " << WSAGetLastError() << endl;
        return 4;
    }

    const size_t receiveBufSize = 1024;
    char receiveBuf[receiveBufSize];

    sockaddr_in addrFrom;
    int addrFromSize = sizeof(addrFrom);

    cout << "Receiving data...." << endl;
    int bytesReceived = recvfrom(udpSocket, receiveBuf, receiveBufSize, 0, (SOCKADDR*)&addrFrom, &addrFromSize);

    if (bytesReceived == SOCKET_ERROR)
    {
        cout << "recvfrom failed with error " << WSAGetLastError() << endl;
        return 4;
    }

    receiveBuf[bytesReceived] = '\0';
    cout << "Received from " << addrFrom.sin_addr.S_un.S_addr << endl;
    cout << "Data: " << receiveBuf << endl;

    closesocket(udpSocket);
    WSACleanup();
}