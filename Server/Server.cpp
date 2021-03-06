// Server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

SOCKET sock;
struct sockaddr_in server, si_other, si_dummy, connections[MAX_CONNECTIONS];
int slen, recv_len;
char buf[BUF_LEN];
WSADATA wsa;
int result;

void printConnectionInfo(sockaddr_in connection);

int main()
{
	slen = sizeof(si_other);

	// Initialize winsock
	result = WSAStartup(MAKEWORD(2, 2), &wsa);
	if (result != 0)
	{
		printf("Socket initialization failed. Error Code: %d\n", WSAGetLastError());
		system("pause");
		exit(EXIT_FAILURE);
	}
	printf("Socket initialized.\n");

	// Create socket
	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock == INVALID_SOCKET)
	{
		printf("Could not create socket. Error Code: %d\n", WSAGetLastError());
		system("pause");
		exit(EXIT_FAILURE);
	}
	printf("Socket created.\n");

	// Prepare sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(PORT);

	// Bind
	result = bind(sock, (struct sockaddr*)&server, sizeof(server));
	if (result == SOCKET_ERROR)
	{
		printf("Bind failed. Error Code: %d\n", WSAGetLastError());
		system("pause");
		exit(EXIT_FAILURE);
	}
	printf("Server running on port %d\n", PORT);

	// Loop and listen for data
	while (true)
	{
		// Clear buffer by filling with null
		memset(buf, '\0', BUF_LEN);

		recv_len = recvfrom(sock, buf, BUF_LEN, 0, (struct sockaddr*)&si_other, &slen);
		if (recv_len == SOCKET_ERROR)
		{
			// Print error if unable to receive packet
			printf("Failed to recieve data. Error Code: %d\n", WSAGetLastError());
		}
		else
		{
			// Handle packet info in no error reading
			bool isConnectionAccepted = false;
			switch (buf[0])
			{
				// On open connection sent from client
			case OPEN_CONNECTION[0]:
				for (int i = 0; i < MAX_CONNECTIONS; i++)
				{
					sockaddr_in* memoryConnection = &connections[i];
					sockaddr_in* curConnections = &si_other;
					// If a connection[i] has the default connection information stored
					if (connections[i].sin_addr.s_addr == si_dummy.sin_addr.s_addr && connections[i].sin_port == si_dummy.sin_port)
					{
						// Accept connection
						result = sendto(sock, CONNECTION_ACCEPTED, BUF_LEN, 0, (struct sockaddr*)&si_other, slen);
						printConnectionInfo(si_other);
						printf("Connection established.\n");

						// Save connection to memory
						memcpy(&connections[i], &si_other, sizeof(si_other));
						//connections[i] = si_other;
						isConnectionAccepted = true;
						break;
					}
				}
				if (!isConnectionAccepted)
				{
					// If there are no unused connection[i]s then refuse connection
					result = sendto(sock, CONNECTION_REFUSED, BUF_LEN, 0, (struct sockaddr*)&si_other, slen);
					printConnectionInfo(si_other);
					printf("Connection refused\n");
				}
				break;

				// On close connection sent from client
			case CLOSE_CONNECTION[0]:
				for (int i = 0; i < MAX_CONNECTIONS; i++)
				{
					// If connection[i] has the same connection information as current connection
					if (connections[i].sin_addr.s_addr == si_other.sin_addr.s_addr && connections[i].sin_port == si_other.sin_port)
					{
						// Write dummy information to connection[i] to nullify it
						connections[i] = si_dummy;
						printConnectionInfo(si_other);
						printf("Connection closed by peer.\n");
						break;
					}
				}
				break;

				// List all connections to server console (debug)
			case 'z':
				for (int i = 0; i < MAX_CONNECTIONS; i++)
				{
					printf("%d. %s:%d\n", i + 1, inet_ntoa(connections[i].sin_addr), ntohs(connections[i].sin_port));
				}
				break;

				// On keypress sent from client
			default:
				printConnectionInfo(si_other);
				printf("%s\n", buf);

				// Broadcast keypress to all connected peers
				for (int i = 0; i < MAX_CONNECTIONS; i++)
				{
					// If a connection[i] does not have the default connection information stored
					if (!(connections[i].sin_addr.s_addr == si_dummy.sin_addr.s_addr && connections[i].sin_port == si_dummy.sin_port))
					{
						result = sendto(sock, buf, BUF_LEN, 0, (struct sockaddr*)&connections[i], sizeof(connections[i]));
					}
				}
				break;
			}

			// Catch socket errors and handle crashes while sending
			if (result == SOCKET_ERROR)
			{
				printf("Failed to send data. Error Code: %d\n", WSAGetLastError());
				system("pause");
				exit(EXIT_FAILURE);
			}
		}
	}
}

void printConnectionInfo(sockaddr_in connection)
{
	// Print details of the peer and the data received
	SYSTEMTIME cur_time;
	GetLocalTime(&cur_time);
	printf("[%02d:%02d:%02d - %s:%d] ", cur_time.wHour, cur_time.wMinute, cur_time.wSecond, inet_ntoa(connection.sin_addr), ntohs(connection.sin_port));
}