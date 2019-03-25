#include <iostream>
#include <deque>
#include <cstdint>

using namespace std;



typedef enum  {START,NUMBER,OPERATION,SIGN} State;
typedef enum {PLUS,MINUS,MUL,DIV} Ops;


//-----------------------------lexic_analisys-------------------
//write numbers in string in deque string_numbers
//write operations in string in deque string_operations

void get_lexems(const char * str, deque<int>& string_numbers,
			deque<Ops>& string_operations,bool& iserror)

{

	State s = START;
	int64_t with_minus = 1;
	int64_t digit_base = 1;

	for (int i=0;;i++){
		
		if (str[i]==0){
			if (s==NUMBER){

				string_numbers.push_back(digit_base*with_minus);
				return;
			}
			else{
				iserror=true;
				return;
			}
		}

		else if ((s == START) && ('0'<=str[i])  && (str[i]<='9')){
			digit_base = (str[i]-'0');
			s = NUMBER;
		}

		else if ((s == START) && (str[i]=='-')){
			s=SIGN;
			with_minus=-1;
		}

		else if (('0'<=str[i]) && (str[i]<='9') && (s==NUMBER)){
			digit_base = 10*digit_base + str[i] - '0';
		}

		else if (('0'<=str[i]) && (str[i]<='9') && (s!=NUMBER)){
			digit_base = (str[i] - '0');
			s=NUMBER;
		}

		else if (((str[i] == '*') || (str[i] == '/') || \
			(str[i] == '+') || (str[i]=='-')) && (s==NUMBER)){

			string_numbers.push_back(digit_base*with_minus);
			digit_base = 0;
			with_minus=1;
			s = OPERATION;

			if (str[i] == '*')
				string_operations.push_back(MUL);

			else if (str[i] == '/')
				string_operations.push_back(DIV);

			else if (str[i]=='+')
				string_operations.push_back(PLUS);

			else if (str[i] == '-')
				string_operations.push_back(MINUS);

		}

		else if((str[i]=='-') && (s==OPERATION)){
			with_minus = -1;
			s = SIGN;
		}
		
		else if(str[i]==' ')
			continue;

		else{
			iserror=true;
			return ;
		}
	}

}

int64_t muldiv(deque<int>& numbers, deque<Ops>& operations,bool& iserror)
{
	int64_t left = numbers.front();
	numbers.pop_front();
	
	for (auto i = operations.begin();i!=operations.end();i++){

		if (*i == MUL){
			operations.pop_front();
			left*=numbers.front();
			numbers.pop_front();
		}
		else if(*i == DIV){
			operations.pop_front();
			if(numbers.front()!=0)
				left/=numbers.front();
			else{
				iserror=true;
				
			}
			numbers.pop_front();
		}
		else
			return left;
	}
	return left;
}

int64_t addsub(deque<int>& numbers, deque<Ops>& operations,bool& iserror){

	int64_t left = muldiv(numbers,operations,iserror);

	for(auto i = operations.begin();i!=operations.end();i++){

		if (*i == PLUS){
			operations.pop_front();
			left += muldiv(numbers,operations,iserror);
			if (iserror)
				return -1;
		}

		else if (*i == MINUS){
			operations.pop_front();
			left -= muldiv(numbers,operations,iserror);
			if (iserror)
				return -1; 

		}
	}

	return left;
}


int main(int argc, char * argv[])
{

	char * str = argv[1];
	bool iserror = false;
	int64_t res;
	deque <int> string_numbers;
	deque <Ops> string_operations;


	if (argc!=2){
		cout<<"error"<<endl;
		return 1;
	}

	get_lexems(str, string_numbers, string_operations,iserror);
	if (iserror){
		cout<<"error"<<endl;
		return 1;
	}
	res=addsub(string_numbers, string_operations,iserror);

	if (iserror){
		cout<<"error"<<endl;
		return 1;
	}
	cout<<res<<endl;
	return 0;
}
