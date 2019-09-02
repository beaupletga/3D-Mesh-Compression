#include "../include/step1.h"

// This first step consist in gathering tetra into diamonds such that the number of isolated tetra is minimized
// A diamond is a set of tetra sharing an edge and forming a cycle around that edge
// At the end we wish to have a set of edge central edge foreach of these diamond (1 edge -> 1 diamond)
// and one vertex which serves as anchor for the diamond

// take a set of tetra which share an edge and return true if they form a full cycle (i.e diamond)
bool is_cycle(vector<Tetrahedron*>& tetra_list)
{
    map<int,int> count_vertex;
    for (int i=0;i<tetra_list.size();i++)
    {
        for (Vertex* j : tetra_list[i]->get_vertices())
        {
            count_vertex[j->get_id()]+=1;
        }
    }
    for (pair<int,int> i : count_vertex)
    {
        if (i.second<2)
        {
            return false;
        }
    }
    return true;
}

Vertex* pick_random_neighbour(Vertex& vertex)
{
    int vertex_index = rand() % vertex.get_neighbours().size();
    return vertex.get_neighbours()[vertex_index];
}

// to compare the angle for the steepest_neighbour_function
bool cmp(Vertex* lhs, Vertex* rhs)
{
    return lhs->angle > rhs->angle;
}

// careful, this function is unsafe because we don't check the bounds
Vertex* pick_steepest_neighbour(Vertex& vertex,int lala)
{
    vector<Vertex*> neighbours=vertex.get_neighbours();

    for(Vertex * i : neighbours)
    {
        i->angle=atan(i->get_coords()[1]/i->get_coords()[2]);
    }
    
    sort(neighbours.begin(),neighbours.end(),cmp);

    if (lala>=neighbours.size())
    {
        cout<<"Bounds problem"<<endl;
        return neighbours.back();
    }
    else
    {
        return neighbours[lala];
    }
}

bool cmp_edge(pair<tuple<int,int>,vector<Tetrahedron*>>  lhs, pair<tuple<int,int>,vector<Tetrahedron*>> rhs)
{
    int a=0;
    int b=0;
    for (Tetrahedron* tetra : lhs.second)
    {
        for (Tetrahedron* neighbour : tetra->get_neighbours())
        {
            if (neighbour->get_in_diamond())
            {
                a++;
            }
        }
    }
    for (Tetrahedron* tetra : rhs.second)
    {
        for (Tetrahedron* neighbour : tetra->get_neighbours())
        {
            if (neighbour->get_in_diamond())
            {
                b++;
            }
        }
    }
    return a<b;
}


// we need to pick a set of edges such that two edges don't belong to the same tetrahedra
// this function simply pick the edge with the highest number of adjacent tetrahedron
// we use the cmp_edge function for the custom comparator
map<tuple<int,int>,vector<Tetrahedron*>> step_1_edge_degree(vector<Vertex>& vertex_list,vector<Tetrahedron>& tetra_list,map<tuple<int,int>,vector<Tetrahedron*>>& edge_dict)
{
    map<tuple<int,int>,vector<Tetrahedron*>> edge_dict_copy;
    map<tuple<int,int>,vector<Tetrahedron*>> edge_to_tetra;
    map<int,int> vertex_check;
    bool already_matched;

    for (pair<tuple<int,int>,vector<Tetrahedron*>> i : edge_dict)
    {
        if (is_cycle(i.second))
        {
            pair<tuple<int,int>,vector<Tetrahedron*>> tmp={i.first,i.second};
            edge_dict_copy[i.first]=i.second;
        }
    }
    // sort(edge_dict_copy.begin(),edge_dict_copy.end(),cmp_edge);
    int count=0;
    while(edge_dict_copy.size()>0)
    {
        auto maximum = *max_element(edge_dict_copy.begin(),edge_dict_copy.end(),cmp_edge);

        for (Tetrahedron* tetra : maximum.second)
        {
            for (tuple<int,int> edge : tetra->enumerate_edges())
            {
                edge_dict_copy.erase(edge);
            }
            tetra->set_in_diamond(true);
        }
        edge_to_tetra[maximum.first]=maximum.second;
        
        if (edge_to_tetra.size()%1000)
        {
            cout<<(double)edge_to_tetra.size()/edge_dict.size()<<endl;
        }
    }
    return edge_to_tetra;
}

double fitness(map<tuple<int,int>,int>& values,map<tuple<int,int>,vector<Tetrahedron*>>& edge_dict)
{
    double value=0;
    map<int,int> tetra_taken;
    for (pair<tuple<int,int>,int> i : values)
    {
        if (i.second==1)
        {
            value+=edge_dict[i.first].size();
            for (Tetrahedron* tetra : edge_dict[i.first])
            {
                tetra_taken[tetra->get_id()]+=1;
            }
        }
    }
    for (pair<int,int> i : tetra_taken)
    {
        if (i.second>1)
        {
            value-=2*(i.second);
            // value+=1;
        }
    }
    return value;
}


map<tuple<int,int>,vector<Tetrahedron*>> step_1_random(vector<Vertex>& vertex_list,vector<Tetrahedron>& tetra_list,
map<tuple<int,int>,vector<Tetrahedron*>>& edge_dict,map<tuple<int,int>,vector<Tetrahedron*>> &lala)
{
    map<tuple<int,int>,int> values;
    map<tuple<int,int>,int> modified_keys;
    map<tuple<int,int>,vector<Tetrahedron*>> edge_to_tetra;
    // initialization of the values
    for (pair<tuple<int,int>,vector<Tetrahedron*>> i : edge_dict)
    {
        if (is_cycle(i.second))
        {
            if (lala.count(i.first)==1)
            {
                values[i.first]=1;
            }
            else
            {
                values[i.first]=0;
            }
        }
    }

    double fit_before=0;
    for (pair<tuple<int,int>,vector<Tetrahedron*>> i :lala)
    {
        fit_before+=i.second.size();
    }

    cout<<"fit before "<<fit_before/tetra_list.size()<<endl;

    int v=0;
    for (Tetrahedron tetra : tetra_list)
    {
        int c=0;
        for (tuple<int,int> edge : tetra.enumerate_edges())
        {
            if (values[edge]==1)
            {
                c++;
            }
        }
        if (c>1)
        {
            v++;
        }
    }
    cout<<"edge twice "<<v<<endl;


    double fit=0;
    double new_fit=0;
    int i=0;
    double old_fit=0;
    while(true)
    {
        auto it = values.begin();
        std::advance(it, rand() % values.size());
        tuple<int,int> random_key = it->first;
        int count=0;
        for (pair<tuple<int,int>,int> i : values)
        {
            if (rand() / double(RAND_MAX)<(double)1/(0.9*values.size()))
            {
                values[i.first]=1-values[i.first];
                modified_keys[i.first]=0;
                
            }
        }
        // values[random_key]=1-values[random_key];
        // cout<<count<<endl;
        new_fit = fitness(values,edge_dict);

        if (fit>new_fit)
        {
            // values[random_key]=1-values[random_key];
            for (pair<tuple<int,int>,int> key : modified_keys)
            {
                values[key.first]=1-values[key.first];
            }
        }

        if (i%5000==0)
        {
            if (new_fit<old_fit)
            {
                break;
            }
            old_fit=new_fit;
        }
        i++;
        // cout<<"size "<<modified_keys.size()<<endl;
        modified_keys.clear();
        fit=new_fit;
        // cout<<i<<" Fitness : "<<new_fit<<endl;
    }
    unordered_set<int> count;
    for (pair<tuple<int,int>,int> i : values)
    {
        if (i.second==1 && is_cycle(edge_dict[i.first]))
        {
            edge_to_tetra[i.first]=edge_dict[i.first];
            for (Tetrahedron* tetra : edge_dict[i.first])
            {
                count.insert(tetra->get_id());
                tetra->set_in_diamond(true);
            }
        }
    }
    v=0;
    for (Tetrahedron tetra : tetra_list)
    {
        int c=0;
        for (tuple<int,int> edge : tetra.enumerate_edges())
        {
            if (values[edge]==1)
            {
                c++;
            }
        }
        if (c>1)
        {
            v++;
        }
    }
    cout<<"edge twice "<<v<<endl;

    cout<<"Perf : "<<(float)count.size()/tetra_list.size()<<endl;
    cout<<"step 1 done"<<endl;
    return edge_to_tetra;
}



map<tuple<int,int>,vector<Tetrahedron*>> step_1_bfs(vector<Vertex>& vertex_list,vector<Tetrahedron>& tetra_list,map<tuple<int,int>,vector<Tetrahedron*>>& edge_dict)
{
    map<tuple<int,int>,vector<Tetrahedron*>> diamond_list;
    unordered_set<int> visited_tetra;
    queue<Tetrahedron*> wait_list;
    vector<tuple<int,int,int>> ocurrences;

    int first_tetra_index=rand()%tetra_list.size();
    wait_list.push(&tetra_list[first_tetra_index]);
    while(!wait_list.empty())
    {
        // add top tetrahedron
        Tetrahedron* tetra=wait_list.front();
        wait_list.pop();
        // if we haven't go trough it yet
        if (visited_tetra.count(tetra->get_id())==0)
        {
            visited_tetra.insert(tetra->get_id());
            // enumerate the 6 edges of the treta to get the adjacents tetra
            ocurrences.clear();
            for (tuple<int,int> edge : tetra->enumerate_edges())
            {
                // if we haven't already took this edge to form a diamond
                if (diamond_list.count(edge)==0 && edge_dict[edge].size()<=9)
                {
                    // if the tetra around this edge create a diamond
                    if(is_cycle(edge_dict[edge]))
                    {
                        // if none of these tetra is already in a diamond
                        if (count_if(edge_dict[edge].begin(),edge_dict[edge].end(),[](Tetrahedron* i){return i->get_in_diamond();})==0)
                        {
                            // we add the edge into a list, with the nb of tetra associated
                            ocurrences.push_back({get<0>(edge),get<1>(edge),edge_dict[edge].size()});
                        }
                    }
                }
            }
            // we take the edge with maximum adjacent tetra
            if (ocurrences.size()>0)
            {
                tuple<int,int,int> max = *max_element( ocurrences.begin(), ocurrences.end(),
                            []( tuple<int,int,int> &a, tuple<int,int,int> &b )
                            {
                                return get<2>(a) < get<2>(b);
                            } );
                tuple<int,int> edge={get<0>(max),get<1>(max)};
                diamond_list[edge]=edge_dict[edge];
                // and mark all the tetra of this new diamond
                for (Tetrahedron* tmp : edge_dict[edge])
                {
                    tmp->set_in_diamond(true);
                }
            }
            // we push into the queue all tetra adjacent (by an edge) to the focused tetra
            for (Tetrahedron* tmp2 : tetra->get_neighbours())
            {
                wait_list.push(tmp2);
            } 
            // we pop the current tetra
            wait_list.pop();
        }
    }
    return diamond_list;
}


map<tuple<int,int>,vector<Vertex*>> step_1_vertex_choose_neighbour(vector<Vertex>& vertex_list,vector<Tetrahedron>& tetra_list,map<tuple<int,int>,vector<Tetrahedron*>>& edge_dict)
{
    map<tuple<int,int>,vector<Vertex*>> edge_to_vertex;
    for(int i=0;i<vertex_list.size();i++)
    {
        tuple<int,int> tmp;
        int lala=0;
        do
        {
            // Vertex* v2 =pick_random_neighbour(vertex_list[i]);
            Vertex* v2 =pick_steepest_neighbour(vertex_list[i],lala);

            if (vertex_list[i].get_id()<v2->get_id())
            {
                tmp= make_tuple(vertex_list[i].get_id(),v2->get_id());
            }
            else
            {
                tmp= make_tuple(v2->get_id(),vertex_list[i].get_id());
            }
            lala++;
        } while (edge_to_vertex.count(tmp)!=0 and lala<=vertex_list[i].get_neighbours().size());
        vector<Vertex*> vec_tmp={&vertex_list[i]};
        edge_to_vertex.insert({tmp,vec_tmp});
        for(Tetrahedron* tetra : edge_dict[tmp])
        {
            tetra->set_in_diamond(true);
        }
    }
    return edge_to_vertex;
}