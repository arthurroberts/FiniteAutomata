#define DEBUG 1

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

// input can be of different types
// chars or strings say
//template <typename TI>

// AUXILARY FUNCTIONS
void stop(std::string message){
	std::cout<<message<<std::endl;
	int dummy;
	std::cin>>dummy;
}
template <typename T > // concatenate vectors
void add_to_vector(std::vector<T>& v1, std::vector<T> v2){
	v1.insert(v1.end(), v2.begin(), v2.end());								
}

template <typename T > // concatenate vectors and return result
std::vector<T> concat_vectors(std::vector<T> v1, std::vector<T> v2){
    std::vector<T> out;
    out.insert(out.end(), v1.begin(), v1.end());								
	out.insert(out.end(), v2.begin(), v2.end());								
    return out;
}

template <typename T > // find index of certain element
int get_Index(std::vector<T> v,  T K) {
    auto it = find(v.begin(), v.end(), K);
    if (it != v.end()) {     // If element was found
        return it - v.begin();
    }
    else { return -1;    }
}

 // deletes all instances of element "td" in vector "v"
template <typename T > 
void delete_one(std::vector<T>& v,  T td) {
    v.erase( std::remove_if(v.begin(),  v.end(),
                                               [td]( T cur){ return cur == td; }), v.end());
}


// deletes several elements (specified in vector td) from vector v
// deletes all instances of each element in "td"
template <typename T >
void delete_several(std::vector<T>& v,  std::vector<T> td) {
    std::for_each(td.begin(), td.end(), [&v](T to_del) { delete_one(v,to_del);});  
}
 

struct InputLetter{
	char in;
  //  TI in;
	InputLetter(){ };
	InputLetter(char t){in=t;};
    bool operator == (const InputLetter& rhs) ;
	void print(){ std::cout<<in; }
};

 bool InputLetter::operator == (const InputLetter& rhs) {
    return ( this->in  == rhs.in); 
 } 

struct State;  // circular definition for Edge

// 
struct Transition{
	InputLetter input; 
    int sou; // origin state - index in vector of states
	int dest; // destination state - index in vector of states
	State* dest_st_poi; // ditto as pointer
	// output(string) for Mealy machine
    // guard - TBA
    
    // functions    
	Transition(){  }; // default constructor
	Transition(InputLetter i, int d){ input=i; dest = d; };
    bool operator == (const Transition& rhs) ;
	
	Transition set_new(InputLetter i, int d);
	void print(){
		std::cout<<"       tra: input="; input.print();
		std::cout<<"       dest ="<<dest<<std::endl;
	}

};

// transitions equality
bool Transition::operator == (const Transition& rhs) {
    return (( this->sou  == rhs.sou) &&  // is this necessary?
                     (this->dest  == rhs.dest)&&
                     (this->input == rhs.input)
                   );
 } 


Transition Transition::set_new(InputLetter i, int d){
	Transition e(i, d);
	return e;
}


struct State{
	int num; // number  in vector of FA states
	std::vector<Transition> tras; // transitions
	bool accepting;
    void delete_transitions_to(State s);
   // void delete_transitions_to(int dest);
    void delete_transitions(InputLetter l);
	// std::string output; for Moore machine
	void print();
    // returns vector of next states
    std::vector<State> nexts(InputLetter l); 
    bool has_tran_to(State dest); // transition to "dest" exists for some input letter
    void update_transitions(const int thr);
};

// true if there is a transition from this state to state "dest"
bool State::has_tran_to(State dest){
    // check if any of tras.dest is equal to dest
    return std::any_of(tras.begin(), tras.end(),  [dest] (Transition t) { return (t.dest == dest.num )  ;});
 }

// delete all transitions from current state to "sdest"
// can be several of them
void State::delete_transitions_to(State sdest){
    // find all transitions in vector "tras" (for this state) which are going to "sdest"
    std::vector<Transition> trs_to_delete;
    std::copy_if(tras.begin(), tras.end(),std::back_inserter(trs_to_delete),
                          [sdest](Transition t){ return t.dest == sdest.num ;});
    // take transitions found out of "tras" vector 
    delete_several<Transition>(tras , trs_to_delete);     
   
}

// one state (number "thr") was deleted
//int new_dest(int i, int thr){    return i<thr? i : i-1;}

// after state "thr" was deleted - reaim the transitions
// if dest<thr - it stays
// if dest>=thr dest = dest-1
void State::update_transitions(const int thr){
    int c =0;
//    std::cout<<" state number = "<<num<<std::endl;
    std::for_each(tras.begin(), tras.end(), [&c,thr](Transition& t){
  //                           std::cout<<"      c="<<c<<" dest="<<t.dest<<" new dest="<<new_dest(t.dest,thr)<<std::endl;
                             t.dest = (t.dest < thr) ? t.dest : t.dest - 1; 
                            /* c++ */ ;});
}

// for this state delete transitions for input l
void State::delete_transitions(InputLetter l){
    
}

// return vector of next states for this state
std::vector<State> State::nexts(const InputLetter l){
    bool deb = false;
    std::vector<State> next;
    // vector of edges with "l"  an input
    std::vector<Transition> edgs;
    std::copy_if(tras.begin(), tras.end(),   std::back_inserter(edgs),
                          [ &l ](Transition e){  return  (e.input ==l ) ;  });
    if (deb){                      
        std::for_each(edgs.begin(),edgs.end(), 
                                        [] (Transition e) {  std::cout<<" dest="<<e.dest<<std::endl; });
      stop(" debugging state.nexts");                                 
    }
    return next;
}

void State::print(){
	std::cout<<"    state number="<<num;
	if (accepting) std::cout<<"     accepting";
	std::cout<<std::endl;
	for( Transition e : tras){ e.print();}
}



