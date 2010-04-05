

#ifndef GRID_OBJECT_H
#define GRID_OBJECT_H

template<typename _Type > class GridReference;

template<typename _GridType >
class MANGOS_DLL_SPEC GridObjBase
{
    template<typename _Type > friend class Type;
    _GridType * m_grid;

protected:
    GridObjBase( ) : m_grid(0), m_isActiveObject(false), container_type(false) {}
    bool m_isActiveObject;
    bool container_type;    // grid container type - false, world type container - true

public:
    _GridType* GetGrid() const { return m_grid; }
    bool isActiveObject() const { return m_isActiveObject; }
    /*void SetActive()
    {
        if(m_isActiveObject == true)
            return;
        m_isActiveObject = true;
        if(m_grid != 0)
            m_grid->IncrActiveCount();
    }

    void SetPassive()
    {
        if(m_isActiveObject == false)
            return;
        m_isActiveObject = false;
        if(m_grid != 0)
            m_grid->DecrActiveCount();
    }*/

    template<typename _Type >
    class MANGOS_DLL_SPEC Type
    {
    public:

        GridReference<_Type>& GetGridRef() { return m_ref; }
        void RemoveFromGrid() { SetGrid(NULL);}
        void SetGrid(_GridType *type)
        {
            if(m_base.container_type)
            {
                if (m_grid_link != 0)
                    m_grid_link->RemoveWorldObject(&m_base);
                m_grid_link = type;
                if (m_grid_link != 0)
                    m_grid_link->AddWorldObject(&m_base);
            }
            else
            {
                if (m_grid_link != 0)
                    m_grid_link->RemoveGridObject(&m_base);
                m_grid_link = type;
                if (m_grid_link != 0)
                    m_grid_link->AddGridObject(&m_base);
            }
        }

    protected:
        Type(_Type* ty) : m_base(*ty), m_grid_link(ty->m_grid) {} //_Type should inherit from GridObjBase

    private:
        GridReference<_Type> m_ref;
        _Type & m_base;
        _GridType *& m_grid_link;// for fast access to m_base members or nice view at least
    };
};





#endif