
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

void printPath(int parent[], int j) 
{  
    if (parent[j] == -1) 
        return; 
  
    printPath(parent, parent[j]); 
  
    printf("%d ", j); 
} 
int printSolution(int dist[], int n, int parent[],int src) 
{ 
     
    printf("\nRouter \t Distance\tPath"); 
    for (int i = 1; i < nodes; i++) 
    { 
        printf("\n%d -> %d \t %d\t%d ", src, i, dist[i], src); 
        printPath(parent, i); 
    } 
} 
void dijkstra(int graph[nodes][nodes], int src) 
{ 
      
    int dist[nodes];  
    int path[nodes]; 
    int parent[nodes]; 
 
    for (int i = 0; i < nodes; i++) 
    { 
        parent[i] = -1; 
        dist[i] = INT_MAX; 
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
                parent[v] = u; 
                dist[v] = dist[u] + graph[u][v]; 
            }  
    } 
    printSolution(dist, nodes, parent,src); 
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
    for(int i=0;i<nodes;i++)
        dijkstra(mat, i); 
    return 0; 
} 