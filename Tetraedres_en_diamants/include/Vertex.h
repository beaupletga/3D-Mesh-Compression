#ifndef VERTEX_H
#define VERTEX_H

#include <iostream>
#include <algorithm>
#include <vector>
using namespace std;


class Vertex
{
    public:
        Vertex(int,vector<double>);
        int get_id();
        vector<double> get_coords();
        bool operator==(Vertex);
        void add_neighbours(vector<Vertex*>);
        vector<Vertex *> get_neighbours();
        void display_neighbours();
        void sort_neighbours();
        double angle;

    private: 
        int id;
        vector<double> coords;
        vector<Vertex*> neighbours;
};



#endif