#ifndef PROGRAMA_HPP
#define PROGRAMA_HPP
#include <string>
#include <vector>
#include "Instruccion.hpp"

class Programa {
	std::vector<Instruccion> v;
public:
	Programa() = default;
	Instruccion &operator[](int i) { 
		return v[i];
	}
	const Instruccion &operator[](int i) const { 
		return v[i];
	}
	void Insert(int pos, const Instruccion &inst) {
		v.insert(v.begin()+pos, inst);
	}
	void Insert(int pos,const std::string &inst, CodeLocation loc) {
		Insert(pos,Instruccion(inst,loc));
	}
	void Insert(int pos,const std::string &inst) {
		Insert(pos,inst,{pos?v[pos-1].loc.linea:-1,pos?v[pos-1].loc.instruccion+1:-1});
	}
	void PushBack(const Instruccion &inst) {
		v.push_back(inst);
	}
	void PushBack(std::string inst) { 
		v.push_back(Instruccion(inst,{int(v.size())+1,1}));
	}
	void Erase(int i) { 
		v.erase(v.begin()+i);
	}
	int GetInstCount() const { 
		return v.size();
	}
	void HardReset() { 
		v.clear();
	}
	
	auto begin() { return v.begin(); }
	auto end() { return v.end(); }
	auto begin() const { return v.begin(); }
	auto end() const { return v.end(); }
};

#endif

