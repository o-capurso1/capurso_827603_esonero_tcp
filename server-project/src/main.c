/*
 * main.c
 *
 * TCP Server - Template for Computer Networks assignment
 *
 * This file contains the boilerplate code for a TCP server
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

#ifndef WIN32
typedef int SOCKET;
#endif

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
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
    int port = SERVER_PORT;
    for (int i = 1; i < argc - 1; i++)
        if (strcmp(argv[i], "-p") == 0)
            port = atoi(argv[i + 1]);

	#if defined WIN32
		WSADATA wsaData;
		ris = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (ris != 0) {
			printf("WSAStartup failed: %d\n", ris);
			return 1;
		}
	#endif

    srand((unsigned)time(NULL));

    // 2. Socket creation (SOCK_STREAM)
    SOCKET my_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (my_socket < 0) {
        perror("Socket creation failed");
        clearwinsock();
        return 1;
    }

    struct sockaddr_in sad;
    sad.sin_family = AF_INET;
    sad.sin_addr.s_addr = INADDR_ANY;
    sad.sin_port = htons(port);

    // 3. Bind
    if (bind(my_socket, (struct sockaddr*) &sad, sizeof(sad)) < 0) {
        perror("Bind failed");
        clearwinsock();
        return 1;
    }

    // 4. Listen
    if (listen(my_socket, QUEUE_SIZE) < 0) {
        perror("Listen failed");
        clearwinsock();
        return 1;
    }

    while (1) {
        // 5. Accept
        struct sockaddr_in cad;
		#if defined WIN32
				int client_len = sizeof(cad);
		#else
				socklen_t len = sizeof(cad);
		#endif

        SOCKET client_socket = accept(my_socket, (struct sockaddr*)&cad, &client_len);
        if (client_socket < 0) {
            perror("Accept failed");
            clearwinsock();
            return 1;
        }

        // 6. Reception request
        weather_request_t req;
        int n = recv(client_socket, (char*)&req, sizeof(req), 0);

        printf("Richiesta '%c %s' dal client ip %s\n", req.type, req.city, inet_ntoa(cad.sin_addr));

        weather_response_t res;

        // 7. Request validation and response generation
        if (req.type != 't' && req.type != 'h' && req.type != 'w' && req.type != 'p') {
            res.status = STATUS_INVALID_REQ;
            res.type = req.type;
            res.value = 0.0f;
        }
        else if (!is_supported_city(req.city)) {
            res.status = STATUS_CITY_ERR;
            res.type = req.type;
            res.value = 0.0f;
        }
        else {
            res.status = STATUS_OK;
            res.type = req.type;
            switch (req.type) {
                case 't':
                	res.value = get_temperature();
                	break;
                case 'h':
                	res.value = get_humidity();
                	break;
                case 'w':
                	res.value = get_wind();
                	break;
                case 'p':
                	res.value = get_pressure();
                	break;
            }
        }

        // 8. Sending response
        send(client_socket, (char*)&res, sizeof(res), 0);
		closesocket(client_socket);
    }

    closesocket(my_socket);
    clearwinsock();
    return NO_ERROR;
}

// City validation function
int is_supported_city(const char* city) {
    static const char* cities[] = {"bari", "roma", "milano", "napoli", "torino", "palermo", "genova", "bologna", "firenze", "venezia"};
    const int n_cities = (sizeof(cities)/sizeof(cities[0]));

    for (int i = 0; i < n_cities; i++) {
		#if defined WIN32
			if (_stricmp(city, cities[i]) == 0)
				return 1;
		#else
			if (strcasecmp(city, cities[i]) == 0)
				return 1;
		#endif
    }

    return 0;
}

// Weather generator functions
float get_temperature() {
	return ((rand() % 500) / 10.0f) - 10.0f; // -10.0 to 40.0
}

float get_humidity() {
	return ((rand() % 801) / 10.0f) + 20.0f; // 20.0 to 100.0
}

float get_wind() {
	return (rand() % 1001) / 10.0f; // 0.0 to 100.0
}

float get_pressure() {
	return ((rand() % 1001) / 10.0f) + 950.0f; // 950.0 to 1050.0
}
