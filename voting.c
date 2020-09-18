#include<stdio.h>
#include <sys/sem.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include<stdlib.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

typedef struct EVM
{
  int slots;
  int id;
  int slots_occupied;
  pthread_t* e_thread;
}EVM;

typedef struct Voter
{
  int id;
  pthread_t* v_thread;
  int voted;
  int evm_id;
}Voter;

typedef struct booth
{
  int id;
  int evms;
  int voters;
  int done_voters;
  EVM* array_o_evms;
  Voter* array_o_voters;
  int voters_ready;
  int evms_ready;
  pthread_mutex_t* array_o_locks;
}booth;

typedef struct parame
{
  booth* boo;
  int tmpid;
}parame;

pthread_mutex_t iter_protect = PTHREAD_MUTEX_INITIALIZER;

void polling_ready_evm(booth* tbh, int count,int pid)
{
  printf("Polling Ready EVM, %d %d\n", count,pid);
  pthread_mutex_lock(&tbh->array_o_locks[0]); // One EVM is taking control of voters
  int i,j;
  for(i = 0; i < tbh->voters; i++)
  {
    if(tbh->array_o_evms[pid].slots == tbh->array_o_evms[pid].slots_occupied)
    {
      break; //Cant take in more than occupied_slots
    }
    if(tbh->array_o_voters[i].voted == 0 )
    {
      tbh->array_o_voters[i].voted = -1; // Slot found
      tbh->array_o_voters[i].evm_id = pid; // Which EVM has served ??
      tbh->done_voters += 1;
      tbh->array_o_evms[pid].slots_occupied += 1;
    }
  } //Voter is waiting

  pthread_mutex_unlock(&tbh->array_o_locks[0]); //EVM released control
  return;
}

void voter_wait_for_evm(booth* tbh,int pid)
{
  printf("Voter %d wating for EVM\n",pid);
  while(tbh->array_o_voters[pid].voted == 0){sleep(0.1);}
  printf("Voter %d has been assigned an EVM\n",pid);
}

void* init_voters(void* parg)
{
  parame* param = parg;
  booth* the_booth = param->boo;
  int id = param->tmpid;
  the_booth->array_o_voters[id].voted = 0; // Not voted yet...
  printf("Voter %d Initialized\n",id);
  //while(the_booth->evms_ready == 0);
  voter_wait_for_evm(the_booth, id);
}

void* init_evms(void* parg)
{
  parame* param = parg;
  booth* the_booth = param->boo;
  int id = param->tmpid;
  //while(the_booth->array_o_evms[id].slots_occupied == the->booth->array_o_evms[id].slots_occupied)
  printf("EVMS %d Initialized\n",id);
  int count = rand()%10 + 1;
  //int count = 1;
  the_booth->array_o_evms[id].slots = count;
  //while(the_booth->voters_ready == 0);
  polling_ready_evm(the_booth, count,id);

}

void* init_booth(void* the_booth)
{
  int i;
  booth* boo = the_booth;
  int num_voters = boo->voters;
  int num_evms   = boo->evms;
  boo->array_o_evms = malloc(sizeof(EVM)*num_evms);
  boo->array_o_voters = malloc(sizeof(Voter)*num_voters);
  boo->array_o_locks = malloc(sizeof(pthread_mutex_t)*1);
  //boo and the_boooth are equivalent
  for(i = 0; i < 1; i++)
  {
    pthread_mutex_init(&boo->array_o_locks[i],NULL);
  }
  for(i = 0; i < num_voters; i++)
  {
    pthread_mutex_lock(&iter_protect);
    boo->array_o_voters[i].v_thread = malloc(sizeof(pthread_t));
    parame param;
    param.boo = the_booth;
    param.tmpid = i;
    pthread_create(boo->array_o_voters[i].v_thread,NULL,init_voters,&param);
    pthread_mutex_unlock(&iter_protect);
    pthread_join(*boo->array_o_voters[i].v_thread,NULL);
  }
  int j;
  for(j = 0; j < num_evms; j++)
  {
    pthread_mutex_lock(&iter_protect);
    boo->array_o_evms[j].e_thread = malloc(sizeof(pthread_t));
    parame param;
    param.boo = the_booth;
    param.tmpid = j;
    pthread_create(boo->array_o_evms[j].e_thread,NULL,init_evms,&param);
    pthread_mutex_unlock(&iter_protect);
    pthread_join(*boo->array_o_evms[j].e_thread,NULL);
  }
}

int main()
{
  int n;
  int i;
  scanf("%d",&n);
  booth box[n];
  pthread_t* thread_array = malloc(sizeof(pthread_t)*n);
  for(i = 0; i < n;i ++){
  scanf("%d %d",&box[i].voters,&box[i].evms);
  box[i].done_voters = 0;
  }
  for(i = 0; i < n;i ++){
  printf("%d %d\n",box[i].voters,box[i].evms);}
  i = 0;
  box[0].id = 0;
  box[0].evms_ready = 0;
  box[0].voters_ready = 0;
  pthread_create(&thread_array[0],NULL,init_booth,&box[0]);
  pthread_join(thread_array[0],NULL);

  for(i = 0; i < box[0].voters;i ++){
  printf("%d %d %d\n",i,box[0].array_o_voters[i].voted, box[0].array_o_voters[i].evm_id);}
  return 0;
}
