#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<unistd.h>
#include <sys/wait.h>
#include<time.h>

void concurrent_merge(int arr[],int l,int m,int r)
{
  int left[10000];
  int right[10000];
  int i,j,k;
  j = 0;
  int len1 = m - l + 1;
  int len2 = r -m;
  for(i = 0;i < len1;i++)
  {
    left[i] = arr[l + i];}
  for(i = 0;i < len2;i++)
  {
    right[i] = arr[m +1+i];}
  i = 0; j = 0;
  int len_left = m - l + 1;
  int len_right = r - m;
  k = l;
  while(i < len_left && j < len_right)
  {
      if(left[i] < right[j])
      {
        arr[k] = left[i];
        i++; k++;
      }
      else
      {
        arr[k] = right[j];
        j++; k++;
      }
  }
  while(i < len_left)
  {
    arr[k] = left[i];
    k++; i++;
  }
  while(j < len_right)
  {
    arr[k] = right[j];
    k++; j++;
  }
  return;
}

void insertion_sort(int arr[], int l,int h)
{
  int i,j,tmp;
  for(i= l+1; i <= h; i++)
  {
    tmp = arr[i];
    j = i - 1;
    while(j >= l && arr[j] > tmp)
    {
      arr[j+1] = arr[j];
      j = j-1;
    }
    arr[j+1] = tmp;
  }
}

void concurrent_merge_sort(int arr[],int l ,int h)
{
  int pid_l;
  int pid_r;
  int stat;
  int diff = h - l + 1;
  if(diff >  5){
  //printf("Merge Sort\n");
  int m = (l+h)/2;
  pid_l = fork();
  if(pid_l == 0)
  {concurrent_merge_sort(arr,l,m); _exit(0);}
  pid_r = fork();
  if(pid_r== 0)
  {concurrent_merge_sort(arr,m + 1,h); _exit(0);}
  waitpid(pid_l, &stat, 0);
  waitpid(pid_r, &stat,0);
  concurrent_merge(arr,l,m,h);
  }
  else
  {
    insertion_sort(arr,l,h);
  }
  return;
}

int main()
{
    int n,i;
    int length = 20000;
    size_t SHM_SIZE = sizeof(int)*length;
    key_t key = IPC_PRIVATE;
    int shared_id = shmget(key,SHM_SIZE, IPC_CREAT | 0666);
    int* shm_array = shmat(shared_id,NULL,0);
    scanf("%d",&n);
  //  printf("n is --> %d <----\n",n);
    int j = 0;
    for(i=0; i < n; i++)
    {
      scanf("%d",&shm_array[i]);
    }
    clock_t begin = clock();
    concurrent_merge_sort(shm_array,0,n-1);
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("%lf\n", time_spent);
    for(i=0; i < n; i++)
    {
      printf("%d ",shm_array[i]);
    }
    printf("\n");
    shmctl(shared_id, IPC_RMID,NULL);
    return 0;
}
