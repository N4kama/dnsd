#include <stdio.h>
#include <signal.h>

#include "signal_handler.h"

// SIGINT global
uint8_t g_sigint = 0;

// SIGINT handler
void sigint_handler(int sig)
{
    (void)sig;

    g_sigint = 1;
    printf("triggered CTRL+C, exiting...\n");
}

// Setup signal handlers
void signal_init()
{
    signal(SIGINT, sigint_handler);
}
