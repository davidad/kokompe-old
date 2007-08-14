#pragma once
#include <iostream>

template <typename T>
class array_2d
{
	private:
	public:
		array_2d(unsigned int new_width, unsigned int new_height)
		: width(new_width), height(new_height), size(height * width), data(new T[size])
		{
			
		}
		~array_2d()
		{
			delete data;
		}
		array_2d(const array_2d& base)
		: width(base.width), height(base.height), size(base.size), data(new T[size])
		{
			for(unsigned int i = 0; i < size; i++) data[i] = base.data[i];
		}
		
		
		T& operator[](unsigned int index){return data[index];}
		T& operator()(unsigned int i, unsigned int j){
			if(i + j * width > size)
			{
				std::cout << i << ", " << j << ", " << data[0] << std::endl;
			}
			
			return data[i + j * width];
		}
		
		const T& operator[](unsigned int index) const {return data[index];}
		const T& operator()(unsigned int i, unsigned int j) const {return data[i + j * width];}
		
		array_2d& operator=(const T& v){for(unsigned int i = 0; i < size; i++) data[i] = v; return *this;}
		array_2d& operator=(const array_2d& a){for(unsigned int i = 0; i < size; i++) data[i] = a[i]; return *this;}
		
		const unsigned int width;
		const unsigned int height;
		const unsigned int size;
		T* const data;
};
