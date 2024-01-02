#include <bits/stdc++.h>
using namespace std;

struct SuccessiveShortestPath
{
  struct Edge
  {
    int start_;
    int end_;
    int flow_;
    int capacity_;
    int cost_;

    int get_dest(int from)
    { // Return the destination node if we were to traverse the Edge from node `from`
      if (from == start_)
      {
        return end_;
      }
      else
      {
        return start_;
      }
    }

    void add_flow(int from, int to_add)
    { // Adds flow from originating vertex `from`
      if (from == start_)
      {
        flow_ += to_add;
      }
      else
      {
        flow_ -= to_add;
      }
    }

    int get_capacity(int from)
    { // Gets the capacity of the edge if the originating vertex is `from`
      if (from == start_)
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
      if (from == start_)
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

  // Successive shortest paths min-cost max-flow algorithm
  // If there is an negative cost cycle initially, then it goes into infinite loop
  int min_cost_max_flow(int source_i, int sink_i)
  {
    int result = 0;

    // First calculate the potentials with Bellman–Ford derivative
    // It starts from a single vertex and is optimized to only operate on active vertices on each layer
    // Thus, it works more like a BFS derivative
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

    // next loop Dijkstra to saturate flow. Once we subtract the difference in potential, every Edge will have a
    // non-negative cost in both directions, so using Dijkstra is safe

    while (1)
    {
      priority_queue<tuple<int, int, Edge *>> frontier;
      vector<bool> explr(nodes_.size(), false);
      vector<int> cost_to_node(nodes_.size(), -1);
      vector<Edge *> Edge_used(nodes_.size(), NULL);
      frontier.push({0, source_i, NULL});

      while (frontier.size() > 0)
      {
        int path_cost;
        int cur_i;
        Edge *cur_Edge_used;
        tie(path_cost, cur_i, cur_Edge_used) = frontier.top();
        path_cost = -path_cost;
        frontier.pop();

        if (!explr[cur_i])
        {
          explr[cur_i] = true;
          Edge_used[cur_i] = cur_Edge_used;
          cost_to_node[cur_i] = path_cost;

          for (Edge *Edge : nodes_[cur_i].connected_Edges_)
            if (Edge->get_capacity(cur_i) > 0)
            {
              int next_i = Edge->get_dest(cur_i);
              // as priority_queue is a max-heap, we use the negative of the path cost for convenience
              // we subtract the difference of potentials from the Edge cost to ensure all Edges have positive cost

              frontier.push({-path_cost - (Edge->get_cost_from(cur_i) - potentials[next_i] + potentials[cur_i]),
                             next_i, Edge});
            }
        }
      }

      if (Edge_used[sink_i] == NULL)
      {
        return result; // didn't find a path -> return
      }
      vector<Edge *> Edges;
      int flow_pushed = numeric_limits<int>::max();
      {
        // we counstruct the path of Edges from source to sink
        int cur_i = sink_i;
        while (cur_i != source_i)
        {
          Edge *Edge = Edge_used[cur_i];
          cur_i = Edge->get_dest(cur_i);
          flow_pushed = min(flow_pushed, Edge->get_capacity(cur_i));
          Edges.push_back(Edge);
        }

        // next push flow back across all the Edges
        for (auto Edge_it = Edges.rbegin(); Edge_it != Edges.rend(); Edge_it++)
        {
          Edge *Edge = *Edge_it;
          Edge->add_flow(cur_i, flow_pushed);
          result += Edge->get_cost_from(cur_i) * flow_pushed;
          cur_i = Edge->get_dest(cur_i);
        }
      }

      // finally, update the potentials so all edge-traversal costs remain non-negative
      for (int i = 0; i < (int)nodes_.size(); i++)
        if (cost_to_node[i] != -1)
        {
          potentials[i] += cost_to_node[i];
        }
    }
  }
};

int main()
{
  SuccessiveShortestPath graph;
  int vertices = 6;
  for (int i = 0; i < vertices; i++)
  {
    graph.addnode();
  }

  // graph.addEdge(0, 2, 4, 4, 1);
  // graph.addEdge(0, 3, 2, 2, 5);
  // graph.addEdge(1, 2, 2, 2, 1);
  // graph.addEdge(1, 3, 2, 6, 1);
  // graph.addEdge(2, 1, 0, 2, 0);
  // graph.addEdge(2, 5, 2, 4, 0);
  // graph.addEdge(3, 4, 4, 8, 1);
  // graph.addEdge(4, 2, 0, 6, -3);
  // graph.addEdge(4, 5, 4, 4, 1);

  //! 31 - 6
  // graph.addEdge(0, 1, 0, 7, 1);
  // graph.addEdge(2, 1, 0, 3, 1);
  // graph.addEdge(0, 2, 0, 4, 1);
  // graph.addEdge(1, 3, 0, 3, 1);
  // graph.addEdge(2, 3, 0, 2, 1);
  // graph.addEdge(3, 4, 0, 3, 1);
  // graph.addEdge(1, 4, 0, 5, 1);
  // graph.addEdge(4, 5, 0, 8, 1);
  // graph.addEdge(3, 5, 0, 5, 1);

  //! 33 - 6
  // graph.addEdge(0, 1, 0, 8, 1);
  // graph.addEdge(0, 2, 0, 10, 1);
  // graph.addEdge(1, 3, 0, 4, 1);
  // graph.addEdge(1, 4, 0, 2, 1);
  // graph.addEdge(2, 4, 0, 5, 1);
  // graph.addEdge(3, 5, 0, 5, 1);
  // graph.addEdge(4, 3, 0, 3, 1);
  // graph.addEdge(4, 5, 0, 8, 1);

  //! 36 - 8
  // graph.addEdge(0, 1, 0, 9, 1);
  // graph.addEdge(0, 2, 0, 5, 1);
  // graph.addEdge(1, 3, 0, 3, 1);
  // graph.addEdge(1, 4, 0, 4, 1);
  // graph.addEdge(2, 3, 0, 2, 1);
  // graph.addEdge(2, 4, 0, 5, 1);
  // graph.addEdge(3, 5, 0, 6, 1);
  // graph.addEdge(3, 6, 0, 3, 1);
  // graph.addEdge(4, 5, 0, 6, 1);
  // graph.addEdge(4, 7, 0, 4, 1);
  // graph.addEdge(5, 7, 0, 6, 1);

  //! 12 - 5
  // graph.addEdge(0, 1, 0, 3, 1);
  // graph.addEdge(0, 2, 0, 2, 1);
  // graph.addEdge(1, 2, 0, 2, 1);
  // graph.addEdge(1, 3, 0, 1, 1);
  // graph.addEdge(2, 3, 0, 3, 1);
  // graph.addEdge(2, 4, 0, 1, 1);
  // graph.addEdge(3, 4, 0, 3, 1);

  //! 11 - 5
  // graph.addEdge(0, 1, 0, 3, 1);
  // graph.addEdge(0, 2, 0, 2, 1);
  // graph.addEdge(1, 2, 0, 2, 1);
  // graph.addEdge(1, 3, 0, 2, 1);
  // graph.addEdge(2, 3, 0, 3, 1);
  // graph.addEdge(2, 4, 0, 1, 1);
  // graph.addEdge(3, 4, 0, 3, 1);

  //! 13 - 6
  graph.addEdge(0, 1, 0, 4, 1);
  graph.addEdge(0, 2, 0, 3, 1);
  graph.addEdge(1, 2, 0, 1, 1);
  graph.addEdge(1, 3, 0, 2, 1);
  graph.addEdge(2, 4, 0, 2, 1);
  graph.addEdge(3, 2, 0, 1, 1);
  graph.addEdge(3, 5, 0, 3, 1);
  graph.addEdge(4, 3, 0, 3, 1);
  graph.addEdge(4, 5, 0, 1, 1);

  //! 14 - 5
  // graph.addEdge(0, 1, 0, 4, 1);
  // graph.addEdge(0, 2, 0, 3, 1);
  // graph.addEdge(1, 2, 0, 1, 1);
  // graph.addEdge(1, 3, 0, 2, 1);
  // graph.addEdge(2, 3, 0, 2, 1);
  // graph.addEdge(2, 4, 0, 1, 1);
  // graph.addEdge(3, 4, 0, 4, 1);

  //! 14 - 5
  // graph.addEdge(0, 1, 0, 4, 1);
  // graph.addEdge(0, 2, 0, 3, 1);
  // graph.addEdge(1, 2, 0, 1, 1);
  // graph.addEdge(1, 3, 0, 2, 1);
  // graph.addEdge(3, 1, 0, 2, 1);
  // graph.addEdge(2, 3, 0, 2, 1);
  // graph.addEdge(2, 4, 0, 1, 1);
  // graph.addEdge(3, 4, 0, 4, 1);

  int source = 0, sink = vertices - 1;

  int result = graph.min_cost_max_flow(source, sink); // Find the min-cost max-flow
  cout << result << '\n';

  return 0;
}