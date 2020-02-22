#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <json-c/json.h>
#include <json-c/json_tokener.h>

constexpr int kBufferSize = 1024;
constexpr char kGetRequest[] =
    "GET / HTTP/1.1\r\n"
    "Host: 127.0.0.1\r\n\r\n";
// "GET /cake/toppings?name=45 HTTP/1.1\r\n"
// "Host: 127.0.0.1\r\n\r\n";

int socket_connect(char *host, in_port_t port)
{
  struct hostent *hp;
  struct sockaddr_in addr;
  int on = 1, sock;

  if ((hp = gethostbyname(host)) == NULL)
  {
    herror("gethostbyname");
    exit(1);
  }
  bcopy(hp->h_addr, &addr.sin_addr, hp->h_length);
  addr.sin_port = htons(port);
  addr.sin_family = AF_INET;
  sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (const char *)&on, sizeof(int));

  if (sock == -1)
  {
    perror("setsockopt");
    exit(1);
  }

  if (connect(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1)
  {
    perror("connect");
    exit(1);
  }
  return sock;
}

void populate_url_parameters(char *buffer, int shoulder, int elbow)
{
  sprintf(buffer, "/arm?shoulder=%d&elbow=%d", shoulder, elbow);
}

void populate_get_header(char *buffer, char *host_name, char *path)
{
  sprintf(buffer, "GET %s HTTP/1.1\r\n"
                  "Host: %s\r\n"
                  "Connection: keep-alive\r\n\r\n",
          path, host_name);
}

void send_get_request(char *host_name, char *path, int port)
{
  int fd;
  char buffer[kBufferSize];
  fd = socket_connect(host_name, port);

  char request_header[2048];
  populate_get_header(request_header, host_name, path);
  write(fd, request_header, strlen(request_header));
  printf("\n%s\n", request_header);
  bzero(buffer, kBufferSize);
  while (read(fd, buffer, kBufferSize - 1) != 0)
  {
    // to do: handle a timeout for waiting
  }
  char *httpbody = strstr(buffer, "\r\n\r\n");
  if (httpbody)
  {
    printf("we got sumn\n");
    printf("parsed:%s\n", httpbody);
    struct json_object *parse_json;
    struct json_object *wheel;
    struct json_object *subsystem;

    parse_json = json_tokener_parse(httpbody);

    json_object_object_get_ex (parse_json, "wheel", &wheel);
    // printf("%s\n", wheel);

      printf("%d\n", json_object_get_int(wheel));

  }
  else
  {
    // to do: handle if there isnt anything returned
    printf("no\n%s\n", buffer);
  }
  
  shutdown(fd, SHUT_RDWR);
  close(fd);
}

int main(int argc, char *argv[])
{
  if (argc < 5)
  {
    fprintf(stderr, "Usage: %s <hostname> <port> <shoulder> <elbow>\n", argv[0]);
    exit(1);
  }

  char request_path[48] = "/get_command?subsystem=drive";
  // populate_url_parameters(request_path, atoi(argv[3]), atoi(argv[4]));
  send_get_request(argv[1], request_path, atoi(argv[2]));
  return 0;
}
