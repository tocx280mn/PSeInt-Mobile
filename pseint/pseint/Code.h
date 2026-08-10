#ifndef CODE_H
#define CODE_H

struct CodeLocation {
	int linea, instruccion;
	CodeLocation():linea(-1),instruccion(-1) {}
	CodeLocation(int _linea, int _inst):linea(_linea),instruccion(_inst) {}
	bool IsOk() const { return linea!=-1 and instruccion!=-1; }
};

#endif
