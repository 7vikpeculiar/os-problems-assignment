#include<stdio.h>
#include <sys/sem.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

sem_t mutex;
typedef struct arge{
  int id;
}arge;
void* thread(void* param)
{
    //wait
    int tmp;
    arge* temparg = param;
    sem_wait(&mutex);
    sem_getvalue(&mutex, &tmp);
    printf("%d\n",tmp);
    while(tmp){
      printf("%d\n %d",tmp);
    }
    printf("\nEntered..\n");

    //critical section
    printf("\n%d..\n", temparg->id);
    //signal
    printf("\nJust Exiting...\n");
    sem_post(&mutex);

  }

int main()
{
    sem_init(&mutex, 0, 2);
    arge a,b,c;
    a.id = 1;
    c.id = 2;
    b.id = 4;

    pthread_t t1,t2,t3;
    pthread_create(&t1,NULL,thread,&a);
    pthread_create(&t2,NULL,thread,&b);
    pthread_create(&t3,NULL,thread,&c);
    pthread_join(t1,NULL);
    pthread_join(t2,NULL);
    pthread_join(t3,NULL);
    sem_destroy(&mutex);
    return 0;
}
