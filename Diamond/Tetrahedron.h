#ifndef TETRAHEDRON_H
#define TETRAHEDRON_H

#include <iostream>
#include <algorithm>
#include <tuple>
#include <map>
#include "Vertex.h"
using namespace std;

// class Tetrahedron
// {
//     public:
//         Tetrahedron(int,vector<Vertex>&);
//         bool is_adjacent(Tetrahedron&);
//         int get_id();
//         map<tuple<int,int>,vector<Vertex>>& get_edges();
//         void display_vertices_id();
//         vector<Vertex>& get_vertices();
//         vector<Tetrahedron>& get_neighbours();
//         void add_edge_vertex_match(tuple<int,int>,Vertex&);
//     private:
//         int id;
//         vector<Vertex> vertices;
//         vector<Tetrahedron> neighbours;
//         map<tuple<int,int>,vector<Vertex>> edges;
// };

class Tetrahedron
{
    public:
        Tetrahedron(int,vector<Vertex *>);
        // bool is_adjacent(Tetrahedron);
        int get_id();
        void display_vertices_id();
        vector<Vertex*> get_vertices();
        vector<Tetrahedron*> get_neighbours();
        void add_edge_vertex_match(tuple<int,int>,Vertex*);
        vector<tuple<int,int>> enumerate_edges();
    private:
        int id;
        vector<Vertex*> vertices;
        vector<Tetrahedron*> neighbours;
        map<tuple<int,int>,vector<Vertex*>> edges;
};


#endif