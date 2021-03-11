// DVR USING BELLMAN FORD ALGO: 
#include <stdio.h>
#include <stdlib.h>
#include <string.h> /*for a from b to c = a b c*/
void create(int V, int E, int mat[V][V])
{
    int src, dst, weight;
    for (int i = 0; i < V; i++) mat[i][i] = 0;
    for(int i=0;i<E;i++)
    {
        scanf("%d %d %d",&src, &dst, &weight);
        mat[src][dst] = weight;
        mat[dst][src] = weight;
    }
}
void bellmanFord(int V, int E, int mat[V][V], int prev_table[V][V], int final_table[V][V])
{
    for(int i = 0;i < V ; i++)
    {
        for(int j = 0 ; j < V ; j++)
        {
            final_table[i][j]=mat[i][j];
            prev_table[i][j]=i;
            for(int k = 0; k < V ; k ++ )
            {
                if ( ( ( final_table[i][j] > mat[k][j]+mat[i][k] ) || (final_table[i][j] == -1) ) && (  mat[k][j] != -1 && mat[i][k] != -1 ) )
                {
                        final_table[i][j] = mat[k][j]+mat[i][k] ;
                        prev_table[i][j] = k;
                }
            }
            for(int k = 0; k < V ; k++)
            {
                if( ((final_table[i][k]!=-1)&&(final_table[k][j]!=-1)) && (final_table[i][j]==-1) )
                {
                        final_table[i][j]=final_table[i][k]+final_table[k][j];
                        prev_table[i][j]=k;       
                }        
            }
        }
    }
}
void displayroute_table(int V, int final_table[V][V], int prev_table[V][V], int mat[V][V])
{
    int temp;
    for (int i = 0; i < V ; i++)
    {
        printf("Router: %d \n", i);
        printf("Ending\tDistance\tPrevious\n");
        for(int j = 0;j < V; j++)
        {
            (i == prev_table[i][j] ) ? ( temp = -1) : (temp = prev_table[i][j]) ;
            (mat[i][j] != -1 && temp == -1) ? ( printf("%d\t%d \t%d \n", j, final_table[i][j], j )) : ( printf("%d\t%d\t%d \n", j, final_table[i][j], temp));
        }
        printf("\n");
    }
}
int main()
{
    int E, V;
    scanf("%d %d",&V, &E);
    int mat[V][V], route_table[V][V][V], final_table[V][V], prev_table[V][V];
    memset(prev_table, -1, sizeof(prev_table[0][0]) * V * V);
    memset(mat, -1, sizeof(mat[0][0]) * V * V);
    memset(route_table, -1, sizeof(route_table[0][0][0]) * V * V * V);
    create(V, E, mat);
    bellmanFord(V, E, mat, prev_table, final_table);
    displayroute_table(V, final_table, prev_table, mat);
}