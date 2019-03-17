#include <cstdlib>
#include <iostream>
#include "numbers.dat"


int is_simple(const int n)
{

		if (n==1)
				return 0;

		for(int i=2;i*i<=n;i++)
				if (n%i==0)
						return 0;

		return 1;

}

//---------------------------bin_search_functions------------------------------

//find fisrt position left element which is equal val
int find_first(const int * arr,const int start,\
								const int end,const int val)
{


		if ((start==end-1)&&(arr[start]!=val))
				return -1;
		if (arr[(start+end)/2]>val)
				return find_first(arr,start,(start+end)/2,val);

		if (arr[(start+end)/2]<val)
				return find_first(arr,(start+end)/2,end,val);

		if (arr[(start+end)/2]==val){
				int i;
				for (i=(start+end)/2;i>start && (arr[i-1]==val);--i){}
				return i;

		}
		return -1;
}


//find last position rigth elemnt which is equal val
int find_last(const int * arr,const int start,\
								const int end,const int val)
{


		if ((start==end-1)&&(arr[start]!=val))
				return -1;

		if (arr[(start+end)/2]>val)
				return find_last(arr,start,(start+end)/2,val);

		if (arr[(start+end)/2]<val)
				return find_last(arr,(start+end)/2,end,val);

		if (arr[(start+end)/2]==val){
				int i;
				for (i=(start+end)/2;(i<end) && (arr[i+1]==val);++i){}
				return i;

		}
		return -1;
}
//------------------------------------------------------------------------------


int count_simple(const int * arr,const int size,\
					const int start,const int end)
{
		int first_pos=find_first(arr,0,size,start);
		int last_pos=find_last(arr,0,size,end);
		int total=0;


		if ((first_pos==-1) || (last_pos==-1))
				return 0;

		for (int i=first_pos;i<=last_pos;i++)
				total+=is_simple(arr[i]);

		return total;

}


int main(int argc, char * argv[])
{

	int start,end;

	if (((argc-1)%2!=0) || (argc<=1)){
		return -1;

	}
	for (int i=1;i<argc;i+=2){
		start=atoi(argv[i]);
		end=atoi(argv[i+1]);
		std::cout<<count_simple(Data,Size,start,end)<<'\n';

	}

	return 0;
}

