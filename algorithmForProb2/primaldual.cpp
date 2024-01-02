#include <bits/stdc++.h>
using namespace std;

struct PrimalDual
{
  struct Edge
  {
    int from_;
    int to_;
    int flow_;
    int capacity_;
    int cost_;

    int get_dest(int from)
    {
      if (from == from_)
      {
        return to_;
      }
      else
      {
        return from_;
      }
    }

    void add_flow(int from, int to_add)
    {
      if (from == from_)
      {
        flow_ += to_add;
      }
      else
      {
        flow_ -= to_add;
      }
    }

    int get_capacity(int from)
    {
      if (from == from_)
      {
        return capacity_ - flow_;
      }
      else
      {
        return flow_;
      }
    }

    int get_cost_from(int from)
    {
      if (from == from_)
      {
        return cost_;
      }
      else
      {
        return -cost_;
      }
    }
  };

  struct Node
  {
    int index_;
    vector<Edge *> connected_Edges_;
  };

  vector<Node> nodes_;
  deque<Edge> Edges_;

  void addnode()
  {
    nodes_.push_back({int(nodes_.size()), {}});
  }

  void addEdge(int start, int end, int flow, int capacity, int cost)
  {
    Edges_.push_back({start, end, flow, capacity, cost});
    nodes_[start].connected_Edges_.push_back(&Edges_.back());
    nodes_[end].connected_Edges_.push_back(&Edges_.back());
  }

  // Dinic's algorithm with potentials
  int dinic_with_potentials(int source_i, int sink_i, vector<int> &potentials)
  {
    int result = 0;

    auto get_modified_cost = [&](int node_i, Edge *Edge)
    {
      if (node_i == Edge->from_)
        return potentials[Edge->from_] - potentials[Edge->to_] + Edge->cost_;
      return potentials[Edge->to_] - potentials[Edge->from_] - Edge->cost_;
    };

    while (1)
    {
      // divide into layers with BFS
      vector<int> level(nodes_.size(), -1);
      {
        deque<tuple<int, int>> queue(1, make_tuple(0, source_i));
        level[source_i] = 0;

        while (queue.size() > 0)
        {
          int cur_level;
          int cur_node_i;
          tie(cur_level, cur_node_i) = queue.front();
          queue.pop_front();

          for (Edge *Edge : nodes_[cur_node_i].connected_Edges_)
          {
            if (Edge->get_capacity(cur_node_i) > 0 && get_modified_cost(cur_node_i, Edge) == 0)
            {
              int next_i = Edge->get_dest(cur_node_i);
              if (level[next_i] == -1)
              {
                level[next_i] = cur_level + 1;
                queue.push_back(make_tuple(cur_level + 1, next_i));
              }
            }
          }
        }

        if (level[sink_i] == -1)
        {
          return result; // path to sink no longer exists -> return
        }
      }

      // perform DFS to push through the blocking flow
      // never push back flow -> speed up DFS by remembering Edge iteration indices
      vector<int> Edge_index(nodes_.size(), 0);

      function<int(int, int)> dfs = [&](int cur_i, int capacity)
      {
        if (cur_i == sink_i)
        {
          return capacity;
        }

        for (int &i = Edge_index[cur_i]; i < (int)nodes_[cur_i].connected_Edges_.size(); i++)
        {
          Edge *Edge = nodes_[cur_i].connected_Edges_[i];
          int next_i = Edge->get_dest(cur_i);
          if (Edge->get_capacity(cur_i) > 0 && get_modified_cost(cur_i, Edge) == 0 && level[cur_i] < level[next_i])
          {
            int pushed_flow = dfs(next_i, min(capacity, Edge->get_capacity(cur_i)));
            if (pushed_flow > 0)
            {
              Edge->add_flow(cur_i, pushed_flow);
              return pushed_flow;
            }
          }
        }
        return 0;
      };

      int pushed_flow;
      do
      {
        pushed_flow = dfs(source_i, numeric_limits<int>::max());
        result += pushed_flow;
      } while (pushed_flow > 0);
    }
  }

  int primalDual_minCost_maxFlow(int source_i, int sink_i)
  {
    int result = 0;

    while (1)
    {
      // calculate the potentials with Bellman–Ford derivative
      // but it looks kinda like a BFS derivative
      vector<int> potentials(nodes_.size(), numeric_limits<int>::max());
      {
        deque<pair<int, int>> front;
        front.push_back({0, source_i});
        while (front.size() > 0)
        {
          int potential;
          int cur_i;
          tie(potential, cur_i) = front.front();
          front.pop_front();

          if (potential >= potentials[cur_i])
          {
            continue;
          }
          potentials[cur_i] = potential;

          for (Edge *Edge : nodes_[cur_i].connected_Edges_)
            if (Edge->get_capacity(cur_i) > 0)
            {
              // traverse the Edge if there is some remaining capacity
              front.push_back({potential + Edge->get_cost_from(cur_i), Edge->get_dest(cur_i)});
            }
        }
      }

      // next find the zero cost when modifying with potentials
      {
        int flow_pushed = dinic_with_potentials(source_i, sink_i, potentials);
        result += flow_pushed * (potentials[sink_i] - potentials[source_i]); // add the cost of the pushed flow
        if (flow_pushed == 0)
        {
          return result;
        }
      }
    }
  }
};

int main()
{
  PrimalDual graph;
  int vertices = 6;
  for (int i = 0; i < vertices; i++)
  {
    graph.addnode();
  }
  graph.addEdge(0, 1, 0, 8, 1);
  graph.addEdge(0, 2, 0, 10, 1);
  graph.addEdge(1, 3, 0, 4, 1);
  graph.addEdge(1, 4, 0, 2, 1);
  graph.addEdge(2, 4, 0, 5, 1);
  graph.addEdge(3, 5, 0, 5, 1);
  graph.addEdge(4, 3, 0, 3, 1);
  graph.addEdge(4, 5, 0, 8, 1);

  int source = 0, sink = vertices - 1;

  int result = graph.primalDual_minCost_maxFlow(source, sink); // Find the min-cost max-flow
  cout << result << '\n';

  return 0;
}
