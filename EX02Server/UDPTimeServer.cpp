#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
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
#include <chrono>
#pragma once
#define TIME_PORT	27015


static int s_TimeLapReqNum = 1;
static auto s_Start = std::chrono::high_resolution_clock::now();

void calculateResponse(char(&recvBuff)[255], char(&sendBuff)[255]);
void measureTimeLap(char(&sendBuff)[255]);
void calculateTimeInSelectedCity(char(&recvBuff)[255], char(&sendBuff)[255]);
void GetDaylightSavings(char(&sendBuff)[255]);
void GetWeekOfYear(char(&sendBuff)[255]);
void GetSecondsSinceBeginingOfMonth(char(&sendBuff)[255]);
void GetMonthAndDay(char(&sendBuff)[255]);
void GetYear(char(&sendBuff)[255]);
void GetTimeWithoutDateOrSeconds(char(&sendBuff)[255]);
void GetClientToServerDelayEstimation(char(&sendBuff)[255]);
void GetTimeSinceEpoch(char(&sendBuff)[255]);
void GetTimeWithoutDate(char(&sendBuff)[255]);
void GET_TIME(char(&sendBuff)[255]);
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
		cout << "Time Server: Recieved: " << bytesRecv << " bytes of \"" << recvBuff << "\" message.\n";
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
		GET_TIME(sendBuff);
	}
	else if (strcmp("GetTimeWithoutDate", recvBuff) == 0) {
		GetTimeWithoutDate(sendBuff);
	}
	else if (strcmp("GetTimeSinceEpoch", recvBuff) == 0) {
		GetTimeSinceEpoch(sendBuff);
	}
	else if ((!strcmp("GetClientToServerDelayEstimation", recvBuff))
		|| (!strcmp("MeasureRTT", recvBuff))) {
		GetClientToServerDelayEstimation(sendBuff);
	}
	else if (!strcmp("GetTimeWithoutDateOrSeconds", recvBuff)) {
		GetTimeWithoutDateOrSeconds(sendBuff);
	}
	else if (strcmp("GetYear", recvBuff) == 0) {
		GetYear(sendBuff);
	}
	else if (!strcmp("GetMonthAndDay", recvBuff)) {
		GetMonthAndDay(sendBuff);
	}
	else if (!strcmp("GetSecondsSinceBeginingOfMonth", recvBuff)) {
		GetSecondsSinceBeginingOfMonth(sendBuff);
	}
	else if (strcmp("GetWeekOfYear", recvBuff) == 0) {
		GetWeekOfYear(sendBuff);
	}
	else if (!strcmp("GetDaylightSavings", recvBuff)) {
		GetDaylightSavings(sendBuff);
	}
	else if (!strcmp("GetTimeWithoutDateInCity1", recvBuff) ||
		!strcmp("GetTimeWithoutDateInCity2", recvBuff) ||
		!strcmp("GetTimeWithoutDateInCity3", recvBuff) ||
		!strcmp("GetTimeWithoutDateInCity4", recvBuff) ||
		!strcmp("GetTimeWithoutDateInCity5", recvBuff)) {
		calculateTimeInSelectedCity(recvBuff, sendBuff);
	}

	else if (!strcmp("MeasureTimeLap", recvBuff)) {
		measureTimeLap(sendBuff);
	}


}
void GET_TIME(char(&sendBuff)[255]) {
	time_t timer;
	time(&timer);
	strcpy(sendBuff, ctime(&timer));
	sendBuff[strlen(sendBuff) - 1] = '\0'; //to remove the new-line from the created string
}
void GetTimeWithoutDate(char(&sendBuff)[255]) {
	time_t timer;
	time(&timer);
	struct tm* times = localtime(&timer);
	sprintf(sendBuff, "%d:%d:%d", times->tm_hour, times->tm_min, times->tm_sec);
}
void GetTimeSinceEpoch(char(&sendBuff)[255]) {
	time_t timer;
	time(&timer);
	sprintf(sendBuff, "%lld", timer);
}
void GetClientToServerDelayEstimation(char(&sendBuff)[255]) {
	DWORD currTime = GetTickCount();
	strcpy(sendBuff, to_string(currTime).c_str());;
}
void GetTimeWithoutDateOrSeconds(char(&sendBuff)[255]) {
	time_t timer;
	time(&timer);
	struct tm* times = localtime(&timer);
	sprintf(sendBuff, "%d:%d", times->tm_hour, times->tm_min);
}
void GetYear(char(&sendBuff)[255]) {
	time_t timer;
	time(&timer);
	struct tm* times = localtime(&timer);
	sprintf(sendBuff, "%d", (times->tm_year + 1900));
}
void GetMonthAndDay(char(&sendBuff)[255]) {
	time_t timer;
	time(&timer);
	struct tm* times = localtime(&timer);
	sprintf(sendBuff, "%d/%d", times->tm_mday, (times->tm_mon + 1));
}
void GetSecondsSinceBeginingOfMonth(char(&sendBuff)[255]) {
	time_t timer;
	time(&timer);
	struct tm* times = localtime(&timer);

	int secondFromDay = times->tm_mday * 24 * 3600;
	int secondFromHour = times->tm_hour * 3600;
	int secondFromMin = times->tm_min * 60;
	int seconds = times->tm_sec;
	int res = secondFromDay + secondFromHour + secondFromMin + seconds;
	sprintf(sendBuff, "%d", res);
}
void GetWeekOfYear(char(&sendBuff)[255]) {
	time_t timer;
	time(&timer);
	struct tm* times = localtime(&timer);
	sprintf(sendBuff, "%d", times->tm_yday / 7);
}
void GetDaylightSavings(char(&sendBuff)[255]) {
	time_t timer;
	time(&timer);
	struct tm* times = localtime(&timer);
	sprintf(sendBuff, "%d", times->tm_isdst);
}
void calculateTimeInSelectedCity(char(&recvBuff)[255], char(&sendBuff)[255]) {

	int timeOffset = 0;
	char citiesOption = recvBuff[24];
	time_t timer;
	time(&timer);

	struct tm* times = gmtime(&timer);

	switch (citiesOption)
	{
	case '1': // Doha
		timeOffset = 3;
		times->tm_hour = times->tm_hour + 3;
		break;
	case '2': // Prague
		times->tm_hour = times->tm_hour + 1;
		break;
	case '3': // New York
		times->tm_hour = times->tm_hour - 5;
		break;
	case '4': // Berlin
		times->tm_hour = times->tm_hour + 1;
		break;
	case '5': // Universal
		break;
	default:
		break;
	}
	strftime(sendBuff, sizeof(sendBuff), "%H:%M:%S", times);
	//sprintf(sendBuff, "%d:%d:%d", (times->tm_hour ) % 24, times->tm_min, times->tm_sec);
}


void measureTimeLap(char(&sendBuff)[255])
{
	if (s_TimeLapReqNum == 1)
	{
		s_Start = std::chrono::high_resolution_clock::now();
		strcpy(sendBuff, "Time lap measurement started");
		s_TimeLapReqNum++;
	}
	else
	{
		auto stop = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double>duration = (stop - s_Start);

		if (duration.count() > 180)
		{
			auto start = std::chrono::high_resolution_clock::now();
			sprintf(sendBuff, "Time lap measurement started");
		}
		else
		{
			sprintf(sendBuff, "Time Lap Measured: %lf", duration.count());
			s_TimeLapReqNum = 0;
		}

	}
}
