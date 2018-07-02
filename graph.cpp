#include <iostream>
#include <conio.h>
#include <set>
#include <queue>
#include <vector>
#include <list>
#include <algorithm>
#include <iterator>
#include <sstream>
#define INF 999999
using namespace std;

// 输入：每条边连接的两个节点编号，权值
const string ipt = "0 2 5 "
"0 3 8 "
"3 5 5 "
"3 6 4 "
"3 2 5 "
"5 6 4 "
"2 1 8 "
"2 7 4 "
"6 7 4 "
"1 7 7 "
"1 4 5 "
"7 4 5 "
"7 9 3 "
"7 8 6 "
"9 8 7 "
"6 9 2 "
"4 8 6 "
"#";
istringstream is(ipt);

struct _edge {
	pair<unsigned, unsigned> points;
	unsigned length;
	bool operator<(const _edge& another) const {
		return length < another.length;
	}
	void reverse() {
		swap(points.first, points.second);
	}
};

istream& operator>>(istream& is, _edge& e) {
	is >> e.points.first >> e.points.second >> e.length;
	return is;
}

ostream& operator<<(ostream& os, const _edge& e) {
	os << e.points.first << ' ' << e.points.second << ' ' << e.length;
	return os;
}

struct _graph {
	vector<_edge> edges;
	unsigned point_count;
	vector<set<unsigned>> adjacent_table;
	vector<vector<unsigned>> adjacent_matrix;
	vector<vector<unsigned>> shortest_dist_matrix;
	vector<vector<unsigned>> shortest_path_matrix;
	vector<vector<list<unsigned>>> shortest_path_list;
	void _sort() {
		sort(edges.begin(), edges.end());
	}
	void get_point_count() {
		point_count = edges.front().points.first;
		for (auto i = edges.begin(); i != edges.end(); i++)
			if (i->points.first > point_count) point_count = i->points.first;
			else if (i->points.second > point_count) point_count = i->points.second;
		point_count++;
	}
	void get_adjacent_table() {
		adjacent_table.resize(point_count);
		for (unsigned i = 0; i < point_count; i++) {
			set<unsigned>& res = adjacent_table[i];
			for (auto j = edges.begin(); j != edges.end(); j++)
				if (j->points.first == i) res.insert(j->points.second);
				else if (j->points.second == i) res.insert(j->points.first);
		}
	}
	void get_adjacent_matrix() {
		adjacent_matrix.resize(point_count);
		for (unsigned i = 0; i < point_count; i++) {
			adjacent_matrix[i].resize(point_count, INF);
			adjacent_matrix[i][i] = 0;
		}
		for (auto i = edges.begin(); i != edges.end(); i++)
			adjacent_matrix[i->points.first][i->points.second] = adjacent_matrix[i->points.second][i->points.first] = i->length;
	}
	void Floyd() {
		shortest_path_matrix.resize(point_count);
		for (unsigned i = 0; i < point_count; i++)
			shortest_path_matrix[i].resize(point_count, i);
		shortest_dist_matrix = adjacent_matrix;
		for (unsigned i = 0; i < point_count; i++)
			for (unsigned j = 0; j < point_count; j++)
				for (unsigned k = 0; k < point_count; k++)
					if (shortest_dist_matrix[j][k] > shortest_dist_matrix[j][i] + shortest_dist_matrix[i][k]) {
						shortest_dist_matrix[j][k] = shortest_dist_matrix[j][i] + shortest_dist_matrix[i][k];
						shortest_path_matrix[j][k] = i;
					}
		shortest_path_list.resize(point_count);
		for (unsigned i = 0; i < point_count; i++)
			shortest_path_list[i].resize(point_count);
		for (unsigned i = 0; i < point_count; i++)
			for (unsigned j = 0; j < point_count; j++)
				if (i == j) shortest_path_list[i][j].push_back(i);
				else {
					shortest_path_list[i][j].push_back(i);
					shortest_path_list[i][j].push_back(j);
					unsigned temp = shortest_path_matrix[i][j];
					while (temp != i) {
						auto k = shortest_path_list[i][j].begin(); k++;
						shortest_path_list[i][j].insert(k, temp);
						temp = shortest_path_matrix[i][temp];
					}
				}
	}
	void out_DFS(const unsigned start, ostream& dest) const {
		static vector<bool> visited(point_count);
		visited[start] = true; dest << start << ' '; 
		for (auto i = adjacent_table[start].begin(); i != adjacent_table[start].end(); i++)
			if (!visited[*i]) out_DFS(*i, dest);
	}
	void out_BFS(const unsigned start, ostream& dest) const {
		queue<unsigned> q; q.push(start);
		vector<bool> visited(point_count);
		while (!q.empty()) {
			unsigned now = q.front(); q.pop();
			if (!visited[now]) {
				visited[now] = true; dest << now << ' '; 
				for (auto i = adjacent_table[now].begin(); i != adjacent_table[now].end(); i++)
					if (!visited[*i]) q.push(*i);
			}	
		} 
	}
};

istream& operator>>(istream& is, _graph& g) {
	copy(istream_iterator<_edge>(is), istream_iterator<_edge>(), back_inserter(g.edges));
	g.get_point_count(); g.get_adjacent_table(); g.get_adjacent_matrix();
	return is;
}

struct _tree {
	vector<_edge> edges;
	vector<bool> points;
	multiset<_edge> touched_edges;
	unsigned weight_sum;
	_graph base;
	_tree() {}
	_tree(const _graph& _G) {
        // Kruskal算法获得图的最小生成树
		base = _G;
		points.resize(base.point_count);
		const _edge& the_edge = base.edges.front();
		edges.push_back(the_edge);
		weight_sum = the_edge.length;
		const unsigned& first_point = the_edge.points.first;
		const unsigned& second_point = the_edge.points.second;
		points[first_point] = true;
		points[second_point] = true;
		refresh_touched_edges(first_point);
		refresh_touched_edges(second_point);
		base.edges.erase(base.edges.begin());
		while (!touched_edges.empty()) {
			const unsigned& new_point = touched_edges.begin()->points.second;
			points[new_point] = true;
			const _edge& new_edge = *touched_edges.begin();
			edges.push_back(new_edge);
			weight_sum += new_edge.length;
			refresh_touched_edges(new_point);
		}
	}
	void refresh_touched_edges(const unsigned _new_point) {
		for (auto i = touched_edges.begin(); i != touched_edges.end(); )
			if (i->points.second == _new_point) i = touched_edges.erase(i);	
			else i++;
		for (auto i = base.edges.begin(); i != base.edges.end(); )
			if (i->points.first == _new_point && !points[i->points.second]) {
				touched_edges.insert(*i); i = base.edges.erase(i);
			}	
			else if (i->points.second == _new_point && !points[i->points.first]) {
				i->reverse();
				touched_edges.insert(*i); i = base.edges.erase(i);
			}
			else i++;
	}
	void out_direct(ostream& dest) const { 
		dest << "树中所有的边（按添加先后顺序）：" << endl;
		copy(edges.begin(), edges.end(), ostream_iterator<_edge>(dest, "\n"));
	}
	void out_DFS(ostream& dest) const {
		dest << "最小生成树的深搜序列：";

	}
	void out_BFS(ostream& dest) const {
		dest << "最小生成树的广搜序列：";

	}
	void out_weight_sum(ostream& dest) const {
		dest << "最小生成树的权值之和为" << weight_sum << endl;

	}
};

int main()
{
	_graph G; is >> G;
	G.Floyd();
	cout << "图的深搜序列如下：";
	G.out_DFS(0, cout);
	cout << endl << "图的广搜序列如下：";
	G.out_BFS(0, cout);
	cout << endl;
	G._sort();
	_tree min_tree(G);      // 最小生成树
	min_tree.out_direct(cout);
	min_tree.out_weight_sum(cout);
	return 0;
}
