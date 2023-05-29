#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <syslog.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#define USER_PORT 9000
#define READ_SIZE 4096
#define DATA_PATH "/var/tmp/aesdsocketdata"

int socket_fd;

void signal_handler(int sig) {
  syslog(LOG_INFO, "Caught signal, exiting");
  close(sockfd);
  close(client_sockfd);
  close(socket_file_fd);
  remove(DATA_PATH);
  
  if (buffer_ptr) {
        free(buffer_ptr);
  }
  exit(0);
}

int main(int argc, char *argv[]) {
  int opt = 0;
  int sockflgs = 1;
  struct addrinfo myinfoin = {.ai_flags = AI_PASSIVE,
                                  .ai_family = PF_INET,
                                  .ai_socktype = SOCK_STREAM,
                                  .ai_protocol = 0,
                                  .ai_addrlen = 0,
                                  .ai_addr = NULL,
                                  .ai_canonname = NULL,
                                  .ai_next = NULL};
  struct addrinfo *myinfo;

  openlog(NULL, 0, LOG_USER);// Set up syslog

  // Set up signal handling for SIGTERM and SIGINT
  struct sigaction s_action;
  memset(&s_action, 0, sizeof(s_action));
  s_action.sa_handler = signal_handler;
  if (sigaction(SIGTERM, &s_action, NULL) != 0) {
    syslog(LOG_ERR, "Error registering SIGTERM: %m");
  }
  if (sigaction(SIGINT, &s_action, NULL) != 0) {
    syslog(LOG_ERR, "Error registering SIGINT: %m");
  }

  // Create a socket
  socket_fd = socket(PF_INET, SOCK_STREAM, 0);
  if (socket_fd < 0) {
    printf("Socket could not be created: %m\n");
    exit(-1);
  }
  
  // Set socket options
  if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &sockflgs,
                 sizeof(sockflgs))) {
    printf("Could not set socket options: %m\n");
    goto fail;
  }
  else if (getaddrinfo(NULL, USER_PORT, &myinfoin, &myinfo)) {
    printf("gettaddrinfo failed: %m\n");
    freeaddrinfo(myinfo);
    goto fail;
  }
  else if (bind(socket_fd, myinfo->ai_addr, sizeof(struct sockaddr)) < 0) {
    printf("bind failed: %m\n");
    goto fail;
  }
  freeaddrinfo(myinfo);
  else if (listen(socket_fd, 5)) {
    printf("listen failed: %m\n");
    goto fail;
  }

  while ((opt = getopt(argc, argv, "dh")) != -1)
    switch (opt) {
    case 'd':
      syslog(LOG_INFO, "Daemonizing...");
	  daemon(0,0);
      break;
    case '?':
    case 'h':
      printf("Usage: %s [-d]\n-d Run as daemon\n", argv[0]);
	  exit(0);
      break;
    default:
      break;
    }

  FILE *data_fd = fopen(DATA_PATH, "a+");
  if (data_fd == NULL) {
    printf("Could not open data file: %m\n");
    goto fail;
  }
  while (1) {
    int buf_size = 1;
  
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    int client_fd = accept(socket_fd, (struct sockaddr *)&addr, &addr_len);// Accept connection
    if (client_fd < 0) {
      printf("accept failed: %m\n");
      goto fail;
    }
    char client_addr[INET_ADDRSTRLEN] = "";
    inet_ntop(AF_INET, &(addr.sin_addr), client_addr, INET_ADDRSTRLEN);
    syslog(LOG_INFO, "Accepted connection from %s", client_addr);

    char *msgbuf = malloc(sizeof(char));
    *msgbuf = '\0';
    
    // Receive data
    while (1) {
      char recv_buf[BUFFER_SIZE + 1];
      int recvd = recv(client_fd, recv_buf, BUFFER_SIZE, 0);
      if (recvd == -1) {
        syslog(LOG_ERR, "Error receiving: %m");
        close(client_fd);
        goto fail;
      } 
      else if (!recvd) {
        syslog(LOG_INFO, "Closed connection from %s", client_addr);
        break;
      }
      recv_buf[recvd] = '\0';
      char *str = recv_buf;
      char *token = strsep(&str, "\n");
      
      if (str == NULL) {
        // haven't found the \n yet, more data to come
        buf_size += recvd;
        msgbuf = realloc(msgbuf, buf_size * sizeof(char));
        strncat(msgbuf, token, recvd);
      } 
      else {
        buf_size += strlen(token);
        msgbuf = realloc(msgbuf, buf_size * sizeof(char));
        strcat(msgbuf, token);
        syslog(LOG_INFO, "Received packet: %s", msgbuf);

        // write packetbuffer to file
        if (fprintf(data_fd, "%s\n", msgbuf) < 0) {
          syslog(LOG_ERR, "Error writing to file: %m");
          goto fail1;
        }
        // go to start of file
        if (fseek(data_fd, 0, SEEK_SET) != 0) {
          syslog(LOG_ERR, "Error seeking file: %m");
          goto fail1;
        }

        char *line = NULL;
        size_t line_len = 0;
        ssize_t num_read;

        // send to client
        while ((num_read = getline(&line, &line_len, data_fd)) != -1) {
          if (send(client_fd, line, num_read, MSG_NOSIGNAL) == -1) {
            syslog(LOG_ERR, "Error sending: %m");
            goto fail1;
          }
        }
        free(line);

        // write the data from to next packet at beginning of msgbuf
        buf_size = strlen(str) + 1;
        msgbuf = realloc(msgbuf, buf_size * sizeof(char));
        strcpy(msgbuf, str);
      }
    }
  }

fail1:
  fclose(data_fd);
  remove(DATA_PATH);
  
fail:
  close(socket_fd);
  return -1;
}
