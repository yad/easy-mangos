
#include "Camera.h"
#include "GridNotifiersImpl.h"
#include "CellImpl.h"
#include "Log.h"

//////////////
#define CAMERA_DEBUGGING

#ifdef CAMERA_DEBUGGING
    #define CAMERA_OUT(s)       sLog.outBasic(s)
    #define V_ASSERT(s)         assert(s)
    #define R_ASSERT(s,ret)     assert(s)
#else
    #define CAMERA_OUT(s)
    #define V_ASSERT(s)         if(!(s)) return
    #define R_ASSERT(s,ret)     if(!(s)) return ret
#endif


Camera::Camera(Player* pl) : GridCamera(this), m_owner(*pl), m_source(pl)
{
    m_source->getViewPoint().AddCamera(this);

    container_type = true;
}

Camera::~Camera()
{
    CAMERA_OUT("Camera: destuctor");
    m_source->getViewPoint().RemoveCamera(this);
}

void Camera::ReceivePacket(WorldPacket *data)
{
    m_owner.SendDirectMessage(data);
}

inline void UpdateForCurrentViewPoint(Camera& c, Player & m_owner, WorldObject & m_source)
{
    c.SetGrid(m_source.GetGrid());

    m_owner.SetUInt64Value(PLAYER_FARSIGHT, (&m_source == &m_owner ? 0 : m_source.GetGUID()));
    c.UpdateVisibilityForOwner();
}

void Camera::SetView(WorldObject *obj)
{
    V_ASSERT(obj);
    V_ASSERT(obj->isType(TYPEMASK_DYNAMICOBJECT|TYPEMASK_UNIT) && "Camera::SetView, viewpoint type is not available for client");
    V_ASSERT(m_source->IsInMap(obj));// IsInMap check phases too

    m_source->getViewPoint().RemoveCamera(this);
    m_source = obj;
    m_source->getViewPoint().AddCamera(this);

    UpdateForCurrentViewPoint(*this, m_owner, *m_source);
}

bool Camera::Event_ResetView(Camera * c)
{
    CAMERA_OUT("Camera: Reset view");
    R_ASSERT(&c->m_owner != c->m_source, false);
    //R_ASSERT(m_source->IsInMap(m_owner), false);   // can be called when m_source not in map?

    c->m_source = &c->m_owner;
    c->m_source->getViewPoint().AddCamera(c);

    UpdateForCurrentViewPoint(*c, c->m_owner, *c->m_source);
    return true;
}

bool Camera::Event_ViewPointVisibilityChanged(Camera * c)
{
    CAMERA_OUT("Camera: Event_ViewPointVisibilityChanged");

    if (!c->m_owner.HaveAtClient(c->m_source))
        return Event_ResetView(c);

    return false;
}

void Camera::ResetView()
{
    CAMERA_OUT("Camera: Reset view");

    //V_ASSERT(&m_owner != m_source);   seems this can be called many times.. and bring assert
    //V_ASSERT(m_source->IsInMap(m_owner));   // can be called when m_source not in map?

    m_source->getViewPoint().RemoveCamera(this);
    m_source = &m_owner;
    m_source->getViewPoint().AddCamera(this);

    UpdateForCurrentViewPoint(*this, m_owner, *m_source);
}

void Camera::Event_AddedToWorld(Camera * c)
{
    CAMERA_OUT("Camera: Added to world");
    UpdateForCurrentViewPoint(*c, c->m_owner, *c->m_source);
}

bool Camera::Event_RemovedFromWorld(Camera * c)
{
    CAMERA_OUT("Camera: removed from world ");

    bool erase = false;
    if(c->m_source == &c->m_owner)
    {
        c->SetGrid(NULL);
    }
    else
    {   
        // set self view
        erase = Event_ResetView(c);
    }
    return erase;
}

void Camera::Event_Moved(Camera * c)
{
    CAMERA_OUT("Camera: moved to another grid");
    c->SetGrid(c->m_source->GetGrid());
}

void Camera::UpdateVisibilityOf(WorldObject* target)
{
    m_owner.UpdateVisibilityOf(m_source, target);
}

template<class T>
void Camera::UpdateVisibilityOf(T * target, UpdateData &data, std::set<WorldObject*>& vis)
{
    m_owner.template UpdateVisibilityOf<T>(m_source, target,data,vis);
}

template void Camera::UpdateVisibilityOf(Player*        , UpdateData& , std::set<WorldObject*>& );
template void Camera::UpdateVisibilityOf(Creature*      , UpdateData& , std::set<WorldObject*>& );
template void Camera::UpdateVisibilityOf(Corpse*        , UpdateData& , std::set<WorldObject*>& );
template void Camera::UpdateVisibilityOf(GameObject*    , UpdateData& , std::set<WorldObject*>& );
template void Camera::UpdateVisibilityOf(DynamicObject* , UpdateData& , std::set<WorldObject*>& );

void Camera::UpdateVisibilityForOwner()
{
    MaNGOS::VisibleNotifier notifier(*this);
    Cell::VisitAllObjects(m_source, notifier, m_source->GetMap()->GetVisibilityDistance(), false);
    notifier.Notify();
}

//////////////////

ViewPoint::~ViewPoint()
{
    if(!m_cameras.empty())
    {
        CAMERA_OUT("ViewPoint deconstructor called, but list of cameras is not empty");
        V_ASSERT(false);
    }
}

