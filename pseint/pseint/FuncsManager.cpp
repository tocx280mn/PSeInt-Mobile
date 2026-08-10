#include "FuncsManager.hpp"
#include "ErrorHandler.hpp"
#include "utils.h"
#include "intercambio.h"
#include "global.h"
#include "DataValue.h"
using namespace std;

void FuncsManager::UnloadSubprocesos() {
	m_subs.clear();
	m_main_name.clear();
}

void FuncsManager::UnloadPredefs() {
	m_predefs.clear();
}

FuncsManager::~FuncsManager ( ) {
	UnloadPredefs();
	UnloadSubprocesos();
}

void FuncsManager::AddSub(std::unique_ptr<Funcion> &&func) {
	m_subs[func->id] = std::move(func);
}

const Funcion *FuncsManager::GetSub(const std::string &name) const {
	auto it = m_subs.find(name);
	_expects(it!=m_subs.end());
	return it->second.get();
}

const Funcion *FuncsManager::GetFunction(const std::string &name, bool must_exists) const {
	auto it = m_predefs.find(name);
	if (it!=m_predefs.end()) return it->second.get();
	it = m_subs.find(name);
	if(it!=m_subs.end()) return it->second.get();
	_expects(not must_exists);
	return nullptr;
}

Funcion *FuncsManager::GetEditableSub(const std::string &name) {
	auto it = m_subs.find(name);
	_expects(it!=m_subs.end());
	return it->second.get();
}

bool FuncsManager::IsSub(const std::string &name) const {
	return m_subs.count(name)!=0;
}

bool FuncsManager::IsPredef(const std::string &name) const {
	return m_predefs.count(name)!=0;
}

bool FuncsManager::IsFunction(const std::string &name) const {
	return IsPredef(name) or IsSub(name);
}

bool FuncsManager::HaveMain() const { 
	return not m_main_name.empty(); 
}

void FuncsManager::SetMain(const std::string &name) { 
	_expects(m_subs.count(name));
	m_main_name = name;
}

std::string FuncsManager::GetMainName() const {
	return m_main_name; 
}

const Funcion *FuncsManager::GetMainFunc() const {
	return GetSub(m_main_name);
}
