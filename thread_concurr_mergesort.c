#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

typedef struct params
{
  int* arr;
  int l;
  int h;
}params;

void concurrent_merge(int* arr,int l, int m,int r)
{
  // int* arr = paramet->arr;
  // int l = paramet->l;
  // int r = paramet->h;
  // m = (l+r)/2;
  //
  int left[10000];
  int right[10000];
  int i,j,k;
  j = 0;
  int len1 = m - l + 1;
  int len2 = r - m;
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

void insertion_sort(int* arr, int l,int h)
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

void* concurrent_merge_sort(void* inp)
{
  params* inpe = (params*)inp;
  int* arr = inpe->arr;
  int l = inpe->l;
  int h = inpe->h;

  int diff = h - l + 1;

  if(diff >  5){
  int m = (l+h)/2;

  params tmp1, tmp2;

  tmp1.arr =arr; tmp1.l = l; tmp1.h = m;
  tmp2.arr = arr; tmp2.l = m + 1; tmp2.h = h;

  pthread_t t1,t2;
  pthread_create(&t1,NULL,concurrent_merge_sort,&tmp1);
  pthread_create(&t2,NULL,concurrent_merge_sort,&tmp2);

  pthread_join(t1,NULL);
  pthread_join(t2,NULL);

  concurrent_merge(arr,l,m,h);
  }
  else
  { insertion_sort(arr,l,h); }
}

int main()
{
    int n,i;
    scanf("%d",&n);
    int arr[n];
  //  printf("n is --> %d <----\n",n);
    for(i=0; i < n; i++)
    {
      scanf("%d",&arr[i]);
    }
    params inp;
    inp.arr = arr;
    inp.l = 0;
    inp.h = n-1;
    clock_t begin = clock();
    concurrent_merge_sort(&inp);
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("%lf\n", time_spent);
    for(i=0; i < n; i++)
    {
      printf("%d ",arr[i]);
    }printf("\n");
    return 0;
}
