#include <fstream>
#include "ProgramaDump.hpp"
#include "case_map.h"

Programa LoadPrograma(const std::string & fname) {
	Programa p;
	std::ifstream fin(fname);
	if (not fin.is_open()) return p;
	int inst_count;
	fin >> inst_count;
	int type, line_num, inst_num, iaux;
	std::string src;
	for(int i=0;i<inst_count;++i) {
		fin >> type >> line_num >> inst_num;
		fin.ignore(); std::getline(fin,src);
		Instruccion inst(src,{line_num,inst_num});
		inst.setType(static_cast<InstructionType>(type));
		
		switch(inst.type) {
			
		case IT_COMMENT: {
			auto &impl = getImpl<IT_COMMENT>(inst);
			std::getline(fin,impl.text);
		} break;
		
		case IT_PROCESO: {
			auto &impl = getImpl<IT_PROCESO>(inst);
			fin >> iaux; impl.principal = iaux==1;
			fin >> impl.fin; fin.ignore(); 
			std::getline(fin,impl.ret_id);
			std::getline(fin,impl.nombre);
			std::getline(fin,impl.args);
		} break;
		
		case IT_FINPROCESO: {
			auto &impl = getImpl<IT_FINPROCESO>(inst); 
			fin >> iaux; impl.principal = iaux==1;
			fin.ignore(); std::getline(fin,impl.nombre);
		} break;
		
		case IT_LEER: {
			auto &impl = getImpl<IT_LEER>(inst); 
			fin >> iaux; impl.variables.resize(iaux);
			fin.ignore();
			for(auto &var : impl.variables)
				std::getline(fin,var);
		} break;
		
		case IT_ASIGNAR: {
			auto &impl = getImpl<IT_ASIGNAR>(inst); 
			std::getline(fin,impl.variable);
			std::getline(fin,impl.valor);
		} break;
		
		case IT_ESCRIBIR: {
			auto &impl = getImpl<IT_ESCRIBIR>(inst);
			fin >> iaux; impl.saltar = iaux==1;
			fin >> iaux; impl.expresiones.resize(iaux);
			fin.ignore();
			for(auto &expr : impl.expresiones)
				std::getline(fin,expr);
		} break;
			
		case IT_MIENTRAS: {
			auto &impl = getImpl<IT_MIENTRAS>(inst); 
			fin >> impl.fin; 
			fin.ignore();
			std::getline(fin,impl.condicion);
		} break;
		
		case IT_REPETIR: {
			auto &impl = getImpl<IT_REPETIR>(inst);
			fin >> impl.fin;
		} break;
		
		case IT_HASTAQUE: {
			auto &impl = getImpl<IT_HASTAQUE>(inst);
			fin >> iaux; impl.mientras_que = iaux==1;
			fin.ignore(); std::getline(fin,impl.condicion);
		} break;
		
		case IT_SI: {
			auto &impl = getImpl<IT_SI>(inst);
			fin >> impl.sino >> impl.fin;
			fin.ignore();
			std::getline(fin,impl.condicion);
		} break;
			
		case IT_PARA: {
			auto &impl = getImpl<IT_PARA>(inst); 
			fin >> impl.fin;
			fin.ignore();
			std::getline(fin,impl.contador);
			std::getline(fin,impl.val_ini);
			std::getline(fin,impl.paso);
			std::getline(fin,impl.val_fin);
		} break;
		
		case IT_PARACADA: {
			auto &impl = getImpl<IT_PARACADA>(inst); 
			fin >> impl.fin;
			fin.ignore();
			std::getline(fin,impl.identificador);
			std::getline(fin,impl.arreglo);
		} break;
			
		case IT_DIMENSION: {
			auto &impl = getImpl<IT_DIMENSION>(inst);
			fin >> iaux; fin.ignore();
			impl.nombres.resize(iaux);
			impl.tamanios.resize(iaux);
			for(size_t i=0;i<iaux;++i) {
				std::getline(fin,impl.nombres[i]);
				std::getline(fin,impl.tamanios[i]);
			}
		} break;
		
		case IT_DEFINIR: {
			auto &impl = getImpl<IT_DEFINIR>(inst); 
			fin >> iaux; impl.variables.resize(iaux);
			fin.ignore();
			for(auto &var : impl.variables)
				std::getline(fin,var);
			fin >> iaux; impl.tipo.cb_log = iaux==1;
			fin >> iaux; impl.tipo.cb_num = iaux==1;
			fin >> iaux; impl.tipo.cb_car = iaux==1;
			fin >> iaux; impl.tipo.rounded = iaux==1;
		} break;
			
		case IT_SEGUN: {
			auto &impl = getImpl<IT_SEGUN>(inst);
			std::getline(fin,impl.expresion);
			fin >> impl.fin >> iaux; 
			impl.opciones.resize(iaux);
			for(int &op : impl.opciones) fin >> op;
		} break;
		
		case IT_OPCION: {
			auto &impl = getImpl<IT_OPCION>(inst);
			fin >> impl.siguiente >> iaux;
			impl.expresiones.resize(iaux);
			fin.ignore();
			for(auto &expr : impl.expresiones)
				std::getline(fin,expr);
		} break;
		
		case IT_ESPERAR: {
			auto &impl = getImpl<IT_ESPERAR>(inst); 
			fin >> impl.factor; fin.ignore();
			std::getline(fin,impl.tiempo);
		} break;
			
		case IT_INVOCAR: {
			auto &impl = getImpl<IT_INVOCAR>(inst); 
			std::getline(fin,impl.nombre);
			std::getline(fin,impl.args);
		} break;	
			
		case IT_ERROR:
		case IT_ESPERARTECLA:
		case IT_BORRARPANTALLA:
		case IT_FINMIENTRAS:
		case IT_ENTONCES:
		case IT_SINO:
		case IT_FINSI:
		case IT_DEOTROMODO:
		case IT_FINSEGUN:
		case IT_FINPARA:
		case IT_NULL: 
			break;
		}
		
		p.PushBack(inst);
	}
	return p;
}

bool SavePrograma(const std::string &fname, /*const*/ Programa &programa) {
	std::ofstream fout(fname);
	if (not fout.is_open()) return false;
	fout << programa.GetInstCount() << std::endl;
	for(/*const*/ Instruccion &inst : programa) {
		fout << static_cast<int>(inst.type) << ' '
			 << inst.loc.linea << ' '  << inst.loc.instruccion << ' '
			 << inst.instruccion << std::endl;
		switch(inst.type) {
			
		case IT_COMMENT: {
			auto &impl = getImpl<IT_COMMENT>(inst); 
			fout << impl.text << std::endl;
		} break;
		
		case IT_PROCESO: {
			auto &impl = getImpl<IT_PROCESO>(inst); 
			fout << (impl.principal?1:0) << ' ' << impl.fin << std::endl
				 << impl.ret_id << std::endl
				 << impl.nombre << std::endl
				 << impl.args << std::endl;
		} break;
		
		case IT_FINPROCESO: {
			auto &impl = getImpl<IT_FINPROCESO>(inst); 
			fout << (impl.principal?1:0) << ' '  << impl.nombre << std::endl;
		} break;
		
		case IT_LEER: {
			auto &impl = getImpl<IT_LEER>(inst); 
			fout << impl.variables.size() << std::endl;
			for(const auto &var : impl.variables)
				fout << var << std::endl;
		} break;
		
		case IT_ASIGNAR: {
			auto &impl = getImpl<IT_ASIGNAR>(inst); 
			fout << impl.variable << std::endl;
			fout << impl.valor << std::endl;
		} break;
		
		case IT_ESCRIBIR: {
			auto &impl = getImpl<IT_ESCRIBIR>(inst);
			fout << (impl.saltar?1:0) << ' ' << impl.expresiones.size() << std::endl;
			for(const auto &expr : impl.expresiones)
				fout << expr << std::endl;
		} break;
			
		case IT_MIENTRAS: {
			auto &impl = getImpl<IT_MIENTRAS>(inst); 
			fout << impl.fin << ' ' << impl.condicion << std::endl;
		} break;
		
		case IT_REPETIR: {
			auto &impl = getImpl<IT_REPETIR>(inst);
			fout << impl.fin << std::endl;
		} break;
		
		case IT_HASTAQUE: {
			auto &impl = getImpl<IT_HASTAQUE>(inst);
			fout << (impl.mientras_que?1:0) << ' ' << impl.condicion << std::endl;
		} break;
		
		case IT_SI: {
			auto &impl = getImpl<IT_SI>(inst);
			fout << impl.sino << ' ' << impl.fin << ' ' << impl.condicion << std::endl;
		} break;
			
		case IT_PARA: {
			auto &impl = getImpl<IT_PARA>(inst); 
			fout << impl.fin << std::endl
				 << impl.contador << std::endl
			     << impl.val_ini << std::endl
			     << impl.paso << std::endl
			     << impl.val_fin << std::endl;
		} break;
		
		case IT_PARACADA: {
			auto &impl = getImpl<IT_PARACADA>(inst); 
			fout << impl.fin << std::endl
				 << impl.identificador << std::endl
				 << impl.arreglo << std::endl;
		} break;
			
		case IT_DIMENSION: {
			auto &impl = getImpl<IT_DIMENSION>(inst);
			fout << impl.nombres.size() << std::endl;
			for(size_t i=0;i<impl.nombres.size();++i) 
				fout << impl.nombres[i] << std::endl
				     << impl.tamanios[i] << std::endl;
		} break;
		
		case IT_DEFINIR: {
			auto &impl = getImpl<IT_DEFINIR>(inst); 
			fout << impl.variables.size() << std::endl;
			for(auto &var : impl.variables)
				fout << var << std::endl;
			fout << (impl.tipo.cb_log?1:0) << ' '
				 << (impl.tipo.cb_num?1:0) << ' '
				 << (impl.tipo.cb_car?1:0) << ' '
				 << (impl.tipo.rounded?1:0) << std::endl;
		} break;
			
		case IT_SEGUN: {
			auto &impl = getImpl<IT_SEGUN>(inst);
			fout << impl.expresion << std::endl;
			fout << impl.fin << ' ' << impl.opciones.size();
			for(int op : impl.opciones) fout << ' ' << op;
			fout << std::endl;
		} break;
		
		case IT_OPCION: {
			auto &impl = getImpl<IT_OPCION>(inst);
			fout << impl.siguiente << ' ' << impl.expresiones.size() << std::endl;
			for(const auto &expr : impl.expresiones)
				fout << expr << std::endl;
		} break;
		
		case IT_ESPERAR: {
			auto &impl = getImpl<IT_ESPERAR>(inst); 
			fout << impl.factor << ' ' << impl.tiempo << std::endl;
		} break;
			
		case IT_INVOCAR: {
			auto &impl = getImpl<IT_INVOCAR>(inst); 
			fout << impl.nombre << std::endl
				 << impl.args << std::endl;
		} break;	
			
		case IT_ERROR:
		case IT_ESPERARTECLA:
		case IT_BORRARPANTALLA:
		case IT_FINMIENTRAS:
		case IT_ENTONCES:
		case IT_SINO:
		case IT_FINSI:
		case IT_DEOTROMODO:
		case IT_FINSEGUN:
		case IT_FINPARA:
		case IT_NULL: 
			break;
		}
	}
	return true;
}



