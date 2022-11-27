#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;
#pragma comment(lib, "Ws2_32.lib")
#include <winsock2.h>
#include <string.h>
#include <time.h>
#include<windows.h>
#include <stdlib.h>
#include <stdio.h>  
#include <iostream>  
#include<sstream>  
#define TIME_PORT	27015

void calculateResponse(char(&recvBuff)[255], char(&sendBuff)[255]);
void main()
{
	WSAData wsaData;
	if (NO_ERROR != WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		cout << "Time Server: Error at WSAStartup()\n";
		return;
	}
	SOCKET m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == m_socket)
	{
		cout << "Time Server: Error at socket(): " << WSAGetLastError() << endl;
		WSACleanup();
		return;
	}
	sockaddr_in serverService;
	serverService.sin_family = AF_INET;
	serverService.sin_addr.s_addr = INADDR_ANY;	
	serverService.sin_port = htons(TIME_PORT);
	if (SOCKET_ERROR == bind(m_socket, (SOCKADDR*)&serverService, sizeof(serverService)))
	{
		cout << "Time Server: Error at bind(): " << WSAGetLastError() << endl;
		closesocket(m_socket);
		WSACleanup();
		return;
	}

	sockaddr client_addr;
	int client_addr_len = sizeof(client_addr);
	int bytesSent = 0;
	int bytesRecv = 0;
	char sendBuff[255];
	char recvBuff[255];

	cout << "Time Server: Wait for clients' requests.\n";

	while (true)
	{
		bytesRecv = recvfrom(m_socket, recvBuff, 255, 0, &client_addr, &client_addr_len);
		if (SOCKET_ERROR == bytesRecv)
		{
			cout << "Time Server: Error at recvfrom(): " << WSAGetLastError() << endl;
			closesocket(m_socket);
			WSACleanup();
			return;
		}

		recvBuff[bytesRecv] = '\0';
	//	cout << "Time Server: Recieved: " << bytesRecv << " bytes of \"" << recvBuff << "\" message.\n";
		calculateResponse(recvBuff, sendBuff );
	
		bytesSent = sendto(m_socket, sendBuff, (int)strlen(sendBuff), 0, (const sockaddr*)&client_addr, client_addr_len);
		if (SOCKET_ERROR == bytesSent)
		{
			cout << "Time Server: Error at sendto(): " << WSAGetLastError() << endl;
			closesocket(m_socket);
			WSACleanup();
			return;
		}

		cout << "Time Server: Sent: " << bytesSent << "\\" << strlen(sendBuff) << " bytes of \"" << sendBuff << "\" message.\n";
	}

	// Closing connections and Winsock.
	cout << "Time Server: Closing Connection.\n";
	closesocket(m_socket);
	WSACleanup();
}
void calculateResponse(char(&recvBuff)[255], char(&sendBuff)[255]) {
	if (strcmp("GET_TIME", recvBuff) == 0) {
		time_t timer;
		time(&timer);
		strcpy(sendBuff, ctime(&timer));
		sendBuff[strlen(sendBuff) - 1] = '\0'; //to remove the new-line from the created string
	}
	else if (strcmp("GetTimeWithoutDate", recvBuff) == 0) {

		time_t timer;
		time(&timer);
		struct tm* times = localtime(&timer);
		sprintf(sendBuff, "%d:%d:%d", times->tm_hour, times->tm_min, times->tm_sec);

	}
	else if (strcmp("GetTimeSinceEpoch", recvBuff) == 0) {
		time_t timer;
		time(&timer);
		sprintf(sendBuff, "%lld", timer);
	}
	else if ((!strcmp("GetClientToServerDelayEstimation", recvBuff)) || (!strcmp("MeasureRTT", recvBuff))) {
		DWORD currTime = GetTickCount();
		strcpy(sendBuff, to_string(currTime).c_str());;
	}
	else if (strcmp("GetTimeWithoutDateOrSeconds", recvBuff) == 0) {
		time_t timer;
		time(&timer);
		struct tm* times = localtime(&timer);
		sprintf(sendBuff, "%d:%d", times->tm_hour, times->tm_min);
	}
}
