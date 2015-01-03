/**
 * transpose of 1-d array.
 * E,g: (1 2 3 4 5 6 7 8 9 10) => (10 9 8 7 6 5 4 3 2 1)
 */

#include <time.h>

int main() {
   int n = 1280000;
   int i, c, d, a[n], b[n];
   clock_t start, end;

   start = clock();
   
   for(i =0; i< 1000; i++) {
      for (c = 0; c < n ; c++) {
         a[c] = c;
      }
      for (c = n - 1, d = 0; c >= 0; c--, d++) {
         b[d] = a[c];
      }
   }

   end = clock();
   printf("Number of seconds: %f\n", (end-start)/(double)CLOCKS_PER_SEC );

 
   return 0;
}
