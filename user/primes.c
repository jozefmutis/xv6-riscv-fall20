#include "kernel/types.h"
#include "user/user.h"

#define R 0
#define W 1

// Redirect standard input or standard output to pipe
void redirect(int k, int p[])
{
    close(k);
    dup(p[k]);
    close(p[R]);
    close(p[W]);
}

void filter(int p)
{
    int n;

    while (read(R, &n, sizeof(n)))
    {
        if (n % p != 0)
        {
            write(W, &n, sizeof(n));
        }
    }
    close(W);
    // very tricky, or the main primes would return after the second process exit.
    wait(0);
    exit(0);
}

// The last process, waitting for a number
void waitForNumber()
{
    int pd[2];
    int p;

    if (read(R, &p, sizeof(p)))
    {
        printf("prime %d\n", p);
        pipe(pd);
        if (fork() == 0)    // child
        {
            redirect(R, pd);
            waitForNumber();
        }
        else    // parent
        {
            redirect(W, pd);
            filter(p);
        }
    }
    exit(0);
}

int main(int argc, char *argv[])
{
    int pd[2];

    pipe(pd);

    if (fork() == 0)    //child
    {
        redirect(R, pd);
        waitForNumber();
    }
    else    // parent
    {
        redirect(W, pd);
        for (int i = 2; i < 36; i++)
        {
            write(W, &i, sizeof(i));
        }
        // close the write side of pipe, cause read returns 0
        close(W);
        // the main primes process should wait until all the other primes process exited
        wait(0);
        exit(0);
    }
    return 0;
}
