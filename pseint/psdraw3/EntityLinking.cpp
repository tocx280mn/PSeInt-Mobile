#include "EntityLinking.h"
#include "Events.h"

#define Ensure(x) { if (!(x)) asm("int3"); }
#include "Entity.h"
#include <cstdlib>


template<typename T>
static T *t_malloc(int n) {
	return reinterpret_cast<T*>(malloc(n*sizeof(T)));
}

template<typename T>
static T *t_realloc(T *p, int n) {
	return reinterpret_cast<T*>(realloc(p,n*sizeof(T)));
}

EntityLinkingBase *EntityLinkingBase::m_all_any = nullptr;

EntityLinkingBase::EntityLinkingBase()
	: m_parent(nullptr), m_prev(nullptr), m_next(nullptr), m_child_id(-1)
{
	if (!m_all_any) {
		m_all_next = m_all_prev = m_all_any = this; 
	} else {
		m_all_next = m_all_any->m_all_next; 
		if (m_all_next) m_all_next->m_all_prev = this;
		m_all_any->m_all_next = this; 
		m_all_prev = m_all_any;
	}
}

EntityLinkingBase::~EntityLinkingBase() {
	// eliminar los hijos, recursivamente, sino quedan huérfanos
	for (int i=0;i<m_childs.Count();i++) {
		EntityLinkingBase *aux = m_childs[i];
		while (aux) {
			EntityLinkingBase *aux2 = aux->m_next;
			delete aux;
			aux=aux2;
		}
	}
	UnLink();
	// desconectar de la lista global
	if (m_all_prev) m_all_prev->m_all_next = m_all_next;
	if (m_all_next) m_all_next->m_all_prev = m_all_prev;
	if (this==m_all_any) m_all_any = m_all_next; // busca otra...
	if (this==m_all_any) m_all_any = nullptr; // ...si no hay otra esta era la ultima
}

void EntityLinkingBase::UnLink(bool generate_event) {
	SetModified();
	EntityLinkingBase *old_parent = m_parent;
	int old_id = m_child_id;
	if (m_next) m_next->m_prev = m_prev;
	if (m_prev) m_prev->m_next = m_next;
	if (m_parent && m_child_id!=-1) {
		if (m_parent->m_childs[m_child_id]==this) {
			m_parent->m_childs[m_child_id] = m_next;
			if (m_next) m_next->m_child_id = m_child_id;
		}
	}
	m_child_id = -1;
	m_parent = m_next = m_prev = nullptr;
	if (generate_event and old_parent and old_id!=-1) 
		old_parent->OnLinkingEvent(EVT_UNLINK,old_id);
}

void EntityLinkingBase::LinkNext(EntityLinkingBase *e) {
	SetModified();
	EntityLinkingBase *old_next = m_next;
	e->m_prev = this;
	m_next = e;
	e->m_parent = m_parent;
	if (old_next) {
		while (e->m_next) {
			e = e->m_next;
			e->m_parent = m_parent;
		}
		e->m_next = old_next;
		old_next->m_prev = e;
	}
}

void EntityLinkingBase::SetChildCount(int new_count, bool generate_event) {
	m_childs.Resize(new_count,nullptr);
	if (generate_event) OnLinkingEvent(EVT_SETCHILDCOUNT,new_count);
}

#ifdef _VERIFY_LINKS
void EntityLinkingBase::VerifyLinks(bool recursive) {
	Ensure(this!=this->m_parent);
	if (this->m_prev) {
		Ensure( this->m_prev->m_next == this );
		Ensure( this->m_parent == this->m_prev->m_parent );
		Ensure( this->m_child_id == -1 );
	} else {
		if (this->m_child_id!=-1) {
			Ensure( this->m_parent->m_childs.Count() > this->m_child_id );
			Ensure( this->m_parent->m_childs[this->m_child_id] == this );
		}
	}
	if (this->m_next) {
		Ensure( this->m_next->m_prev == this );
		Ensure( this->m_parent == this->m_next->m_parent );
	}
	for(int i=0;i<this->m_childs.Count();i++) {
		if (this->m_childs[i]) {
			Ensure( this->m_childs[i]->m_child_id == i );
			Ensure( this->m_childs[i]->m_parent == this );
		}
	}
	if (recursive) {
		if (this->m_next) m_next->VerifyLinks(true);
		for(int i=0;i<this->m_childs.Count();i++) 
			if (this->m_childs[i]) this->m_childs[i]->VerifyLinks(true);
	}
}

void EntityLinkingBase::VerifyAll( ) {
	EntityLinkingBase *e0 = EntityLinkingBase::m_all_any, 
		              *e1 = EntityLinkingBase::m_all_any;
	do {
		e0->VerifyLinks(false);
		e0 = e0->m_all_next;
	} while(e0!=e1);
}
#endif

void EntityLinkingBase::RemoveChild (int id, bool also_delete, bool generate_event) {
	_verify_links_;
	SetModified();
	if (m_childs[id]) {
		EntityLinkingBase *the_child = m_childs[id];
		the_child->UnLink(false); // no generar EVT_UNLINK, se genera abajo un EVT_REMOVECHILD
		if (also_delete) delete m_childs[id];
	}
	m_childs.Remove(id);
	for (int i=0;i<m_childs.Count();i++)
		if (m_childs[i]) m_childs[i]->m_child_id = i;
	if (generate_event) OnLinkingEvent(EVT_REMOVECHILD,id);
	_verify_links_;
}

void EntityLinkingBase::InsertChild (int id, EntityLinkingBase *e, bool generate_event) {
	_verify_links_;
	SetModified();
	m_childs.Insert(id,nullptr);
	for (int i=0;i<m_childs.Count();i++)
		if (m_childs[i]) m_childs[i]->m_child_id = i;
	if (e) LinkChild(id,e); // agrega al final
	if (generate_event) OnLinkingEvent(EVT_INSERTCHILD,id);
	_verify_links_;
}

void EntityLinkingBase::LinkChild (int id, EntityLinkingBase * e) {
	SetModified();
	if (!m_childs.Count()) SetChildCount(id+1);
	else while (m_childs.Count()<=id) InsertChild(m_childs.Count(),nullptr);
	if (e) {
		e->m_prev = nullptr; 
		if (m_childs[id]) m_childs[id]->m_prev = e;
		EntityLinkingBase *old_child = m_childs[id];
		m_childs[id] = e; 
		e->m_child_id = id;
		e->m_parent = this;	
		while (e->m_next) {
			e = e->m_next; 
			e->m_parent = this;
		}
		e->m_next = old_child; 
		if (old_child) old_child->m_child_id = -1;
	}
}

bool EntityLinkingBase::Contains (EntityLinkingBase * aux) const {
	if (this==aux) return true;
	for (int i=0;i<m_childs.Count();i++)
		if (m_childs[i] && m_childs[i]->Contains(aux)) return true;
	return false;
}

