// -------------------------------------------------------------------------- //
// Script info
//
//
// -------------------------------------------------------------------------- //


// -------------------------------------------------------------------------- //
// Includes
// -------------------------------------------------------------------------- //

// Standard
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>


// Library
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <arpa/inet.h>
#include <netdb.h>


// User defined
#include "BananaClient.h"

// -------------------------------------------------------------------------- //


// -------------------------------------------------------------------------- //
// Globals
// -------------------------------------------------------------------------- //

/*
 * 	Port number between 1024 and 65535
 */
#define PORT "25002"
#define BACKLOG 4

// -------------------------------------------------------------------------- //


// -------------------------------------------------------------------------- //
// Externs
// -------------------------------------------------------------------------- //

void sigchld_handler(int s){
	while(waitpid(-1, NULL, WNOHANG) > 0);
}

// Get socket address IPv4 or IPv6
void *get_in_addr(struct sockaddr *sa){
	if (sa->sa_family == AF_INET) {
    	return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


BananaClient::BananaClient(){
	bindable();
}

int BananaClient::bindable() {
	printf("BClient starting...\n");

	struct addrinfo hints, *servinfo, *p;
	struct sigaction sa;
	int yes = 1;
	int rv;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	for (p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}
		fcntl(sockfd, F_SETFL, O_NONBLOCK);

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
				sizeof(int)) == -1) {
			perror("sesockopt");
			exit(1);
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("server: bind");
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "server: failed to bind\n");
		return 2;
	}

	freeaddrinfo(servinfo);

	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}

	sa.sa_handler = sigchld_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	return 0;
}

void BananaClient::loop() {
	printf("server: waiting for connections...\n");

	struct sockaddr_storage their_addr;
	socklen_t sin_size;
	char s[INET6_ADDRSTRLEN];
	int new_fd;

	sin_size = sizeof their_addr;
	new_fd = accept(sockfd, (struct sockaddr *) &their_addr, &sin_size);
	if (new_fd == -1) {
		perror("accept");
		return;
	}

	inet_ntop(their_addr.ss_family,
			get_in_addr((struct sockaddr *) &their_addr),
			s, sizeof s);
	printf("sever: got connection from %s\n", s);

	if (!fork()) {
		close(sockfd);
		if (send(new_fd, "Hello Andrew, from Richard!", 40, 0) == -1) {
			perror("send");
		}
		close(new_fd);
		exit(0);
		return;
	}

	close(new_fd);
	return;


}



BananaClient::~BananaClient(){

}


