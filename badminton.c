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
#define NUM__PLAYERS_NEEDED 2
#define NUM_REFREES_NEEDED 1

int player_slots = 2;
int refree_slots = 1;
int organiser_slot = 1;
int enter = 1, exit_v = 1;

sem_t player_mutex;
sem_t refree_mutex;
sem_t coupled_mutex; // To ensure that all 3 leave organiser at the same time.
sem_t wait_for_players_mutex; // To ensure that refree waits before starting match.

typedef struct person
{
  int id;
  int tid;
  char type;
}person;

void warmUp(person* parg)
{
  int temp1;
  sem_wait(&wait_for_players_mutex);
  sem_getvalue(&wait_for_players_mutex,&temp1);
  printf(ANSI_COLOR_RED "Player(%d) %d warming up \n" ANSI_COLOR_RESET,parg->id, parg->tid);
  sleep(1);
  printf(ANSI_COLOR_RED "Player(%d) %d done warming up\n" ANSI_COLOR_RESET,parg->id,parg->tid);
  sem_post(&wait_for_players_mutex);
  return;
}

void adjustEquipment(person* parg)
{
  int enter_r = 1;
  int temp_r1;
  while(enter_r)
  {
    sem_getvalue(&wait_for_players_mutex,&temp_r1);
    if(temp_r1 == 0)
    {enter_r = 0;}
  }
  printf("Refree(%d) %d adjusting equipment\n",parg->id, parg->tid);
  sleep(0.5);
  int temp2_r;
  int key_r = 1;
  printf("Refree(%d) %d done with adjusting equipment\n", parg->id,parg->tid);
  while(key_r)
  {
    sem_getvalue(&wait_for_players_mutex,&temp2_r);
    if(temp2_r == 2)
    {key_r = 0;}
  }
  printf(ANSI_COLOR_GREEN "Refree(%d) %d starting match\n" ANSI_COLOR_RESET, parg->id,parg->tid);
  return;
}

void enterCourt(person* parg)
{
  if(parg->type == 'p')
  {
    printf(ANSI_COLOR_MAGENTA "Player(%d) %d entered court\n" ANSI_COLOR_RESET,parg->id,parg->tid);
    warmUp(parg);
  }
  else if(parg->type == 'r')
  {
    printf(ANSI_COLOR_MAGENTA "Refree(%d) %d entered court\n" ANSI_COLOR_RESET,parg->id,parg->tid);
    adjustEquipment(parg);
  }
  return;
}

void meetOrganiser(person* parg)
{
  if(parg->type == 'p')
  {
    sem_wait(&player_mutex);
  }
  else if(parg->type == 'r')
  {
    sem_wait(&refree_mutex);
  }
  int tmp1,tmp2,tmp3,tmp4;
  sem_getvalue(&player_mutex, &tmp1);
  sem_getvalue(&refree_mutex, &tmp2);
  sem_wait(&coupled_mutex);
  while(enter)
  {
    sem_getvalue(&coupled_mutex, &tmp3);
    if(tmp3 == 0)
    {
      enter = 0;
    }
  }
  if(parg->type == 'p')
  { printf(ANSI_COLOR_MAGENTA "Player meets organiser \n" ANSI_COLOR_RESET); }
  else if(parg->type == 'r')
  { printf(ANSI_COLOR_MAGENTA "Refree meets organiser\n" ANSI_COLOR_RESET);  }
    enterCourt(parg);
    sem_post(&coupled_mutex);
    while(exit_v)
    {sem_getvalue(&coupled_mutex, &tmp4);
      if(tmp4 == (player_slots + refree_slots))
      { exit_v = 0; }
    }
    enter = 1; exit_v = 1;
  if(parg->type == 'p')
  {
    sem_post(&player_mutex);
  }
  else if(parg->type == 'r')
  {
    sem_post(&refree_mutex);
  }
  return;
}
void enterAcademy(person* parg)
{
  if(parg->type == 'p')
  {printf(ANSI_COLOR_CYAN "Player(%d) %d, is entering the academy\n" ANSI_COLOR_RESET,parg->id,parg->tid);}
  else
  {printf(ANSI_COLOR_CYAN "Refree(%d) %d, is entering the academy\n" ANSI_COLOR_RESET,parg->id,parg->tid);}
  meetOrganiser(parg);
  return;
}

void* thread(void* parg)
{
    enterAcademy(parg);
    return parg;
}


int main()
{
    int n,i,tim,check = 1,tot;
    scanf("%d",&n);
    int num_players = 2*n;
    int num_refrees = n;
    person person_array[3*n];
    pthread_t thread_array[3*n];
    sem_init(&refree_mutex, 0, refree_slots);
    sem_init(&player_mutex, 0, player_slots);
    sem_init(&coupled_mutex, 0, refree_slots + player_slots); //organiser
    sem_init(&wait_for_players_mutex, 0, player_slots);
    for(i = 0; i < 3*n; i++)
    {
        tim = rand()%3 + 1;
        printf(ANSI_COLOR_RED " i is %d " ANSI_COLOR_RESET,i);
        tot = num_players + num_refrees;
        check = rand()%tot + 1;
        if(check <= num_players)
        {
          person_array[i].id = i;
          person_array[i].tid = 2*n - num_players;
          person_array[i].type = 'p';
          printf(ANSI_COLOR_RED "player(%d) %d\n" ANSI_COLOR_RESET,i,person_array[i].tid); num_players--;
        }
        else
        {
          person_array[i].id = i;
          person_array[i].tid = n - num_refrees;
          person_array[i].type = 'r';
          printf(ANSI_COLOR_RED "refree(%d) %d\n" ANSI_COLOR_RESET,i,person_array[i].tid); num_refrees--;
        }
        pthread_create(&thread_array[i],NULL,thread,&person_array[i]);
    }
    for(i = 0; i < 3*n; i++)
    {
    pthread_join(thread_array[i],NULL);
    }
    //sleep(1);
    return 0;
}
