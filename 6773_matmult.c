#include<stdlib.h>
#include<stdio.h>
#include<pthread.h>

#define Item(X, I, J, NCOL)      X[((J) + (I) * (NCOL))]

/*
 * matrix multiplication
 *     C = A*B
 *     A has l rows and m cols
 *     B has m rows and n cols
 *     C has l rows and n cols
 */


void matmult(int* A, int* B, int* C, int l, int m, int n)
{
    for(int i=0; i<l; i++)
        for(int j=0; j<n; j++)
        {
            int sum = 0;
            for(int k=0; k<m; k++)
                sum += Item(A, i, k, m) * Item(B, k, j, n);
            Item(C, i, j, n) = sum;
        }
}

void *mult(void* arg)
{
    int *elements = (int *)arg;
    int sum = 0, i = 0;
    int x = elements[0];
    for (i = 1; i <= x; i++)
           sum += elements[i]*elements[i+x];
    int *p = (int*)malloc(sizeof(int));
    *p = sum;
    pthread_exit(p);
}

/*
 * matrix multiplication
 *     C = A*B
 *     A has l rows and m cols
 *     B has m rows and n cols
 *     C has l rows and n cols
 */
void matmult_v1(int* A, int* B, int* C, int l, int m, int n)
{      
    pthread_t *threads;
    threads = (pthread_t*)malloc(l*n*sizeof(pthread_t));
    int count = 0;
    int* elements = NULL;
    for (int i = 0; i < l; i++){
        for (int j = 0; j < n; j++){
            elements = (int *)malloc((l+n+1)*sizeof(int));
            elements[0] = m;
            for (int k = 0; k < m; k++){
                elements[k+1] = Item(A,i,k,m);
                elements[k+m+1] = Item(B,k,j,n);
            }
            pthread_create(&threads[count++], NULL,mult, (void*)(elements));
        }
    }
    count = 0;
    for (int i = 0; i < l; i++)
    {
      for(int j = 0; j < n; j++){
            void *k;
            pthread_join(threads[count], &k);
            int *p = (int *)k;
            Item(C,i,j,n) = *p;
            count++;
        }
    }
    free(elements);
}

/************************************************************************/

void *mult_rows(void* arg)
{
    int *data = (int *)arg;
    int m = data[0];
    int n = data[1];
    int *B = (int*)malloc(m*n*sizeof(int));
    int *C = (int*)malloc(n*sizeof(int));
    int iteration = (m*n)+m+2;
    int rows=0,cols=0;
    for (int i = m+2; i < iteration; i++)
    {
        Item(B,rows,cols,n) = data[i];
        cols++;
        if(cols%n == 0){
            rows++;
            cols=0;
        } 
    }
    for(int j=0; j<n; j++){
            int sum = 0;
            for(int k=0; k<m; k++)
                sum += data[2+k]*Item(B, k, j, n);
           C[j]= sum;
    }
    pthread_exit(C);
}
/*
 * matrix multiplication
 *     C = A*B
 *     A has l rows and m cols
 *     B has m rows and n cols
 *     C has l rows and n cols
 */
void matmult_v2(int* A, int* B, int* C, int l, int m, int n)
{
    pthread_t *threads;
    threads = (pthread_t*)malloc(l*sizeof(pthread_t));
    for (int i = 0; i < l; i++)
    {
        int* data = (int *)malloc(((m*n)+n+2)*sizeof(int));
        data[0]=m;
        data[1]=n;
        int count=m+2;      
        for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
        {
           data[count ++] = Item(B,i,j,n);
        }
        count=2;
        for (int j = 0; j < m; j++)
        {
           data[count ++] = Item(A,i,j,m);
        }
        pthread_create(&threads[i], NULL,mult_rows, (void*)(data));
    }
    for (int i = 0; i < l; i++)
    {
        void *k;
        //Joining all threads and collecting return value
        pthread_join(threads[i], &k);
        int *p = (int *)k;
        for(int j = 0; j < n; j++){
            Item(C,i,j,n) = p[j];
        }

    }

}
