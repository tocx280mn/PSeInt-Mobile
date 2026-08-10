#ifndef EntityLinkingBase_H
#define EntityLinkingBase_H
#include <cstddef>
#include "Vector.h"


#ifdef _VERIFY_LINKS
#	define _verify_links_ EntityLinkingBase::VerifyAll()
#else
#	define _verify_links_
#endif

// Esta clase encapsula todo lo que tenga que ver con el manejo de enlaces entre las
// entidades... es decir, el mantenimiento del grafo/árbol
// Entity va a heredar (aunque no directamente, ver EntityLinking). Algunos pocas 
// modificaciones que puede generar algún método del grafo y requieren cambios en
// Entity (por ejemplo, al agregar/quitar un hijo, hay que sincronizar los
// auxiliares que tiene el dibujo de cada hijo). Esas cosas se las notifica con 
// el método virtual OnLinkingEvent
struct EntityLinkingBase {
public:
	// todas las entidades del algoritmo/función actual forman una gran lista enlazada usando los atributos all_*,
	// para poder recorrer toodas las entidades sin importar el orden, o sin conocer la primera
	EntityLinkingBase *m_all_next, *m_all_prev;
	static EntityLinkingBase *m_all_any;
protected:
	// para ubicar la entidad dentro del diagrama
	EntityLinkingBase *m_next,*m_prev,*m_parent;
	// entidades hijas/anidadas (para las estructuras de control)
	int m_child_id; // indice de this en el arreglo child del parent (-1 si no es el hijo directo o no tiene parent)
	Vector<EntityLinkingBase*> m_childs; // arreglo de hijos (primer entidad de los sub-bloques, 1 para repetitivas, 2 para si, n para segun)
public:
	EntityLinkingBase();
	virtual ~EntityLinkingBase();
	void UnLink(bool generate_event=true);
	void LinkNext(EntityLinkingBase *e);
	void LinkChild(int id, EntityLinkingBase *e);
	void SetChildCount(int new_count, bool generate_event=true);
	void VerifyLinks(bool recursive);
	void RemoveChild(int id, bool also_delete = true, bool generate_event=true);
	void InsertChild(int id, EntityLinkingBase *e, bool generate_event=true);
	bool Contains(EntityLinkingBase *aux) const;
	static void VerifyAll();
	
	enum LnkEvtType { EVT_UNLINK, EVT_SETCHILDCOUNT, EVT_INSERTCHILD, EVT_REMOVECHILD };
	virtual void OnLinkingEvent(LnkEvtType t, int i) {}
};

// Para evitar casteos a cada rato, esta interfase los hace... para hacerlos sin
// depender de Entity tiene que ser genérica, y lo haga acá separado de EntityLinkingBase
// para que no sea tooodo template... Entonces, el trabajo sucio está en EntityLinkingBase,
// y acá solo la parte genérica de la interfaz.
template<typename Entity> 
struct EntityLinking : public EntityLinkingBase {
	Entity *GetParent() { return reinterpret_cast<Entity*>(m_parent); }
	Entity *GetNext() { return reinterpret_cast<Entity*>(m_next); }
	Entity *GetPrev() { return reinterpret_cast<Entity*>(m_prev); }
	Entity *GetChild(int id) { return reinterpret_cast<Entity*>(m_childs[id]); }
	int GetChildId() { return m_child_id; }
	int GetChildCount() { return m_childs.Count(); }

	// para operar sobre la lista global (la de todo el proceso, atributos all_*)
	class AllIterator {
		friend class EntityLinking;
		EntityLinkingBase *m_current, *m_first;
		AllIterator(EntityLinking *current) 
			: m_current(current), m_first(nullptr) {}
		bool IsNull() { return m_current == nullptr or (m_current == m_first); }
	public:
		AllIterator &operator++() { 
			if (!m_first) m_first = m_current;
			m_current = m_current->m_all_next;
			return *this;
		}
		Entity *GetPtr() { return reinterpret_cast<Entity*>(m_current); }
		void SetPtr(Entity *e) { m_current = e; }
		Entity *operator->() { return reinterpret_cast<Entity*>(m_current); }
		Entity &operator*() { return *reinterpret_cast<Entity*>(m_current); }
		bool operator!=(AllIterator &oit) { return m_current==oit.m_current; }
		bool operator!=(Entity *oit) { return (not (oit==nullptr and IsNull())) and m_current!=oit; }
	};
	static AllIterator AllBegin() { return AllIterator(reinterpret_cast<EntityLinking*>(m_all_any)); }
	static Entity *AllEnd() { return nullptr; }
	
	// hay varias listas globales, una por subproceso... cada una se cierra
	// circularmente sobre sus propias entidades y no hay referencias cruzadas
	// entre dos diferentes... cambiando el all_any se cambia sobre cual se trabaja
	static void AllSet(Entity *any) { m_all_any = any; }
	
	virtual void OnLinkingEvent(LnkEvtType t, int i) {}
	virtual ~EntityLinking() {}
};

#endif

