
#include <stdio.h> 
#include <limits.h> 
     

int nodes;
int minDistance(int dist[], int path[]) 
{ 
      
    int min = INT_MAX, min_index; 
  
    for (int v = 0; v < nodes; v++) 
        if (path[v] == 0 && dist[v] <= min) 
            min = dist[v], min_index = v; 
  
    return min_index; 
} 
int printSolution(int dist[], int n,int src) 
{ 
     
    for (int i = 1; i < nodes; i++) 
    { 
        if(src==0 && i==nodes-1)
        {
            if(dist[i]==9999)
                printf("-1");
            else
                printf("%d ", dist[i]); 
            return 0;
        }
    } 
    printf("-1");
} 
void dijkstra(int graph[nodes][nodes], int src) 
{ 
      
    int dist[nodes];  
    int path[nodes]; 
 
    for (int i = 0; i < nodes; i++) 
    {  
        dist[i] = 9999; 
        path[i] = 0; 
    } 
    dist[src] = 0; 
    for (int count = 0; count < nodes - 1; count++) 
    { 
        int u = minDistance(dist, path); 
        path[u] = 1; 
        for (int v = 0; v < nodes; v++) 

            if (!path[v] && graph[u][v] && dist[u] + graph[u][v] < dist[v]) 
            { 
                dist[v] = dist[u] + graph[u][v]; 
            }  
    } 
    printSolution(dist, nodes,src); 
} 
int main() 
{ 
    int m;
    scanf("%d %d", &nodes,&m);
    int mat[nodes][nodes];

    for(int i=0;i<nodes;i++)
    {
        for(int j=0;j<nodes;j++)
        {
            mat[i][j]=0;
        }
    }
    for(int i=0;i<m;i++)
    {
        int u,v,w;
        scanf("%d %d %d",&u,&v,&w);
        mat[v][u]=w;
        mat[u][v]=w;
    }
    int p;
    scanf("%d",&p);
    for(int i=0;i<nodes;i++)
    {
        for(int j=0;j<nodes;j++)
        {
            if(i==p || j==p)
            {
                mat[i][j]=0;
            }
        }
    }
    dijkstra(mat, 0); 
    return 0; 
} 