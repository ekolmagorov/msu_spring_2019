#include <iostream>
#include <fstream>
#include <algorithm>
#include <mutex>
#include <thread>
#include <string>
#include <limits>

using namespace std;

//-----------GLOBALS-------------------------------------------------

//less than 8MB, make it for more simple testing
const size_t MAX_RAM_SIZE = 32;
size_t BUFFSIZE = MAX_RAM_SIZE/2;
 
mutex filemutex; 
//--------------------------------------------------------------------

size_t getFileLenght(fstream &file)
{
	file.seekg(0,file.end);
	size_t length = static_cast<size_t>(file.tellg());
	file.seekg(0,file.beg);
	return length;
}

void write2file(const vector<uint64_t> &values,
			const size_t first_pos, fstream &file)
{
	file.seekp(first_pos);
	for (auto &value: values)
		file.write(reinterpret_cast<const char *>(&value),
						 sizeof(uint64_t));
	
}


void read_from_file(vector<uint64_t> &values, const size_t first_pos,
					const size_t last_pos, fstream &file)
{
	uint64_t val;
	file.seekg(first_pos);
	while (static_cast<size_t>(file.tellg()) !=\
				 min(last_pos,getFileLenght(file))){

		file.read(reinterpret_cast<char*>(&val),sizeof(uint64_t));
		values.push_back(val);
	}

	return;
}

void output_file(const char * filename)
{
	fstream file;
	file.open(filename, fstream::in);
	uint64_t val;
	file.seekg(0,file.beg);
	size_t lenght = getFileLenght(file);

	while (static_cast<size_t>(file.tellg()) < lenght){

		file.read(reinterpret_cast<char*>(&val),sizeof(uint64_t));
		cout << val <<  endl;

	}

	file.close();
	return;
}

void multithreading_sort(size_t first_pos, size_t last_pos,
			size_t dst_first_pos, ifstream &src, fstream &dst)
{
	/*
		after calling src will have sorted chunks, each chunck have size
		MAX_RAM_SIZE / 2
	*/

	vector<uint64_t> buffer;
	size_t num_chunks = (last_pos - first_pos) / BUFFSIZE;

	if (static_cast<float>(last_pos - first_pos)/BUFFSIZE - num_chunks > 0)
		num_chunks = (last_pos - first_pos) / BUFFSIZE + 1;
	else
		num_chunks = (last_pos - first_pos) / BUFFSIZE;

	for (size_t i = 0; i < num_chunks; i++){

		filemutex.lock(); // read source file can one thread
		src.seekg(i * BUFFSIZE + first_pos);
		while(static_cast<size_t>(src.tellg()) < \
						min((i+1) * BUFFSIZE + first_pos, last_pos)){

			uint64_t val;
			src.read(reinterpret_cast<char*>(&val),sizeof(uint64_t));
			buffer.push_back(val);
		}

		filemutex.unlock();
		sort(buffer.begin(),buffer.end());
		write2file(buffer, i * BUFFSIZE, dst);
		buffer.clear();
	}
		
}

//chunck_size -- size of sorted  array
void merge_from_file(fstream &src,
			const size_t chunk_size, const size_t thread_num)
{
	/*
		merge sorted chuncks
	*/

	if (chunk_size >= getFileLenght(src))
		return ;
	size_t lenght = getFileLenght(src);
	size_t chunks_in_file = lenght/chunk_size;

	if (static_cast<float>(lenght)/chunk_size - chunks_in_file > 0)
		chunks_in_file++;

	string new_filename = to_string(thread_num);

	fstream tmp_file;
	tmp_file.open(new_filename.c_str(), ios::out);
	tmp_file.close();
	tmp_file.open(new_filename.c_str(), ios::binary | ios::out | 
							ios::in | ios::trunc);

	//like in classic algorithm mergesort
	for (size_t i = 0; i < chunks_in_file/2; i++){
		size_t base_left =  2 * i * chunk_size;
		size_t ptr_left = base_left;
		size_t base_right  = (2 * i + 1) * chunk_size;
		size_t ptr_right = base_right;
		uint64_t left, right;

		src.seekg(ptr_left);
		src.read(reinterpret_cast<char *>(&left), sizeof(uint64_t));
		src.seekg(ptr_right);
		src.read(reinterpret_cast<char *>(&right), sizeof(uint64_t));

		while ((ptr_left < chunk_size+base_left) || ((ptr_left < lenght) && 
			(ptr_right < chunk_size + base_right) && (ptr_right < lenght))){

			if (left < right){

				tmp_file.write(reinterpret_cast<char *>(&left),
									sizeof(uint64_t));
				ptr_left += sizeof(uint64_t);
				src.seekg(ptr_left);
				src.read(reinterpret_cast<char * >(&left), sizeof(uint64_t));

			}

			else{

				tmp_file.write(reinterpret_cast<char *>(&right), 
									sizeof(uint64_t));
				ptr_right += sizeof(uint64_t);
				src.seekg(ptr_right);
				src.read(reinterpret_cast<char *>(&right), sizeof(uint64_t));

			}
			if (ptr_left >= chunk_size + base_left){
				left = numeric_limits<uint64_t>::max();
			}
			if (ptr_right >= chunk_size + base_right){
				right = numeric_limits<uint64_t>::max();
			}

			if (ptr_right >= lenght){

				// otherwise fstream doesn't read value

				src.clear();  
				src.seekg(0,src.beg);
				right = numeric_limits<uint64_t>::max();

			}
		}

	}

	if (chunks_in_file % 2 == 1){
		size_t base = chunks_in_file / 2 * 2 * chunk_size;
		uint64_t val;
		
		src.seekg(base);
		while (base < lenght){

			src.read(reinterpret_cast<char *>(&val), sizeof(uint64_t));	
			tmp_file.write(reinterpret_cast<char *>(&val), sizeof(uint64_t));
			base += sizeof(uint64_t);
			src.seekg(base);
		}
	}


	src.close();

	if (thread_num == 1){
		rename(new_filename.c_str(),"first");
	}

	else{
		rename(new_filename.c_str(), "second");
	}

	tmp_file.flush();
	merge_from_file(tmp_file, chunk_size * 2, thread_num);
	
}


void sort_file(const char * SrcFile, const char* DstFile)
{

	// main sorting function, here created two temporary files,
	// which will be removed after calling
 
	ifstream src;
	src.open(SrcFile, fstream::binary | fstream::in);

	if (!src.is_open())
		throw runtime_error("I/O error: Could not open source file");
	
	src.seekg(0,src.end);
	size_t lenght = src.tellg();
	size_t  mid = lenght/(2 * sizeof(uint64_t)) * sizeof(uint64_t);
	src.seekg(0,src.beg);
	
		
	fstream tmp_first;
	tmp_first.open("first", ios::out);
	tmp_first.close();
	tmp_first.open("first", ios::binary | ios::in | ios::out);
	if (!tmp_first.is_open())
		throw runtime_error("I/O error: Could not open destination file");
	
	fstream tmp_second;
	tmp_second.open("second", ios::out);
	tmp_second.close();
	tmp_second.open("second", ios::binary | ios::in | ios::out);
	if (!tmp_second.is_open())
		throw runtime_error("I/O error: Could not open destination file");
	
	filemutex.unlock();


	//devide source file on two  parts and sort chunks in them independly

	thread first_thread  = thread(multithreading_sort, src.beg, mid, 0, 
						 ref(src), std::ref(tmp_first));

	thread second_thread  = thread(multithreading_sort, mid, lenght, 0,
						 ref(src), std::ref(tmp_second));
	

	first_thread.join();
	second_thread.join();

	tmp_first.flush();
	tmp_second.flush();	

		
	first_thread = thread(merge_from_file, ref(tmp_first),  BUFFSIZE, 1);

	second_thread = thread(merge_from_file, ref(tmp_second), BUFFSIZE, 2);
	
	first_thread.join();
	second_thread.join();


	tmp_first.close();
	tmp_second.close();
	
	//change mode, that append second part to first part and call merge sort

	tmp_first.open("first", ios::binary | ios::app | ios::in);
	tmp_second.open("second", ios::binary | ios::in);

	std::copy( 
    		(std::istreambuf_iterator<char>(tmp_second)),
     		std::istreambuf_iterator<char>(),
     		std::ostreambuf_iterator<char>(tmp_first)
	);
	

	tmp_first.flush();
	
	tmp_second.close();
	remove("second");

	merge_from_file(tmp_first, min(mid,lenght - mid), 1);
	
	tmp_first.close();

	rename("first",DstFile);
	src.close();

}


int main(void)
{
	const char *in_file = "test";
	const char *result_file = "result";
	try{
		sort_file(in_file,result_file);
	}
	catch(const runtime_error &error){
		std::cout << error.what() << std::endl;
	}
	output_file(result_file);
	return 0;
}
