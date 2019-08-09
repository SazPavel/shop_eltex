#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int shop[5];
int wait = 1;
pthread_mutex_t lock_shop[5];

void *customer(void *ptr)
{
    int *need = (int*)ptr;
    int i, tmp;
    srand(pthread_self());
    while(*need > 0)
    {
        i = rand() % 5;
        pthread_mutex_lock(&lock_shop[i]);
        tmp = shop[i];
        shop[i] = 0;
        pthread_mutex_unlock(&lock_shop[i]);
        *need -= tmp;
        printf("customer: %x, shop: %d, %d need: %d\n", (int)pthread_self(), i, tmp, *need);
        sleep(4);
    }
    printf("EXIT\n");
    return NULL;
}

void *loader(){
    int i, cycle = 1;
    srand(pthread_self());
    while(cycle)
    {
        if(!wait)
        {
            cycle = 0;
            break;
        }
        i = rand() % 5;
        pthread_mutex_lock(&lock_shop[i]);
        shop[i] += 500;
        printf("loader: shop: %d %d\n", i, shop[i]);
        pthread_mutex_unlock(&lock_shop[i]);
        sleep(2);
    }
    return NULL;
}

int main()
{
    pthread_t tid[4];
    int needs[3];
    int i;
    printf("SHOPS: ");
    for(i = 0; i < 5; i++)
    {
        pthread_mutex_init(&lock_shop[i], NULL);
        shop[i] = rand() % 200 + 900;
        printf("%d ", shop[i]);
    }
    printf("\n");
    for(i = 0; i < 3; i++)
    {
        needs[i] = rand() % 2000 + 9000;
        printf("customer %d, need %d\n", i, needs[i]);
        if(pthread_create(&tid[i], NULL, customer, &needs[i]) != 0)
        {
            printf("Can't create thread\n");
            exit(-1);
        }
    }
    if(pthread_create(&tid[i], NULL, loader, NULL) != 0)
    {
        printf("Can't create thread\n");
        exit(-1);
    }
    for(i = 0; i < 3; i++)
        if(pthread_join(tid[i], NULL) != 0)
        {
            printf("Pthread join error\n");
            exit(-1);
        }
    wait = 0;
    if(pthread_join(tid[i], NULL) != 0)
    {
        printf("Pthread join error\n");
        exit(-1);
    }
    printf("SHOPS: ");
    for(i = 0; i < 5; i++)
    {
        printf("%d ", shop[i]);
        pthread_mutex_destroy(&lock_shop[i]);
    }
    exit(0);
}