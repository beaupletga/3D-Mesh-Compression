#include "../include/step3.h"


void set_central_edge(vector<Diamond> &diamond_list,map<tuple<int,int>,vector<Tetrahedron*>> &edge_dict,
map<int,vector<Tetrahedron*>> &vertex_dict)
{
    unordered_set<int> vertex_taken;
    for(Diamond &diamond : diamond_list)
    {
        if (diamond.get_tetra_list().size()>1)
        {
            unordered_map<int,int> vertex_frequency;
            for(Tetrahedron* tetra : diamond.get_tetra_list())
            {
                for(Vertex* vertex : tetra->get_vertices())
                {
                    vertex_frequency[vertex->get_id()]++;
                }
            }
            vector<int> central_vertices;
            for(pair<int,int> i : vertex_frequency)
            {
                if(i.second>2)
                {
                    central_vertices.push_back(i.first);
                }
            }
            // to check that the diamond is well formed (only 2 vertex appear more than twice)
            assert(central_vertices.size()==2);
            diamond.set_central_edge(pair<int,int>{central_vertices[0],central_vertices[1]});
            vertex_taken.insert(central_vertices[0]);
            vertex_taken.insert(central_vertices[1]);
        }       
    }

    // cout<<"vertex taken : "<<vertex_taken.size()<<endl;
    // unordered_set<int> xx;
    int count_x=0;
    for(pair<int,vector<Tetrahedron*>> vertex : vertex_dict)
    {
        int count_isolated=0;
        if(vertex_taken.count(vertex.first)==0)
        {
            for(Tetrahedron* tetra : vertex.second)
            {
                if(tetra->get_diamond_ref()->get_tetra_list().size()==1)
                {
                    count_isolated++;
                }
            }
            if (count_isolated==0)
            {
                count_x++;
            }
        }
    }
    cout<<"count x : "<<count_x<<endl;


    for(Diamond &diamond : diamond_list)
    {
        if (diamond.get_tetra_list().size()==1)
        {
            int min_value=10000;
            tuple<int,int> min_edge;
            int min_size;
            for(tuple<int,int> edge : diamond.get_tetra_list()[0]->enumerate_edges())
            {
                int d=vertex_taken.count(get<0>(edge))+vertex_taken.count(get<1>(edge));
                if(d<min_value)
                {
                    min_edge=edge;
                    min_value=d;
                    min_size=edge_dict[edge].size();
                }
                else if(d==min_value)
                {
                    if(edge_dict[edge].size()<min_size)
                    {
                        min_edge=edge;
                        min_size=edge_dict[edge].size();
                    }
                }
            }
            diamond.set_central_edge(pair<int,int>{get<0>(min_edge),get<1>(min_edge)});
        }           
    }
}


void cc(vector<Diamond> &diamond_list)
{
    int x=0;
    for(int i=0;i<diamond_list.size();i++)
    {
        if (diamond_list[i].has_anchor)
        {
           x++;
        }
    }
    cout<<"Nb Diamonds with anchor : "<<x<<endl;
}

// this step aims at choosing an anchor vertex for each diamond
// such that any vertex is linked to only one diamond
void step_3(vector<Diamond> &diamond_list,vector<Vertex> &vertex_list,map<int,vector<Tetrahedron*>> &vertex_dict,
map<tuple<int,int>,vector<Tetrahedron*>> &edge_dict)
{
    // foreach vertex, we add to a list the central edges adjacent to it
    map<int,vector<pair<int,int>>> remaining_vertices;
    for(Diamond &diamond : diamond_list)
    {  
        int vertex1 =  diamond.get_central_edge().first;
        int vertex2 =  diamond.get_central_edge().second;
        remaining_vertices[vertex1].push_back(diamond.get_central_edge());
        remaining_vertices[vertex2].push_back(diamond.get_central_edge());
    }

    // while there is a vertex not associated to an central edge and which is adjacent to a non paired central edge
    while(remaining_vertices.size()>0)
    {
        // we choose the vertex with the smallest number of adjacent central edges
        pair<int,vector<pair<int,int>>> it = *min_element(remaining_vertices.begin(),remaining_vertices.end(),
        [](pair<int,vector<pair<int,int>>> i,pair<int,vector<pair<int,int>>> j){return i.second.size()<j.second.size();});
        // we pair this vertex to the first of its adjacent edges        
        pair<int,pair<int,int>> w = {it.first,{it.second[0]}};
        // we add this vertex as an anchor to the diamond of this central edge
        for(Tetrahedron* tetra : edge_dict[w.second])
        {
            if(tetra->get_diamond_ref()->get_central_edge()==it.second[0])
            {
                tetra->get_diamond_ref()->set_anchor_vertex(&vertex_list[it.first]);
                tetra->get_diamond_ref()->has_anchor=true;
                break;
            }
        }

        remaining_vertices.erase(it.first);
        if (w.first==w.second.first && remaining_vertices.count(w.second.second)>0)
        {
            // deleting in map doesn't seem to work so I need to push back all elements except the one i want to delete
            vector<pair<int,int>> x;
            for(int k=0;k<remaining_vertices[w.second.second].size();k++)
            {
                if(remaining_vertices[w.second.second][k]!=w.second)
                {
                    x.push_back(remaining_vertices[w.second.second][k]);
                }
            }
            remaining_vertices[w.second.second]=x;

            if (remaining_vertices[w.second.second].size()==0)
                remaining_vertices.erase(w.second.second);
            
        }
        else if (w.first==w.second.second && remaining_vertices.count(w.second.first)>0)
        {
            vector<pair<int,int>> x;
            for(int k=0;k<remaining_vertices[w.second.first].size();k++)
            {
                if(remaining_vertices[w.second.first][k]!=w.second)
                {
                    x.push_back(remaining_vertices[w.second.first][k]);
                }
            }
            remaining_vertices[w.second.first]=x;

            if (remaining_vertices[w.second.first].size()==0)
                remaining_vertices.erase(w.second.first);
        }
    }
}

// this function aims at associating all unassociated vertices of step_3 (around 0.1% of vertices)
// to do this, for each unpaired vertex, we break an adjacent diamond (containing at least 3 tetra)
// into several diamond of size 1 and put the vertex as the anchor of one of this diamond
void step_3_bis(vector<Diamond> &diamond_list,vector<Vertex> &vertex_list,map<int,vector<Tetrahedron*>> &vertex_dict)
{
    for(Vertex vertex : vertex_list)
    {
        bool is_in=false;
        for(Diamond diamond : diamond_list)
        {
            if (diamond.has_anchor && diamond.get_anchor_vertex()->get_id()==vertex.get_id())
            {
                is_in=true;
                break;
            } 
        }
        if(!is_in)
        {
            bool finished=false;
            
            for(Tetrahedron* tetra : vertex_dict[vertex.get_id()])
            {
                if (tetra->get_diamond_ref()->get_tetra_list().size()>1 && !finished)
                {
                    bool a=false;
                    bool b=false;
                    Diamond diamond1,diamond2;
                    vector<Diamond> c;

                    for(Tetrahedron* tetra2 : tetra->get_diamond_ref()->get_tetra_list())
                    {
                        bool a_taken=false;
                        bool b_taken=false;
                        int v0,v1,v2,v3;
                        v0=tetra2->get_vertices()[0]->get_id();
                        v1=tetra2->get_vertices()[1]->get_id();
                        v2=tetra2->get_vertices()[2]->get_id();
                        v3=tetra2->get_vertices()[3]->get_id();
                        vector<int> v_list={v0,v1,v2,v3};
                        vector<Tetrahedron*> f = {tetra2};                       

                        if (!a)
                        {
                            for (int i=0;i<v_list.size();i++)
                            {
                                if (vertex.get_id()==v_list[i])
                                {
                                    a_taken=true;
                                    diamond1 = Diamond(diamond_list.back().get_id()+1,f,tetra2->get_vertices()[i]);
                                    diamond1.has_anchor=true;
                                    a=true;
                                    break;
                                }
                            }
                        }
                        if (!a_taken && !b)
                        {   
                            int anchor_id=tetra->get_diamond_ref()->get_anchor_vertex()->get_id();
                            for (int i=0;i<v_list.size();i++)
                            {
                                if (anchor_id==v_list[i])
                                {
                                    diamond2 = Diamond(diamond_list.back().get_id()+1,f,tetra->get_diamond_ref()->get_anchor_vertex());
                                    diamond2.has_anchor=true;
                                    b=true;
                                    b_taken=true;
                                    break;
                                }
                            }
                        }
                        else if (!a_taken && !b_taken)
                        {
                            Diamond diamond3 = Diamond(diamond_list.back().get_id()+1,f,tetra->get_diamond_ref()->get_anchor_vertex());
                            c.push_back(diamond3);
                        }


                        if (a && b && !finished)
                        {
                            for (int i=0;i<diamond_list.size();i++)
                            {
                                if (&diamond_list[i]==tetra->get_diamond_ref())
                                {
                                    if (diamond_list[i].has_anchor)
                                    {
                                        diamond_list.push_back(diamond2);
                                    }
                                    else
                                    {
                                        diamond2.has_anchor=false;
                                        c.push_back(diamond2);
                                    }
                                    diamond_list[i]=diamond1;
                                    finished=true;
                                    break;
                                }
                            }
                        }   
                    }
                    if (a && b)
                    {
                        diamond_list.insert( diamond_list.end(), c.begin(), c.end());
                    }
                }
            }
        }   
    }
}



void step_3_ter(vector<Diamond> &diamond_list)
{
    map<tuple<int,int,int>,vector<Diamond*>> external_faces_dict;
    // we add the external faces of each diamond to the dict
    // as soon as a face is shared by 2 tetra, we link the 2 diamonds
    for(Diamond &diamond : diamond_list)
    {
        for (tuple<int,int,int> face : diamond.get_external_faces())
        {
            if (external_faces_dict.count(face)==0)
            {
                external_faces_dict[face]={&diamond};
            }
            else
            {
                external_faces_dict[face].push_back(&diamond);
                external_faces_dict[face][0]->add_neighbour(face,external_faces_dict[face][1]);
                external_faces_dict[face][1]->add_neighbour(face,external_faces_dict[face][0]);
            }
        }
    }
}