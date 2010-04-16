

#ifndef GRID_OBJECT_H
#define GRID_OBJECT_H

#include "GridReference.h"

template<typename _GridType >
class MANGOS_DLL_SPEC GridObjBase
{
    template<typename _Type > friend class Type;
    //static _GridType * null_grid = 0;
    _GridType ** _m_grid;

protected:
    GridObjBase( ) : _m_grid(0), m_isActiveObject(false), container_type(false) {}
    bool m_isActiveObject;
    bool container_type;    // grid container type - false, world type container - true

public:
    _GridType* GetGrid() const { return *_m_grid; }
    bool isActiveObject() const { return m_isActiveObject; }

    template<typename _Type >
    class MANGOS_DLL_SPEC Type : protected GridReference<_Type >
    {
    public:

        GridReference<_Type>& GetGridRef() { return *this; }
        void RemoveFromGrid()
        {
            if(m_base.container_type)
            {
                if (m_grid != 0)
                    m_grid->RemoveWorldObject(&m_base);
            }
            else
            {
                if (m_grid != 0)
                    m_grid->RemoveGridObject(&m_base);
            }

            m_grid = 0;
        }
        void SetGrid(_GridType *type)
        {
            if(m_base.container_type)
            {
                if (m_grid != 0)
                    m_grid->RemoveWorldObject(&m_base);
                m_grid = type;
                if (m_grid != 0)
                    m_grid->AddWorldObject(&m_base);
            }
            else
            {
                if (m_grid != 0)
                    m_grid->RemoveGridObject(&m_base);
                m_grid = type;
                if (m_grid != 0)
                    m_grid->AddGridObject(&m_base);
            }
        }

    protected:
        Type(_Type* ty) : m_base(*ty), m_grid(0) { ty->_m_grid = &m_grid; } //_Type should inherit from GridObjBase

    private:
        void targetObjectDestroyLink()
        {
            GridReference<_Type >::targetObjectDestroyLink();
            m_grid = 0;
        }

        _Type & m_base;
        _GridType * m_grid;
    };
};


#endif