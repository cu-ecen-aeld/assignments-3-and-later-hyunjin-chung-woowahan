#include <stdio.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUF_SIZE (1024 * 1024)
#define FILE_PATH ("/var/tmp/aesdsocketdata")

static bool disconnect = false;
static int sockfd;
static int sockfd_connected;
static char *buf;
static char ipaddr[INET_ADDRSTRLEN];

static void signal_handler(int signal_number)
{
	if (signal_number == SIGINT || signal_number == SIGTERM)
	{
		syslog(LOG_INFO, "Closed connection from %s\n", ipaddr);
		shutdown(sockfd, SHUT_RDWR);
		shutdown(sockfd_connected, SHUT_RDWR);
		remove(FILE_PATH);
		disconnect = true;
	}
}

static bool send_data(void)
{
	FILE* file;

	if ((file = fopen(FILE_PATH, "r")) == NULL)
	{
		printf("file open fail\n");
		return false;
	}

	while(fgets(buf, BUF_SIZE, file) != NULL)
	{
		//printf("send string: %s\n", buf);
		if (send(sockfd_connected, buf, strlen(buf), 0) == -1)
		{
			printf("send is failed\n");
			return false;
		}
	}
								
	if (fclose(file) != 0)
	{
		printf("fail to close file");
		return false;
	}

	return true;
}

static bool write_file(int data_size)
{
	FILE* file;
	
	if ((file = fopen(FILE_PATH, "a+")) == NULL)
	{
		printf("fail to open write file");
		return false;
	}

	char* cur_buf = buf;
	for (int i = 0; i < data_size; i++)
	{
		if (buf[i] == '\n')
		{
			buf[i] = '\0';
			//printf("write string: %s\n",cur_buf);

			if (fprintf(file, "%s\n", cur_buf) < 0)
			{
				printf("fail to write file");
				return false;
			}

			cur_buf = &buf[i + 1];
		}
	}

	if (fclose(file) != 0)
	{
		printf("fail to close file");
		return false;
	}

	return true;
}
								

int main(int argc, char * argv[])
{
	bool daemon_mode = false;
	int pid = 0;
	int result = 0;
	struct sigaction new_action;
	struct addrinfo hints;

	if ((argc == 2) && (!strcmp(argv[1], "-d")))
		daemon_mode = true;

	memset(&new_action, 0, sizeof(struct sigaction));
	new_action.sa_handler = signal_handler;

	if (sigaction(SIGTERM, &new_action, NULL) != 0)
	{
		perror("fail to register SIGTERM signal handler");
		return -1;
	}

	if (sigaction(SIGINT, &new_action, NULL) != 0)
	{
		perror("fail to register SIGINT signal handler");
		return -1;
	}
			
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	//hints.ai_flags = AI_PASSIVE;
	
	printf("start aesdsocket\n");

	if ((sockfd = socket(hints.ai_family, hints.ai_socktype, 0)) == -1)
	{
		perror("fail to create socket");
		return -1;
	}
	
	printf("create socket\n");
	
	struct addrinfo *servinfo;

	int option = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(int)) == -1)
	{
		perror("fail to set socket option");
		return -1;
	}
	
	printf("set socket\n");
	
	int getaddrinfo_err;
	if ((getaddrinfo_err = getaddrinfo(NULL, "9000", &hints, &servinfo)))
	{
		printf("%d\n", getaddrinfo_err);
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(getaddrinfo_err));
		return -1;
	}

	const char* p_ip_addr;
	if ((p_ip_addr = inet_ntop(AF_INET, servinfo->ai_addr, ipaddr, INET_ADDRSTRLEN)) == NULL)
	{
		perror("fail to convert ipv4 string");
		return -1;
	}
	
	syslog(LOG_INFO, "Accepted connection from %s\n", p_ip_addr);
	
	if (bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1)
	{
		perror("fail to bind socket");
		return -1;
	}

	printf("bind socket\n");
	
	freeaddrinfo(servinfo);

	if (daemon_mode)
		pid = fork();

	if (pid == 0)
	{
		if (listen(sockfd, 5) == -1)
		{
			perror("fail to listen socket");
			return -1;
		}
		
		printf("listen\n");

		struct sockaddr sockaddr_connected;
		socklen_t sockaddrlen_connected = sizeof(struct sockaddr);
		
		openlog (NULL, 0, LOG_USER);
		buf = malloc(sizeof(char) * BUF_SIZE);

		while (!disconnect)
		{
			if ((sockfd_connected = accept(sockfd, &sockaddr_connected, &sockaddrlen_connected)) == -1)
			{
				perror("fail to listen socket");
				result = -1;
				break;
			}	

			while(true)
			{
				int num_bytes_received;

				if ((num_bytes_received = recv(sockfd_connected, buf, BUF_SIZE, 0)) < 0)
				{
					perror("fail to receive data");
					disconnect = true;
					result = -1;
					break;
				}
				else if (num_bytes_received == 0)
				{
					//printf("finished\n");
					break;
				}
				
				if (num_bytes_received > BUF_SIZE)
				{
					fprintf(stderr, "packet is exceed the buffer size!");
					disconnect = true;
					result = -1;
					break;
				}
				
				if (write_file(num_bytes_received) == false)
				{
					fprintf(stderr, "fail to write file");
					disconnect = true;
					result = -1;
					break;
				}
		
				if (send_data() == false)
				{
					fprintf(stderr, "fail to send data");
					disconnect = true;
					result = -1;
					break;
				}
			}
		}
		
		free(buf);
		closelog();
	}

	return result;
}
