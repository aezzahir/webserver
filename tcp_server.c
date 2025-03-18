#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include  <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netinet/in.h>

#define BACKLOG 10
#define PORT 1437
#define BUFFER_SIZE 2048
/**
 * setup_sever: setup the server and return the server socket fd
 * @ip_address: the ip address of the network interface ...
 * @port_number: the server port number
 * @return: the socket file descriptor
 * */

int setup_server(char *ip_address, int port_number) {
	struct sockaddr_in server_socket;
	int server_socket_fd;

	// Define the Address and port for the server
	inet_pton(AF_INET, ip_address, &(server_socket.sin_addr));
	server_socket.sin_family = AF_INET; // IPv4
	server_socket.sin_port = htons(port_number);// port number
	// Create a socket fd
	server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket_fd == -1){
		fprintf(stderr, "socket fd error: %s\n", strerror(errno));
		return -1;
	}
	// bind the socket_fd to the server address
	if (-1 == bind(server_socket_fd, (const struct sockaddr *)&server_socket, sizeof(server_socket))) {
		fprintf(stderr, "bind: %s\n", strerror(errno));
		return -1;
	}
	printf("Bound socket to localhost port %d\n", port_number);
	if (-1 == listen(server_socket_fd, BACKLOG)) {
		fprintf(stderr, "listen error: %s\n", strerror(errno));
		return (-1);
	}
	printf("Listening on port %d ...\n", port_number);
	return (server_socket_fd);
}
int main() {
	printf("---- SERVER ---- \n\n");
	char buffer[BUFFER_SIZE];
	struct sockaddr_in server_socket;
	int server_socket_fd;
	struct sockaddr_in client_socket;
	int client_socket_fd;
	socklen_t addr_size;
	int status;
	int bytes_recv;
	int bytes_sent;
	int bytes_read;
	//
	int index_fd;

	
	addr_size = sizeof(client_socket);
	client_socket_fd = accept(server_socket_fd, (struct sockaddr *)&client_socket, &addr_size);
	if (client_socket_fd == -1){
		fprintf(stderr, "client socket fd error: %s\n", strerror(errno));
		return (4);
	}
	printf("Accepted new connection on client socket fd: %d\n", client_socket_fd);

	bytes_recv = 1;
	while (1) {
		bytes_recv = recv(client_socket_fd, buffer, BUFFER_SIZE, 0);
		if (bytes_recv <= 0) {
			continue;
		}
		else  {
			buffer[bytes_recv] = '\0';
			printf("message received from client:\n %s\n", buffer);
			memset(buffer, 0, BUFFER_SIZE);
			index_fd = open("./index.html", O_RDONLY);
			bytes_read = read(index_fd, buffer, BUFFER_SIZE);
			if (bytes_sent == BUFFER_SIZE) {
				printf("\n need more space for buffer ... \n");
			}
			bytes_sent = send(client_socket_fd,buffer,bytes_read, 0);
			if (bytes_sent == -1) {
				fprintf(stderr, "send error: %s\n", strerror(errno));
			}
			else if (bytes_sent == bytes_read){
				printf("Sent full message to client socket %d: \n", client_socket_fd);
			}
			else {
				printf("Sent partial message to client socket %d: %d bytes sent.\n", client_socket_fd, bytes_sent);
			}

		}

		memset(buffer, 0, BUFFER_SIZE);
	}


	return (0);
}
