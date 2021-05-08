#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define X 0

sem_t s;

void *child(void *arg)
{
    printf("child\n");
    sem_post(&s);
    return NULL;
}

int main(int argc, char *argv[])
{
    sem_init(&s, 0, X);
    printf("parent: begin\n");

    pthread_t c;
    pthread_create(c, NULL, child, NULL);

    sem_wait(&s);
    printf("parent: end\n");
    return 0;
}