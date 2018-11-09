#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <iomanip>

/*
 * Text: https://www.codingame.com/ide/puzzle/the-great-dispatch
 *
 * The goal is to dispatch object haveing a weight and volume into bins
 * following two rules:
 *     - Size of the bins
 *     - Minimize the difference between heavier bin and lightest
 *
 * It is a problem of "Bin packing" which is NP-hard
 * Math: https://iuuk.mff.cuni.cz/~sgall/ps/FFabs.pdf
 * -> Wikipedia tells a solution can be found under 30ms
 *
 * More comprehensible: https://www.aaai.org/Papers/AAAI/2002/AAAI02-110.pdf
 */

using namespace std;

const int LIMIT_VOLUME = 99;

/* Template to create a pointer */
template<typename T>
T * ptr(T & obj) { return &obj; } //turn reference into pointer!

template<typename T>
T * ptr(T * obj) { return obj; } //obj is already pointer, return it!

template<typename T>
string selfaddr_to_string(T o)
{
    const void * address = static_cast<const void*>(ptr(o));
    stringstream ss;
    ss << address;  
    string s_address = ss.str();
    
    return s_address;
}

/* Template string function for vectors */
template<class myType>
string toString(const vector<myType> v)
{
    string res = "[";
    
    if (v.size() == 0) {
        res += "empty";   
    } else {
        for (auto o : v) {
            auto p_o = ptr(o);
            res += "\n\t" + p_o->toString() + ",";
        }
        res += "\n";
    }
    
    return res + "]";
}

vector<int> pick_some_samples(int max)
{
    vector<int> v;

    /* Pick some at start, middle and end */
    int last = max - 1;

    v.push_back(0);
    v.push_back(1);
    v.push_back(max/2);
    v.push_back(last - 1);
    v.push_back(last);
    
    return v;
}

template<class myType>
vector<myType> get_sample(vector<myType> v,
                                vector<int> (*picker)(int max))
{
    vector<myType> r;

    auto idx_samples = picker(v.size());
    
    for (auto idx : idx_samples) {
        auto o = v[idx];
        r.push_back(o);
    }
    
    return r;
}

template<class myType>
string sampleToString(vector<myType> v)
{
    string res = "Sample: ";
    
    auto s = get_sample(v, pick_some_samples);
    
    res += toString(s);
    
    return res;
}

/*
template<typename T, typename O>
struct Optimizer {
	T m_cur_val;
	T m_goal;

	O m_object;

	Optimizer(O obj, T goal)
	: m_object(obj),
	m_goal(goal),
	m_cur_val(0)
	{

	}

	T compute(void);
	T update(void);
}

struct Optimizer myOptimizer = Optimizer({1,2,3}, 0);
*/

static int box_id = 0;

struct Box {
    float weight;
    float volume;
    int id;
    
    Box(Box *) {}
    
    Box(float w, float v)
        : weight(w), volume(v), id(box_id++)
    {}
    
    string toString() const
    {
        string res = "";
        
            res = "Box " + to_string(id)
                + "(" + selfaddr_to_string(this)
                + ") [W:" + to_string(weight)
                + " / V:" + to_string(volume)
                + "]";
        return res;
    }
};

std::ostream &operator<< (std::ostream &os, Box const &o) {
    os << o.toString();    return os;}

struct TrackedBox : Box {
    int track_info;

    TrackedBox(float w, float v)
        :  Box(w,  v), track_info(-1)
    {}

    TrackedBox(Box b)
        :  Box(b), track_info(-1)
    {}
    
    void set_track_info(int info)
    {
        track_info = info;   
    }
    
    string toString() const
    {
        string res = "";
        
        res += "" + Box::toString()
            + " tracked -> " + to_string(track_info);
        
        return res;
    }
};

std::ostream &operator<< (std::ostream &os, TrackedBox const &o) {
    os << o.toString();    return os;}

static int truck_id = 0;

struct Truck {
    vector<TrackedBox *> content;
    float weight;
    float volume;
    float max_volume;
    int id;
    
    Truck()
        :
        content(0),
        weight(0),
        volume(0),
        max_volume(LIMIT_VOLUME),
        id(truck_id++)
    {}
    
    float get_space_left()
    {
        return (max_volume - volume);  
    }
    
    bool can_load_box(Box *box)
    {
        return box->volume < get_space_left();
    }

    bool load_box(TrackedBox *box)
    {
        // cerr << "Loading " << box->toString()
        //     << " to " << toString() << endl;
        if (can_load_box(box)) {
            content.push_back(box);
            box->set_track_info(id);
            volume += box->volume;
            weight += box->weight;
            // cerr << "Box loaded : " << toString() << endl;
            return true;
        }
        
        // cerr << "Issue loading " << this->toString()
        //     << " with " << *box << endl;

        return false;
    }
    
    string toString() const
    {
        string res = "Truck " + to_string(id)
            + " (" + selfaddr_to_string(this)
            + ") [W:" + to_string(weight)
            + " / V: " + to_string(volume)
            + " / MV: " + to_string(max_volume)
            + " / Bs: " + to_string(content.size())
            + "] content:" + ::toString(content);
        return res;
    }
};

/* Template to use internal toString function */
std::ostream &operator<< (std::ostream &os, Truck const &o) {
    os << o.toString();    return os;}

template<typename K>
struct StatResult {
	K avg;
	K std;
	K min;
	K max;
	K delta;

	StatResult<K>(K a, K s, K mi, K ma, K d)
	: avg(a),
	std(s),
	min(mi),
	max(ma),
	delta(d)
	{}
};


template<class myType, typename K>
struct StatResult<K> stat(vector<myType> v, K (*get_value)(myType o))
{
    K std = 0;
    K avg = 0;
    K delta = 0;
    
    auto min = numeric_limits<K>::max();
    auto max = numeric_limits<K>::min();

    K w_average = 0;
    for (auto t : v) {
        K w = get_value(t);
        
        min = (w < min) ? w : min;
        max = (w > max) ? w : max;
        
        w_average += w;
    }
    w_average /= v.size();
    avg = w_average;

    K w_std_dev = 0;
    for (auto t : v) {
        K weight = get_value(t);
        K diff = w_average - weight;
        K weight_sq = diff * diff;
        
        w_std_dev += weight_sq;
    }
    w_std_dev /= v.size();
    std = sqrt(w_std_dev);
    
    delta = max - min;

	return StatResult<K>(avg, std, min, max, delta);
}

float get_truck_weight(Truck *o) {
    return o->weight;
};

float get_box_weight(TrackedBox *o) {
    return o->weight;
};

float get_box_volume(TrackedBox *o) {
    return o->volume;
};

void display_box_weight(Box *box, char c, int scale)
{
    for (int i = 0; i < box->weight / scale; i++) {
        cerr << c;
    }
}

void display_truck_load(vector<Truck *> trucks)
{
    /* For each truck (line), we show its boxes (column) */
    char disp[2] = {'|','-'};
    int truck_idx = 0;

    for (auto t : trucks) {
        cerr << "Truck "
            << setw(4) << truck_idx++
            << "(" << setw(8) << t->weight << ")"
            << ":";

        for (auto b : t->content) {
            char c = (c == disp[0]) ? disp[1] : disp[0];
            display_box_weight(b, c, 3);
        }

        cerr << endl;
    }
}

int dummy_distributor(vector<Truck *> trucks, Box *box)
{
(void)trucks;
(void)box;

    return 1;   
}

int fill_distributor(vector<Truck *> trucks, Box *box)
{
    int cnt = 0;

    for (auto t : trucks) {
        if (t->can_load_box(box))
            return cnt;
        
        cnt++;
    }

	return 0;
}

int modulo_distributor(vector<Truck *> trucks, Box *box)
{
    static int call = 0;
    int ret = 0, mod_idx;
    
    mod_idx = call % trucks.size();
    call++;
    
    if (trucks[mod_idx]->can_load_box(box) == false) {
        /* Search for a truck where to load */
    }
    
    return ret;
}

#if 0
struct SortingMode {
    template<typename O>
    bool sort(const O o1, const O o2);
};

struct IncreasingMode : SortingMode {
    template<typename O>
    bool sort(const O o1, const O o2)
    {
        return o1 < o2;
    }
};

struct DecreasingMode : SortingMode {
    template<typename O>
    bool sort(const O o1, const O o2)
    {
        return o1 > o2;
    }
};

bool box_weight(const Box &b1, const Box &b2, SortingMode *sorter)
{
    return sorter->sort(b1.weight, b2.weight);
}
#endif
/*
Idea for distribution:
 - Sort the objects: Heavier first
 - Distribute the weight:
   - 1st round: just fill each truck wit 1 weight -> It is the worst case
   - 2nd rounf: Fill starting from the end until w[n+1] > w{n}
   
 -> The sort will force to have the beter elements to place
 
 - Runtime addition:
    -> Add the object in the sorted array, and just proceed
*/
int idea_distribution(vector<Truck *> &trucks, vector<TrackedBox *> &boxes)
{
    auto sort_truck_weight_increasing = [](const Truck *t1, const Truck *t2){ return t1->weight < t2->weight;};
    auto sort_box_weight_decreasing = [](const Box *b1, const Box *b2){ return b1->weight > b2->weight;};
//    auto sort_box_volume_decreasing = [](const Box *b1, const Box *b2){ return b1->volume > b2->volume;};
//    auto sort_box_mix_decreasing = [](const Box *b1, const Box *b2){ return (b1->weight / b1->volume) > (b2->weight / b2->volume);};

    /* Sort the boxes from heavier to lighter */
    cerr << "prepared: " << sampleToString(boxes) << endl;
    cerr << "Sorting boxes" << endl;
    sort(boxes.begin(), boxes.end(), sort_box_weight_decreasing);
    //cerr << "boxes sorted: " << sampleToString(boxes) << endl;
        
    /*
    1st pass
    Load each truck with a box
    */
    cerr << "1st pass" << endl;
    unsigned int idx = 0;
    auto it_box = boxes.begin();
    for (; it_box != boxes.end(); it_box++) {
        if (idx < trucks.size()) {
            bool loaded = trucks[idx]->load_box(*it_box);
            if (!loaded) {
                cerr << "1st pass (" << idx << ") can't load " << **it_box
                    << "into " << *trucks[idx] << endl;   
                return -1;   
            }
            idx++;
        } else {
            cerr << "All the trucks have been loaded with a box" << endl;
            /* All the trucks have been filled with a box */
            break;   
        }
    }

    cerr << "1st pass: " << sampleToString(trucks) << endl;

    //display_truck_load(trucks);
    cerr << "Sorting  after first pass" <<endl;
    sort(trucks.begin(), trucks.end(), sort_truck_weight_increasing);
    //display_truck_load(trucks);
    
    /* 
    completion
    Load the remaining boxes in the trucks starting to fill the lighter
    
    We can do it by adding a weight to the lighter truck,
    then sorting the set of truck which are before until the lighter
     -> Inserting the truck into the list keeping it sorted
    
    */
    cerr << "2nd pass" << endl;
    for (; it_box != boxes.end(); it_box++) {
        bool loaded;

        for (auto it_truck = trucks.begin(); it_truck != trucks.end(); it_truck++) {

            loaded = (*it_truck)->load_box(*it_box);
            if (loaded) {
                break;
            }
        }

        if (loaded) {
            /* We sort the trucks */
            sort(trucks.begin(), trucks.end(), sort_truck_weight_increasing);
        } else {
            cerr << "Error to load " << *it_box << " into truck:" << endl;
            cerr << sampleToString(trucks) << endl;
            return 1;
        }

        {
            /* Check sort */
        }
    }

    cerr << "2nd pass: " << sampleToString(trucks) << endl;
    
    /* Loop over the boxes inside a truck to add tracking info*/
    // cerr << "NB boxes per truck:" << endl;
    // for (auto t : trucks) {
    //       cerr << " " << t->content.size();
    // }
    // cerr << endl;

	return 0;
}

int load_box(vector<Truck *> trucks, TrackedBox *box)
{
    int truck_to_load, truck_loaded;

    /* Find the truck to laod with the distribution function */
    truck_to_load = fill_distributor(trucks, box);
    
    auto truck = trucks[truck_to_load];
    
    /* Load the truck */
    // cerr << "Loading truck " << truck_to_load
    //     << ": " << truck << endl;
    bool loaded = truck->load_box(box);
    if (!loaded) {
        truck_loaded = -1;
    } else
        truck_loaded = truck_to_load;    
    
    return truck_loaded;
}

Truck * TruckGenerator() {return new Truck();}

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/
int main()
{
    int nb_trucks = 100;
    vector<Truck *> trucks(nb_trucks);
    int boxCount;
    vector<TrackedBox *> tracked_boxes;
    
    //trucks.reserve(nb_trucks);
    generate(trucks.begin(), trucks.end(), TruckGenerator);
cerr << trucks.size() << " trucks prepared" << endl;

    //cerr << "prepared: " << sampleToString(trucks) << endl;
    
    cin >> boxCount; cin.ignore();
    tracked_boxes.reserve(boxCount);

cerr << "Loading " << boxCount << " boxes" << endl;
    for (int i = 0; i < boxCount; i++) {
        float weight;
        float volume;
        cin >> weight >> volume; cin.ignore();
//cerr << "Box " << i << ": " << weight << " / " << volume << endl;

        TrackedBox *box = new TrackedBox(Box(weight, volume));
        tracked_boxes.push_back(box);
//cerr << "Received " << *box << endl;
    }
    
    /* Compute stat on box */
    {
        auto w_stat = stat(tracked_boxes, get_box_weight);
        cerr << "Box weight stat, "
            << "avg: " << w_stat.avg
            << " std: " << w_stat.std
            << " delta: " << w_stat.delta
            << endl;
    } 
    {
        auto v_stat = stat(tracked_boxes, get_box_volume);
        cerr << "Box vo;ume stat, "
            << "avg: " << v_stat.avg
            << " std: " << v_stat.std
            << " delta: " << v_stat.delta
            << endl;
    } 
    
    /* Check if impossible */
        float total_volume = 0;
    for (auto b : tracked_boxes) {
        total_volume += b->volume;
    }
    cerr << "Total volume: " << total_volume << endl;
    if ((total_volume / trucks.size()) > LIMIT_VOLUME) {
        cerr << "There is no solution as the limit per truck is exeeded";   

	return -1;
    }
    
cerr << tracked_boxes.size() << " boxes tracked" << endl;
    
    int distrib = idea_distribution(trucks, tracked_boxes);
    if (distrib != 0) {
        cerr << "Error during distribution" << endl;
        return 2;
    }
    
cerr << "all boxes loaded" << endl;
    
    string res = "";
    for (auto b : tracked_boxes) {
        //cerr << "Box " << b << "track info: " << b->track_info << endl;
        res += to_string(b->track_info) + " ";
        //cerr << "new res: [" << res << "]" << endl;
    }
    
    for (auto t : trucks)
        if (t->volume > LIMIT_VOLUME)
            cerr << "Truck is overloaded "
                << t->volume << " instead of " << LIMIT_VOLUME << endl;

    {
        auto w_stat = stat(trucks, get_truck_weight);
        cerr << "Truck stat, "
            << "avg: " << w_stat.avg
            << " std: " << w_stat.std
            << " delta: " << w_stat.delta
            << endl;
    }

    // Write an action using cout. DON'T FORGET THE "<< endl"
    // To debug: cerr << "Debug messages..." << endl;
    
cerr << "Distribution: [" << res << "]" << endl;

	//display_truck_load(trucks);

    cout << res << endl;
}
