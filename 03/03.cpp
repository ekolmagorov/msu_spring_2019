#include <cstring>
#include <iostream>
#include <new>


using namespace std;

class MatrixRow
{
	size_t length;
	int* row;
	
public:

	MatrixRow():length(0),row(nullptr){};
	MatrixRow(const MatrixRow& origin);
	MatrixRow(const size_t columns);
	bool operator==(const MatrixRow& mr) const;
	bool operator!=(const MatrixRow& mr) const;
	MatrixRow& operator*=(const int val);
	int& operator[](const size_t index);
	void print() const;
	const int& operator[](const size_t index) const;
	~MatrixRow();
};

MatrixRow::MatrixRow(const MatrixRow& origin)
{
	if (this == &origin)
		return;

	if (row != nullptr)
		delete [] row;

	length = origin.length;
	row = new int[length];
	std::memcpy(row,origin.row,sizeof(int)*length);
}

bool MatrixRow::operator==(const MatrixRow& mr) const
{
	if (&mr == this)
		return true;
	if (mr.length != length)
		return false;

	for (size_t i = 0; i < length; i++){
		if (mr.row[i] != row[i]){
			return false;
		}
	}
	return true;
}

bool MatrixRow::operator!=(const MatrixRow& mr) const
{
	if (mr.length != length)
		return false;

	for (size_t i = 0; i < length; i++){

		if (mr.row[i] != row[i]){
			return true;
		}
	}
	return false;
}


MatrixRow::MatrixRow(const size_t columns)
{
	length=columns;
	row  = new int[columns];
}

MatrixRow& MatrixRow::operator*=(const int val)
{
	for(size_t i = 0; i < length; i++){
		row[i]*=val;
	}
	return (*this);
}

int& MatrixRow::operator[](const size_t index)
{
	if (index >= length)
		throw std::out_of_range("Second index is out of matrix size");

	return row[index];
}

const int& MatrixRow::operator[](const size_t index) const
{	
	if (index >= length)
		throw std::out_of_range("Second index is out of matrix size");

	return row[index];
}

void MatrixRow::print() const
{	
	for(size_t i = 0; i < length; i++){
		std::cout<<row[i]<<' ';
	}
	std::cout<<std::endl;
}

MatrixRow::~MatrixRow()
{
	delete [] row;
}

class Matrix
{
	size_t heigth;
	size_t width;
	MatrixRow * ptr;

public:
	Matrix(const size_t r=0, const size_t c=0);
	Matrix(const Matrix& m);
	bool operator==(const Matrix& m) const;
	bool operator!=(const Matrix& m) const;
	Matrix& operator*=(const int val);
	MatrixRow& operator[](const size_t row);
	const MatrixRow& operator[](const size_t row) const;
	size_t getRows() const {return this->heigth;}
	size_t getColumns() const {return this->width;}
	void print() const;
	~Matrix();
};

Matrix::Matrix(const size_t r,const size_t c)
{
	heigth = r;
	width = c;
	if ((heigth != 0) && (width != 0)){
		ptr = static_cast<MatrixRow *>(operator new[](heigth*sizeof(MatrixRow)));
		for (size_t i = 0;i < heigth; i++){
			new (ptr+i)MatrixRow(width);
		}
	}
	else{
		ptr=nullptr;
	}
}

Matrix::Matrix(const Matrix& m)
{
	
	heigth = m.heigth;
	width = m.width;
	
	ptr = static_cast<MatrixRow*>(operator new[](heigth*sizeof(MatrixRow))); 
	for (size_t i = 0;i < m.heigth; i++){
		new (ptr+i)MatrixRow(m.ptr[i]);
	}
	
}


bool Matrix::operator==(const Matrix& m) const 
{
	if (this == &m){
		return true;
	}
	if ((m.heigth != heigth) || (m.width != width))
		return false;

	for (size_t i = 0; i < heigth; i++){
		if (!(ptr[i] == m.ptr[i])){
			return false;
		}
	}

	return true;
}

bool Matrix::operator!=(const Matrix& m) const 
{

	if ((m.heigth != heigth) || (m.width != width))
		return true;

	for (size_t i = 0;i < heigth; i++){
		if (!(ptr[i] == m.ptr[i])){
			return true;
		}
	}

	return false;
}

Matrix& Matrix::operator*=(const int val)
{
	for (size_t i = 0;i < heigth; i++){
		ptr[i] *= val;
	}
	return (*this);
}

MatrixRow& Matrix::operator[](const size_t row)
{
	if (row >= heigth)
		throw std::out_of_range("Out of matrix rows!");

	return ptr[row];
}

const MatrixRow& Matrix::operator[](const size_t row) const
{
	if (row >= heigth)
		throw std::out_of_range("Out of matrix rows!");

	return ptr[row];
}

void Matrix::print() const
{
	for (size_t i = 0;i < heigth; i++){
		for(size_t j = 0; j < width; j++){
			std::cout<<ptr[i][j]<<' ';
		}
		std::cout<<std::endl;
	}
}

Matrix::~Matrix()
{
	for (size_t i=0;i<heigth;i++){
		ptr[i].~MatrixRow();
	}
	operator delete[](ptr);
}
