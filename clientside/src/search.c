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
 * Modified by:
 *    https://github.com/marius-hel
 * 
 */

#include <openssl/aes.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

unsigned char zero[16];
unsigned char scrambledzero[16];
unsigned char ciphertext[16];
unsigned char key[16];
AES_KEY expanded;

unsigned char sizes[16] = {0};
unsigned char ind[16] = {0};
unsigned char bytes_to_test[16][256];

long long int num_candidates = 1;
long long int counter = 0;

int search(char b)
{
    for (int i = 0; i < sizes[b]; i++)
    {
        ind[b] = i;
        if (b < 15)
        {
            search(b + 1);
        }
        else
        {
            counter++;

            // Progress indicator
            if (counter % 1000 == 0)
            {
                printf("%3.2f%%\r", (float)(counter * 100) / num_candidates);
            }

            for (int k = 0; k < 16; k++)
            {
                key[k] = bytes_to_test[k][ind[k]];
            }

            AES_set_encrypt_key(key, 128, &expanded);
            AES_encrypt(zero, scrambledzero, &expanded);

            if (!memcmp(scrambledzero, ciphertext, 16))
            {
                printf("Key found after %lld tries.\n", counter);
                printf("The key is");
                for (int x = 0; x < 16; ++x)
                    printf(" %02x", key[x]);
                printf(".\n");

                exit(0);
            }
        }
    }
}

int main()
{
    int b;
    int x;
    long long int n;
    int i;

    for (b = 0; b < 16; ++b)
    {
        scanf("%x", &x);
        ciphertext[b] = x;
    }

    while (scanf("%lld", &n) == 1)
    {
        num_candidates *= n;
        scanf("%d", &b);
        sizes[b] = n;
        for (i = 0; i < n; ++i)
        {
            scanf("%x", &x);
            bytes_to_test[b][i] = x;
        }
    }
    printf("%lld candidates\n", num_candidates);
    search(0);;
    return 0;
}