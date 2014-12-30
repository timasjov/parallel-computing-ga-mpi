/**
 * transpose of 1-d array.
 * E,g: (1 2 3 4 5 6 7 8 9 10) => (10 9 8 7 6 5 4 3 2 1)
 */
#define   TOTALELEMS   10000
#define   MAXPROC      128

#include <stdio.h>
#include <omp.h>
 
int main() {
   int n, c, d, a[100], b[100];
   n = TOTALELEMS*MAXPROC;
 
   for (c = 0; c < n ; c++)
      a[c] = c;

   for (c = n - 1, d = 0; c >= 0; c--, d++)
      b[d] = a[c];
 
   return 0;
}
