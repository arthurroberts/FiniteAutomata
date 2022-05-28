/ input can be of different types
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

