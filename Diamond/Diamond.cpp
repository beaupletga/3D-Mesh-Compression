#include "Diamond.h"

// constructor for each diamond
// we know that the set of tetra shape as a cycle
Diamond::Diamond(int id,vector<Tetrahedron*>& elements,Vertex* anchor_vertex)
{
    this->id=id;
    bool already_in=false;
    this->tetra_list.push_back(elements[0]);
    this->anchor_vertex=anchor_vertex;
    if (elements.size()==1)
    {
        this->neighbours={NULL,NULL,NULL,NULL};
    }
    else
    {
        for(int i=0;i<elements.size()*2;i++)
        {
            this->neighbours.push_back(NULL);
        }
    }
    // we want to add the tetra such that the order is the same as in the cycle around the central edge
   
    while (this->tetra_list.size()!=elements.size())
    {
        for(int i=0;i<elements.size();i++)
        {
            already_in=false;
            // if the new element is adjacent to the last added one
            if (elements[i]->is_adjacent(this->tetra_list.back()))
            {
                // we check that this new element isn't already added
                for (int j=0;j<this->tetra_list.size();j++)
                {
                    if (this->tetra_list[j]->get_id()==elements[i]->get_id())
                    {
                        already_in=true;
                        break;
                    }
                }
                // if it's not already added, then we add it
                if (!already_in)
                {
                    this->tetra_list.push_back(elements[i]);
                    // elements[i]->display_vertices_id();
                    // cout<<elements[i]->get_id()<<endl;
                    break;
                }
            }
        }
    }
}


int Diamond::get_id()
{
    return this->id;
}

vector<Tetrahedron*> Diamond::get_tetra_list()
{
    return this->tetra_list;
}

tuple<int,int> Diamond::get_central_edge()
{
    return this->central_edge;
}

vector<Vertex*> Diamond::get_vertices()
{
    return this->vertices;
}

vector<Diamond*> Diamond::get_neighbours()
{
    return this->neighbours;
}

vector<tuple<int,int,int>> Diamond::get_external_faces()
{
    map<tuple<int,int,int>,int> faces;
    vector<tuple<int,int,int>> external_faces;
    for (Tetrahedron* tetra : this->tetra_list)
    {
        for (tuple<int,int,int> face : tetra->enumerate_faces())
        {
            if (faces.count(face)==0)
            {
                faces[face]=1;
            }
            else
            {
                faces[face]+=1;
            }
        }
    }
    for(pair<tuple<int,int,int>,int> face : faces)
    {
        if (face.second==1)
        {
            // cout<<get<0>(face.first)<<" "<<get<1>(face.first)<<" "<<get<2>(face.first)<<endl;
            external_faces.push_back(face.first);
        }
    }
    return external_faces;
}


void Diamond::add_neighbour(tuple<int,int,int> &face,Diamond* neighbour)
{
    // cout<<this->tetra_list.size()<<endl;
    for (int i=0;i<this->tetra_list.size();i++)
    {
        int similarity=0;
        // cout<<this->tetra_list.size()<<endl;
        // tetra_list[0]->display_vertices_id();
        for (Vertex* vertex : this->tetra_list[i]->get_vertices())
        {
            if (vertex->get_id()==get<0>(face) || vertex->get_id()==get<1>(face) || vertex->get_id()==get<2>(face))
            {
                similarity++;
            }
        }
        if (similarity==3)
        {
            if (get<0>(face)==this->anchor_vertex->get_id() || get<1>(face)==this->anchor_vertex->get_id()
            || get<2>(face)==this->anchor_vertex->get_id())
            {
                this->neighbours[2*i]=neighbour;
            }
            else
            {
                this->neighbours[2*i+1]=neighbour;
            }
        }
    }
}
