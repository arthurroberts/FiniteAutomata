/*
 * NFA 
  model
*/

#define DEBUG 1

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include  "nfa.h"

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

template <typename T > // concatenate vectors
std::vector<T> concat_vectors(std::vector<T> v1, std::vector<T> v2){
    std::vector<T> out;
    out.insert(out.end(), v1.begin(), v1.end());								
	out.insert(out.end(), v2.begin(), v2.end());								
    return out;
}

// transform_if - combine copy_if (which works for single type)

// input can be of different types
// chars or strings say
//template <typename TI>
struct InputLetter{
	char in;
  //  TI in;
	InputLetter(){ }
	InputLetter(char t){in=t;}
    bool operator == (const InputLetter& rhs) ;
	void print(){ std::cout<<in; }
};

 bool InputLetter::operator == (const InputLetter& rhs) {
    return ( this->in  == rhs.in); 
 } 

struct State;  // circular definition for Edge

// 
struct Transition{
	Transition(){  }; // default constructor
	Transition(InputLetter i, int d){ input=i; dest = d; };
	
	Transition set_new(InputLetter i, int d);
	InputLetter input; 
	int dest; // destination state - index in vector 
	State* dest_st_poi; // ditto as pointer
	// output(string) for Mealy machine
    // guard - TBA
	void print(){
		std::cout<<"       edge: input="; input.print();
		std::cout<<"       dest ="<<dest<<std::endl;
	}

};

Transition Transition::set_new(InputLetter i, int d){
	Transition e(i, d);
	return e;
}

struct State{
	int num; // number  in vector of FA states
	std::vector<Transition> tras; // 
	bool accepting;
    void delete_transitions(InputLetter l);
	// std::string output; for Moore machine
	void print();
    std::vector<State> nexts(InputLetter l); //next states
};

// for this state delete transitions for input l
void State::delete_transitions(InputLetter l){
    
}


// return vector of next states for this state
std::vector<State> State::nexts(InputLetter l){
    bool deb = false;
    std::vector<State> next;
    // vector of edges with "l"  an input
    std::vector<Transition> edgs;
    std::copy_if(tras.begin(), tras.end(),
                          std::back_inserter(edgs),
                          [ &l ](Transition e){  return /*e.input.equal(l)*/ (e.input ==l ) ;  });
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

// describes non-deteministic Finite Automata
struct FA{
    std::vector<State> states; // index in this vector is state label
	std::vector<int> next_states(int cur_state_num,InputLetter input);
    std::vector<State> next_states(State cur_st, InputLetter input);
    // FA maintenace functions
    int add_state(State& s);
    int add_state(std::vector<Transition> es);
    int add_transition(State sou, InputLetter l , int dest);
    int add_transition(int sou, InputLetter l , int dest);
    void print(){ std::cout<< "FA:"<<std::endl; for(State s:states) s.print(); }
};

// returns destinations for current state (by number) and input
std::vector<int> FA::next_states(int cur_st_num, InputLetter l){
   // all tarnsitions from current state
	std::vector<Transition> edges = states.at(cur_st_num).tras;
	// edges with current letter as an input
	std::vector<Transition> edges_inp;
	// filter by input
	std::copy_if(edges.begin(),edges.end(),
	             std::back_inserter(edges_inp), 
				 [l](Transition e){ return    e.input==l   /*e.input.equal(input)*/    ; } );
    std::vector<int> dests(edges_inp.size());
    std::transform(edges_inp.begin(),edges_inp.end(),dests.begin(),
	                    [](Transition e){return e.dest;} );
	return dests;
}

// returns vector of destinations for current state and input
std::vector<State> FA::next_states(State cur_st, InputLetter input){
    // vector of filtered transitions with current letter as an input
	std::vector<Transition> edges_inp;
	// filter all state edges by input letter
	std::copy_if( cur_st.tras.begin(),cur_st.tras.end(),
	                         std::back_inserter(edges_inp), 
				             [input](Transition e){ return (e.input==input); } );
                 
     // copy destinations (as States) for filtered edges into output vector           
    std::vector<State> dests;
    std::for_each(edges_inp.begin(),  edges_inp.end(),
	                                [&dests, this](Transition e){
                                   dests.push_back(  this->states.at(e.dest)  ) ;} );
    return dests;
}

// FA maintenence functions
int FA::add_state(State& s){
    // how many states now?
    s.num = states.size();
    states.push_back(s);
    return 0;
}

//int FA::add_transition(State sou, InputLetter l , State dest){
int FA::add_transition(State sou, InputLetter l , int dest){
    // check that there is destination
    if ( (dest<0) || (dest>(int)states.size() ) ) {
        return -1;
    }
    Transition t{l,dest};
    sou.tras.push_back( t );          
    return 0;
}

// add transition based on numbers
int FA::add_transition(int sou, InputLetter l , int dest){
    // check that there is destination
    if ( (dest<0) || (dest>(int)states.size() ) ) {
        return -1;
    }
    states.at(sou).tras.push_back( (Transition){l,dest} );          
    return 0;
}


// vector of states engaged for given input sequence
struct Trace{
	std::vector<State> trace_states;
	bool active;  //trace not terminated yet
    Trace add_state(State s);
    std::vector<Trace>  grow_trace(FA fa , InputLetter l);
    void print() ;
};


Trace Trace::add_state(State s){
    // duplicate what is was before
    Trace out;
    out.trace_states = trace_states;
    // and add new state
    out.trace_states.push_back(s);
    return out;
}



// modifies current trace in-place
//   
//  if there are no transitions from state for this letter 
//       - make trace non-active
//  if there is one transition - modify this trace, return empty vector {}
//  if this state is origin of more traces -
//  returns additional traces as a vector
std::vector<Trace>  Trace::grow_trace(FA fa , InputLetter l){
    bool ldeb = false;
    if (ldeb) std::cout<<"    Inside grow one trace:"<<std::endl;
    if (ldeb) std::cout<<"     current size of trace="<<trace_states.size()<<std::endl;
    
    if (!active) return {}; // trace is not active, do nothing
    
    // last state in the trace by position in vector
    State last_state = trace_states.at(trace_states.size()-1);
    // vector of destination states for last state in the trace
    std::vector<State> next_states = fa.next_states(last_state,l);
    if (ldeb) { std::cout<<" Last state:"<<std::endl; last_state.print();
                      std::cout<<"     next_states size="<<next_states.size()<<std::endl;
   }
    if (next_states.size()==0) {
        active = false; // no transitions from last state, trace terminates
        return {};
    }
    
      // there is only one transition out                                                           
     if (next_states.size() == 1) {
         // add first of next states to the trace (it is modified in-sito)
         trace_states.push_back(next_states.at(0)); 
         return {};  
     }
      //  for next_states[1..end] form vector of traces to return
      // copy traces as they were
      std::vector<Trace> out;    // to return                              
      std::for_each(next_states.begin()+1, next_states.end(),
                              [&out, this](State s){
                                  Trace n = this->add_state(s);
                                  n.active = true;
                                   out.push_back(n);   
                               });
      // add  state to current trace
      trace_states.push_back(next_states.at(0)); 
  
      if (ldeb) {std::cout<<" Vector of traces to return from grow one trace:"<<std::endl;
                       std::for_each(out.begin() , out.end(), [](Trace t){ t.print() ; } );                  
       }
      // if there are more than 1 next states - duplicate trace
      if (ldeb)  stop("      quiting grow one trace");
  
    return out;
}

// prints state numbers only 
void Trace::print() {
//     std::for_each(trace_states.begin(), trace_states.end(),
//                              [](State s) { s.print(); }) ;
     std::for_each(trace_states.begin(), trace_states.end(),
                              [](State s) {  std::cout<< s.num<<"  "; }) ;
     std::cout<<"  active="<<active<<std::endl;
};

//  set of many traces
struct TracesSet{
    // sequence of input symbols which produces that
    std::vector<InputLetter> input_seq;
	std::vector<Trace> traces; // many traces for NFA
   void print();
   void init(State); // init to single state
   void grow_traces(const FA& fa, InputLetter l);
   void add_trace(Trace t);  //  {traces.push_back(t);}
};

void TracesSet::print(){
    std::cout<<" Printing TracesSet"<<std::endl;
    std::cout<<"  There are "<<traces.size()<<" traces in the set"<<std::endl;
    std::for_each(traces.begin(), traces.end(), []( Trace t) {t.print();}  );
}

// put int initial state (can be one only)
void TracesSet::init(State s){
    Trace t;
    t.trace_states.push_back(s);
    t.active = true;
    traces.push_back(t);    
}

void TracesSet::add_trace(Trace t){
    traces.push_back(t);
    if (DEBUG) std::cout<<" trace added. new size="<<traces.size()<<std::endl;
}


// grow all active traces by state, which is reached with input l
//  1 - make vector of all new traces
//  2 - add theses new traces to the set
void TracesSet::grow_traces(   const FA& fa, InputLetter l  ){
    bool ldeb = false;
    if (ldeb)  {std::cout<<" start grow_traces()";
                      std::cout<<"   Input letter="; l.print(); std::cout<<std::endl; 
                      std::cout<<"   Traces set="; print(); std::cout<<std::endl;
                      stop(" about to start growing all traces");;std::cout<<std::endl;
    }
    
    // for each trace in the set
    // calculate if it produces new traces (grow_trace() returns additional traces)
    // and accumulate these new traces
    std::vector<Trace> traces_to_add;
    std::for_each(traces.begin(), traces.end(),
                         [&traces_to_add, fa ,l ]  (Trace& t) {
                              add_to_vector<Trace>(traces_to_add, t.grow_trace(fa,l)); 
                         });
    // add newly produced traces to the set   
    add_to_vector<Trace>(traces , traces_to_add);
    if (ldeb) { std::cout<<" after processing traces are:"<<std::endl;
                      print();  //print traces set
                      stop(" quiting grow_traces");
    }
    
}

// Run NFA for input word, Start with state 0 (1 initial state)
TracesSet  run_input_seq(const FA& fa , std::vector<InputLetter> input_seq) {
 
     //initialize traces to just one trace which contains one intial state (0)
     TracesSet ts;  
     ts.init(fa.states.at(0)); 
    
     std::cout<<" Running FA"<<std::endl;
     // grow all traces by input letter
      std::for_each(input_seq.begin(),input_seq.end(),
              [&ts, fa] (InputLetter l){  ts.grow_traces(fa,l); });	
        
	   return  ts;  // input did run to the accepted state
}


int main(){
	// input alphabet
   InputLetter a('a');
   InputLetter b('b');

   // make FA
   FA nfa1;
   State s1;
   //Transition e1; 
   /*
   s1.tras.push_back( e1.set_new(a,0)  );
   s1.tras.push_back( e1.set_new(a,1)  );
   s1.tras.push_back( e1.set_new(b,1) );*/
   nfa1.add_state(s1);
   nfa1.add_state(s1);

   std::cout<<nfa1.add_transition( 0, a,0 );
   nfa1.add_transition( 0,  a,1);
   nfa1.add_transition( 0,  b,1);
   
   nfa1.add_transition(1 , a,0);
   nfa1.add_transition( 1 , b,1);
   
   // another node
   //s1.tras.clear();
   //s1.tras.push_back(e1.set_new(a,0));
   //s1.tras.push_back(e1.set_new(b,1));
   //nfa1.add_state(s1);
   
   nfa1.print();
   
   return 0;
   
   // debug Input
   //InputLetter x = a;
   //std::cout<<" Equalitt="<<(x==b)<<std::endl;
   //stop("debug equality");
   
   // debug 
   //std::vector<State> dst = nfa1.next_states( nfa1.states.at(0),a);
   //std::for_each(dst.begin(), dst.end(),
   //                        [](auto s){ std::cout<<" num="<<s.num<<std::endl; ;});
   //stop("Debug nexts");
   
  // make input sequence
   std::vector<InputLetter> input_word{ a,b,b,a , a };
  // run FA
   TracesSet ts =   run_input_seq( nfa1 , input_word);
   ts.print();
}


