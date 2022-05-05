#include <cstdio>
#include <pthread.h>

int main(int argc, const char** argv)
{
    printf("hello. thread id: %lu\n", pthread_self());
    return 0;
}