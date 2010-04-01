
#include "Camera.h"
#include "GridNotifiersImpl.h"

//////////////
#define CAMERA_DEBUGGING

#ifdef CAMERA_DEBUGGING
#define CAMERA_OUT(s)  sLog.outError(s)
#define C_ASSERT(s) assert(s)
#endif

Camera::Camera(Player* pl) : GridCamera(this), m_owner(pl), m_source(pl)
{
    m_source->getViewPoint().AddCamera(this);

    container_type = true;
}

Camera::~Camera()
{
    m_source->getViewPoint().RemoveCamera(this);
    CAMERA_OUT("Camera: destuctor");
}

void Camera::ReceivePacket(WorldPacket *data)
{
    m_owner->SendDirectMessage(data);
}

void Camera::SetView(WorldObject *obj)
{
    C_ASSERT(obj && obj != m_source);
    C_ASSERT(m_source->IsInMap(obj));// IsInMap check phases too

    m_source->getViewPoint().RemoveCamera(this);
    m_source = obj;
    m_source->getViewPoint().AddCamera(this);

    SetGrid(m_source->GetGrid());

    m_owner->SetUInt64Value(PLAYER_FARSIGHT, (m_source == m_owner ? 0 : m_source->GetGUID()));//update client view
    UpdateVisibilityForOwner();
}

bool Camera::Event_ResetView()
{
    C_ASSERT(m_owner != m_source);
    //C_ASSERT(m_source->IsInMap(m_owner));   // can be called when m_source not in map?

    m_source = m_owner;
    m_source->getViewPoint().AddCamera(this);

    SetGrid(m_source->GetGrid());

    m_owner->SetUInt64Value(PLAYER_FARSIGHT, NULL);
    UpdateVisibilityForOwner();

    CAMERA_OUT("Camera: Reset view");
    return true;
}

bool Camera::Event_ViewPointVisibilityChanged()
{
    CAMERA_OUT("Camera: Event_ViewPointVisibilityChanged");

    if (m_owner != m_source && !m_owner->HaveAtClient(m_source))
        return Event_ResetView();

    return false;
}

void Camera::ResetView()
{
    C_ASSERT(m_owner != m_source);
    //C_ASSERT(m_source->IsInMap(m_owner));   // can be called when m_source not in map?

    m_source->getViewPoint().RemoveCamera(this);
    m_source = m_owner;
    m_source->getViewPoint().AddCamera(this);

    SetGrid(m_source->GetGrid());

    m_owner->SetUInt64Value(PLAYER_FARSIGHT, NULL);
    UpdateVisibilityForOwner();

    CAMERA_OUT("Camera: Reset view");
}

bool Camera::Event_AddedToWorld()
{
    SetGrid(m_source->GetGrid());

    m_owner->SetUInt64Value(PLAYER_FARSIGHT, (m_source == m_owner ? 0 : m_source->GetGUID()));
    UpdateVisibilityForOwner();

    CAMERA_OUT("Camera: Added to world");
    return false;
}

bool Camera::Event_RemovedFromWorld()
{
    bool erase = false;
    if(m_source == m_owner)
    {
        SetGrid(NULL);
    }
    else
    {   
        // set self view
        erase = Event_ResetView();
    }

    CAMERA_OUT("Camera: removed from world ");
    return erase;
}

bool Camera::Event_Moved()
{
    SetGrid(m_source->GetGrid());

    CAMERA_OUT("Camera: moved to another grid");
    return false;
}

void Camera::UpdateVisibilityOf(WorldObject* target)
{
    m_owner->UpdateVisibilityOf(m_source, target);
}

template<class T>
void Camera::UpdateVisibilityOf(T * target, UpdateData &data, std::set<WorldObject*>& vis)
{
    m_owner->template UpdateVisibilityOf<T>(m_source, target,data,vis);
}

template void Camera::UpdateVisibilityOf(Player*        , UpdateData& , std::set<WorldObject*>& );
template void Camera::UpdateVisibilityOf(Creature*      , UpdateData& , std::set<WorldObject*>& );
template void Camera::UpdateVisibilityOf(Corpse*        , UpdateData& , std::set<WorldObject*>& );
template void Camera::UpdateVisibilityOf(GameObject*    , UpdateData& , std::set<WorldObject*>& );
template void Camera::UpdateVisibilityOf(DynamicObject* , UpdateData& , std::set<WorldObject*>& );

bool Camera::UpdateVisibilityForOwner()
{
    MaNGOS::VisibleNotifier notifier(*this);
    m_source->VisitAll(notifier, m_source->GetMap()->GetVisibilityDistance(), false);
    notifier.Notify();
    return false;
}

//////////////////

ViewPoint::~ViewPoint()
{
    if(!m_cameras.empty())
    {
        CAMERA_OUT("ViewPoint destroyer called, but list of cameras is not empty");
        C_ASSERT(false);
    }
}

void ViewPoint::CameraCall(bool (Camera::*m_func)(void))
{
    if (m_cameras.empty())
        return;

    struct caller 
    {
        bool (Camera::*m_func)(void);
        caller(bool (Camera::*m_f)(void)) : m_func(m_f) {}
        //returns true if need remove camera from viewpoint's camera list
        bool operator () (Camera* c) const { return (c->*m_func)(); }
    };
    m_cameras.remove_if( caller(m_func) );
}