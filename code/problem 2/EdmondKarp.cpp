#include "main.h"

class maxflow
{
public:
    int N, E, S, T;
    vector<vector<int>> graph;
    vector<vector<int>> residual;
    vector<int> parent;
    vector<bool> visited;
    int max_flow = 0;
    void N_and_E(int n, int e)
    {
        N = n;
        E = e;
        graph.resize(N, vector<int>(N, 0));
        residual.resize(N, vector<int>(N, 0));
        parent.resize(N, -1);
        visited.resize(N, false);
    }
    void S_and_T(int s, int t)
    {
        S = s;
        T = t;
    }
    bool bfs()
    {
        fill(visited.begin(), visited.end(), false);
        queue<int> q;
        q.push(S);
        visited[S] = true;
        parent[S] = -1;
        while (!q.empty())
        {
            int u = q.front();
            q.pop();
            for (int v = 0; v < N; v++)
            {
                if (visited[v] == false && residual[u][v] > 0)
                {
                    q.push(v);
                    parent[v] = u;
                    visited[v] = true;
                }
            }
        }
        return (visited[T] == true);
    }
    void EdmondKarp()
    {
        for (int u = 0; u < N; u++)
        {
            for (int v = 0; v < N; v++)
            {
                residual[u][v] = graph[u][v];
            }
        }
        while (bfs())
        {
            int path_flow = INT_MAX;
            for (int v = T; v != S; v = parent[v])
            {
                int u = parent[v];
                path_flow = min(path_flow, residual[u][v]);
            }
            for (int v = T; v != S; v = parent[v])
            {
                int u = parent[v];
                residual[u][v] -= path_flow;
                residual[v][u] += path_flow;
            }
            max_flow += path_flow;
        }
    }
    void addEdge(int from, int to, int capacity)
    {
        graph[from][to] = capacity;
    }
    void print()
    {
        cout << "Max flow: " << max_flow << endl;
    }
};
void playGround(string filename)
{
    ifstream ss(filename);
    string str, x1, x2, from, to, capacity;
    maxflow *mf = new maxflow();
    while (ss >> str)
    {
        if (str == "N_and_E")
        {
            ss >> x1 >> x2;
            mf->N_and_E(stoi(x1), stoi(x2));
        }
        else if (str == "S_and_T")
        {
            ss >> x1 >> x2;
            mf->S_and_T(stoi(x1), stoi(x2));
        }
        else if (str == "END")
        {
            mf->EdmondKarp();
            mf->print();
        }
        else
        {
            ss >> from >> to >> capacity;
            mf->addEdge(stoi(from), stoi(to), stoi(capacity));
        }
    }
}