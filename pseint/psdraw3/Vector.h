#ifndef VECTOR_H
#define VECTOR_H
#include <cstddef>
#include <cstdlib>

template<typename T>
class Vector {
	T *m_data;
	int m_size, m_capacity;
public:
	Vector() : m_data(nullptr), m_size(0), m_capacity(0) {}
	T &operator[](int i) { return m_data[i]; }
	const T &operator[](int i) const { return m_data[i]; }
	int Count() const { return m_size; }
	void Resize(int n) {
		if (!m_data) {
			m_data = reinterpret_cast<T*>(malloc(n*sizeof(T)));
			m_size = m_capacity = n;
		} else if (n>=m_capacity) {
			m_data = reinterpret_cast<T*>(realloc(m_data,n*sizeof(T)));
			m_size = m_capacity = n;
		} else {
			m_size = n;
		}
	}
	void Resize(int n, T t) {
		Resize(n);
		for(int i=0;i<n;i++) 
			m_data[i]=t;
	}
	void Insert(int i, T x) { 
		Resize(m_size+1);
		for(int j = m_size-1 ; j>i ; --j)
			m_data[j] = m_data[j-1];
		m_data[i] = x;
	}
	void Remove(int i) { 
		for(int j = i ; j<m_size-1; ++j)
			m_data[j] = m_data[j+1];
		Resize(m_size-1);
	}
	~Vector() { if (m_data) free(m_data); }
};

#endif

