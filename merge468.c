/* Author: Kevin Cho
 *
 * This is an implementation of merge sort assuming the
 * data is an array a[] of length N, and N is a power of 2.
 *
 * The main() program uses a function called merge() which
 * merges subarrays in sorted order.  Rewrite the program
 * so that it uses threads to do the merging of the
 * subarrays.  In particular, each call to merge() creates
 * a thread.
 *
 * The merging is done in a for-loop.  Each
 * pass through the for-loop causes subarrays of the same size
 * to be merged together.  The mergings within a pass should
 * be done in parallel using one thread per merge. However,
 * the main program must wait for all the threads to complete
 * before doing another pass of the for-loop.  Thus, mergings
 * of subarrays of size 1 can be done in parallel, then
 * mergings of subarrays of size 2 can be done in parallel and
 * so forth.
 *
 * Use the pthread API
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#define N 32

/* Global variables -- be careful */
int a[N];     /* Array to sort */
int temp[N];  /* Temporary storage */

void genvalues(int n); /* Initializes array a[] */
void prnvalues(int n); /* Prints array a[] */
void * merge(void * arrayIndex); /* Merges subarrays */

typedef struct Arr {
    int low;
    int mid;
    int high;
} ArrayIndex;

int main() {

int arrsize;     /* Size of subarrays to merge */
int numarr;      /* Number of subarrays */
int newarrsize;  /* New subarray size */
int newnumarr;   /* New number of subarrays */
int i;
                 /*   at "first+midpt" and ends at "last-1" */

genvalues(N);    /* Initialize a[] with random values */
printf("Initial values:\n");
prnvalues(N);    /* Display the values */

arrsize = 1;

while (arrsize < N) {
   printf("*** Merging subarrays of size %d\n",arrsize);
   arrsize= 2*arrsize; /* merge subarrays to double subarray size */
   pthread_t thread[N/arrsize];
   ArrayIndex aIndex[N/arrsize];
   int k = 0;
   for (i=0; i<N; i+=arrsize) {
      aIndex[k].low = i;
      aIndex[k].mid = aIndex[k].low +(arrsize/2);
      if (aIndex[k].low + arrsize < N) aIndex[k].high = aIndex[k].low + arrsize;
      else aIndex[k].high = N;
      k++;
   }

   for (i=0; i<N/arrsize; i++) pthread_create(&thread[i], NULL, merge, &aIndex[i]);
   for (i=0; i<N/arrsize; i++) pthread_join(thread[i] , NULL);
}

printf("\nOutput:\n");
prnvalues(N); /* Display the sorted values */

}

/*
 * Merges subarrays (a[first], ..., a[midpt-1])
 *    and (a[midpt],..., a[last-1]) into
 *    subarray temp[first],..., temp[last-1] in increasing order.
 *    Then it copies temp[first],..., temp[last-1] back into
 *    a[first],..., a[last-1].
 */
void * merge(void * arrayIndex)
{
    ArrayIndex *pa = (ArrayIndex *) arrayIndex;
    int leftptr;   /* Pointers used in array a[ ] */
    int rightptr;
    int k;         /* pointer used in array temp[ ] */
    int delay;

    /*
     * Do not delete the next three lines.  They cause the function to
     * delay by a amount that is proportional to the subarray it is merging
     */
    if (pa->high-pa->low > 0) delay = pa->high-pa->low;
    else delay = 1;
    usleep(delay*250000);

    leftptr = pa->low;
    rightptr = pa->mid;

    /* Merge values in the two arrays of a[] into temp[] */
    for(k=pa->low; k< pa->high; k++) {
       if (leftptr >= pa->mid) temp[k] = a[rightptr++];
       else if (rightptr >= pa->high) temp[k] = a[leftptr++];
       else if (a[leftptr] < a[rightptr]) temp[k] = a[leftptr++];
       else if (a[leftptr] >= a[rightptr]) temp[k] = a[rightptr++];
       else printf("There's a bug \n");
    }

    /* Copy temp[] back to a[] */
    for(k=pa->low; k<pa->high; k++) a[k] = temp[k];

    pthread_exit(NULL);
}



/*
 * Initializes array a[] with random values.
 */
void genvalues(int n)
{
int k;
int i;
int current;

k = 2*n;
current = 0;
for (i=0; i<n; i++) {
   current = (current*73 + 19)%k;
   a[i] = current;
}
}

/*
 * Prints the values in the array a[]
 */
void prnvalues(int n)
{
int i;
for (i=0; i<n; i++) {
   printf(" %d ", a[i]);
   if ((i+1)%10 == 0) printf("\n");
}
printf("\n");

}
