#include <iostream>
#include <type_traits>

#pragma once

enum class Error
{
	NoError,
	CorruptedArchive
};


class Serializer
{
	static constexpr char Separator =  ' ';
	std::ostream& ss;

public:
	explicit Serializer(std::ostream& out):ss(out){};
	
	template <class T>
	Error save(T& object)
	{
		return object.serialize(*this);
	}

	template <class ... ArgsT>
	Error operator()(ArgsT&&... args){
		return process(std::forward<ArgsT>(args)...);
	}

private:

	Error process(){return Error::NoError;}

	template<class T, class ... ArgsT>
	Error process(T&& val, ArgsT&&... args){

		if (std::is_same<bool,typename\
			 std::remove_reference<decltype(val)>::type>::value){

			if (val==true)
				ss << "true" << Separator;
			else
				ss << "false" << Separator;
		}
		else if (std::is_same<uint64_t, typename\
			 std::remove_reference<decltype(val)>::type>::value){
			ss << val << Separator;
		}
		else{

			return Error::CorruptedArchive;
		}

		return process(std::forward<ArgsT>(args)...);
	}
};


class Deserializer
{
	static constexpr char Separator = ' ';
	std::istream& ss;

public:
	explicit Deserializer(std::istream& in):ss(in){};
	
	template <class T>
	Error load(T& object)
	{
		return object.deserialize(*this);
	}
	
	template <class ... ArgsT>
	Error operator()(ArgsT&&... args){
		return process(std::forward<ArgsT>(args)...);
	}

private:

	template <class T>
	Error process(T&& arg)
	{
		std::string s;
		ss >> s;

		if (std::is_same<bool, typename\
			 std::remove_reference<decltype(arg)>::type>::value){

			if (s == "true"){
				arg = true;
			}
			else if (s == "false"){
				arg = false;
			}
			else
				return Error::CorruptedArchive;
		}

		else if (std::is_same<uint64_t, typename\
			 std::remove_reference<decltype(arg)>::type>::value){

			if (s[0] == '-'){
				return Error::CorruptedArchive;
			}
			arg = atoi(s.c_str());
		}

		else{
			return Error::CorruptedArchive;
		}
		return Error::NoError;

	}	

	template<class T, class ... ArgsT>
	Error process(T&& arg, ArgsT&&... args)
	{
		Error is_error = process(arg); 
		if (is_error==Error::NoError){
			if (ss.get() != Separator)
				return Error::CorruptedArchive;
			return process(std::forward<ArgsT>(args)...);
		}
		else{
			return is_error;
		}
	}
};
