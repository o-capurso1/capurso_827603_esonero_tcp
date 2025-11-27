/*
 * main.c
 *
 * TCP Client - Template for Computer Networks assignment
 *
 * This file contains the boilerplate code for a TCP client
 * portable across Windows, Linux and macOS.
 */

#if defined WIN32
#include <winsock.h>
#else
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#define closesocket close
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "protocol.h"

#define NO_ERROR 0

void clearwinsock() {
#if defined WIN32
	WSACleanup();
#endif
}

int main(int argc, char* argv[]) {
    int ris = 0; // NO_ERROR

    // 1. Parsing command line
    char server_ip[32] = "127.0.0.1";
    int port = SERVER_PORT;
    char request[BUFFER_SIZE] = "";
    for (int i = 1; i < argc-1; i++) {
        if (strcmp(argv[i], "-s") == 0)
            strcpy(server_ip, argv[i+1]);
        else if (strcmp(argv[i], "-p") == 0)
            port = atoi(argv[i+1]);
        else if (strcmp(argv[i], "-r") == 0)
            strcpy(request, argv[i+1]);
    }

	#if defined WIN32
		WSADATA wsaData;
		ris = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (ris != 0) {
			printf("WSAStartup failed: %d\n", ris);
			return 1;
		}
	#endif

    // Parsing request
    weather_request_t req;
    memset(&req, 0, sizeof(req));
    sscanf(request, "%c %[^\n]", &req.type, req.city);

    // 2. Socket
    SOCKET my_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (my_socket < 0) {
        perror("Socket creation failed");
        clearwinsock();
        return 1;
    }

    struct sockaddr_in sad;
    sad.sin_family = AF_INET;
    sad.sin_addr.s_addr = inet_addr(server_ip);
    sad.sin_port = htons(port);

    // 3. Connect
    if (connect(my_socket, (struct sockaddr*)&sad, sizeof(sad)) < 0) {
        perror("Connect failed");
        clearwinsock();
        return 1;
    }

    // 4. Sending request
    send(my_socket, (char*)&req, sizeof(req), 0);

    // 5. Get answer
    weather_response_t res;
    int n = recv(my_socket, (char*)&res, sizeof(res), 0);

    printf("Ricevuto risultato dal server ip %s. ", server_ip);
    if (n <= 0) {
        perror("Error receiving response");
    }
    else if (res.status == STATUS_OK) {
        switch (res.type) {
            case 't':
            	printf("%s: Temperatura = %.1f°C\n", req.city, res.value);
            	break;
            case 'h':
            	printf("%s: Umidità = %.1f%%\n", req.city, res.value);
            	break;
            case 'w':
            	printf("%s: Vento = %.1f km/h\n", req.city, res.value);
            	break;
            case 'p':
            	printf("%s: Pressione = %.1f hPa\n", req.city, res.value);
            	break;
        }
    }
    else if (res.status == STATUS_CITY_ERR) {
        printf("Citta' non disponibile\n");
    }
    else if (res.status == STATUS_INVALID_REQ) {
        printf("Richiesta non valida\n");
    }

	closesocket(my_socket);
    clearwinsock();
    return NO_ERROR;
}
