#include <stdio.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <signal.h>

#include "ControlBlock.h"
#include <bcm2835.h>

static volatile sig_atomic_t doRun = 1;

extern "C"
{
void sig_handler(int signo)
{
    if ((signo == SIGINT) | (signo == SIGQUIT) | (signo == SIGABRT) | (signo == SIGTERM))
    {
        printf("[ControlBlockService] Releasing input devices.\n");
        doRun = 0;
    }
}
}

void register_signalhandlers()
{
    /* Register signal handlers  */
    if (signal(SIGINT, sig_handler) == SIG_ERR)
    {
        printf("\n[ControlBlockService] Cannot catch SIGINT\n");
    }
    if (signal(SIGQUIT, sig_handler) == SIG_ERR)
    {
        printf("\n[ControlBlockService] Cannot catch SIGQUIT\n");
    }
    if (signal(SIGABRT, sig_handler) == SIG_ERR)
    {
        printf("\n[ControlBlockService] Cannot catch SIGABRT\n");
    }
    if (signal(SIGTERM, sig_handler) == SIG_ERR)
    {
        printf("\n[ControlBlockService] Cannot catch SIGTERM\n");
    }
}

int main(int argc, char** argv)
{
    if (!bcm2835_init())
    {
        std::cout << "Error while initializing BCM2835 library." << std::endl;
        return 1;
    };

    try
    {
        register_signalhandlers();
        ControlBlock controlBlock;
        while (doRun)
        {
            controlBlock.update();
            std::this_thread::sleep_for(std::chrono::milliseconds(25));
        }
    }
    catch (int errno)
    {
        std::cout << "Error while running main loop. Error number: " << errno << std::endl;
    }

    bcm2835_close();

    return 0;
}
