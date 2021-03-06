//  dispatchtest.c -- dispatch messages between local services
//

#include <stdio.h>
#include "o2.h"


#define N_ADDRS 20

#define MAX_MESSAGES 50000

int s = 0;
int w = 1;

void service_one(o2_msg_data_ptr data, const char *types,
                 o2_arg_ptr *argv, int argc, void *user_data)
{
    char p[100];
    sprintf(p, "/two/benchmark/%d", s % N_ADDRS);
    if (s < MAX_MESSAGES) {
        o2_send(p, 0, "i", s);
    }
    if (s % 10000 == 0) {
        printf("Service one received %d messages\n", s);
    }
    s++;
}

void service_two(o2_msg_data_ptr data, const char *types,
                 o2_arg_ptr *argv, int argc, void *user_data)
{
    char p[100];
    sprintf(p, "/one/benchmark/%d", w % N_ADDRS);
    if (w < MAX_MESSAGES) {
        o2_send(p, 0, "i", w);
    }
    if (w % 10000 == 0) {
        printf("Service two received %d messages\n", w);
    }
    w++;
}


int main(int argc, const char * argv[])
{
    // o2_debug_flags("a");
    o2_initialize("test");    
    o2_service_new("one");
    for (int i = 0; i < N_ADDRS; i++) {
        char path[100];
        sprintf(path, "/one/benchmark/%d", i);
        o2_method_new(path, "i", &service_one, NULL, FALSE, FALSE);
    }
    
    o2_service_new("two");
    for (int i = 0; i < N_ADDRS; i++) {
        char path[100];
        sprintf(path, "/two/benchmark/%d", i);
        o2_method_new(path, "i", &service_two, NULL, FALSE, FALSE);
    }

    o2_send("/one/benchmark/0", 0, "i", 0);
    while (s < 50000) {
        o2_poll();
    }
    o2_finish();
    printf("DONE\n");
    return 0;
}
