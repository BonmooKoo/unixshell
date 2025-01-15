#include "ish.h"

volatile time_t last_time = 0;
volatile sig_atomic_t flag = 0;

void handle_sigint(int sig){
    //ignore
}

// void handle_sigquit(int sig) {
//     time_t current_time = time(NULL);

//     if (last_time != 0 && difftime(current_time, last_time) <= 5) {
//         exit(0);
//     }

//     last_time = current_time;
//     printf("\nType Ctrl-\\ again within 5 seconds to exit.\n");
// }

void handle_sigquit(int sig){
    if(flag){
        exit(0);
    }
    flag = 1;
    printf("\nType Ctrl-\\ again within 5 seconds to exit\n");

    alarm(5);
}

void handle_sigalarm(int sig) {
    //reset flag
    flag = 0;
}

void set_sighandler(){
    //SIGINT
    struct sigaction sa_int = {0}; 
    sa_int.sa_handler=handle_sigint;
    sa_int.sa_flags=SA_RESTART;
    sigaction(SIGINT,&sa_int,NULL);

    //SIGQUIT
    struct sigaction sa_quit = {0};
    sa_quit.sa_handler = handle_sigquit;
    sa_quit.sa_flags = SA_RESTART;
    sigaction(SIGQUIT, &sa_quit, NULL);

    //SIGALRM
    struct sigaction sa_alarm = {0};
    sa_alarm.sa_handler = handle_sigalarm;
    sa_alarm.sa_flags = SA_RESTART;
    sigaction(SIGALRM, &sa_alarm, NULL);
}