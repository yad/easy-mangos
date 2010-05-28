
#ifndef _CAMERA_H
#define _CAMERA_H

#include "GridDefines.h"

class ViewPoint;
class WorldObject;
class UpdateData;
class WorldPacket;

/// Camera - object-receiver. Receives broadcast packets from nearby worldobjects, object visibility changes and sends them to client
class MANGOS_DLL_SPEC Camera
{
    friend class ViewPoint;
    public:

        Camera(Player* pl);
        virtual ~Camera();

        WorldObject* getBody() { return m_source;}
        Player* getOwner() { return &m_owner;}

        void SetView(WorldObject *obj);     // set camera's view to any worldobject.
                                            // Note: this worldobject must be in same map, in same phase with camera's owner(player)
                                            // Note: client supports only unit and dynamic objects as farsight objects
        void ResetView();                   // set view to camera's owner

        template<class T>
        void UpdateVisibilityOf(T * obj, UpdateData &d, std::set<WorldObject*>& vis);
        void UpdateVisibilityOf(WorldObject* obj);

        void ReceivePacket(WorldPacket *data);
        void UpdateVisibilityForOwner();    // updates visibility of worldobjects around viewpoint for camera's owner

    private:
        // called when viewpoint changes visibility state
        void Event_AddedToWorld();
        bool Event_RemovedFromWorld();
        void Event_Moved();
        bool Event_ResetView();
        bool Event_ViewPointVisibilityChanged();

        Player & m_owner;
        WorldObject *m_source;
        void UpdateForCurrentViewPoint();

    public:
        GridReference<Camera>& GetGridRef() { return m_gridRef; }
        bool isActiveObject() const { return false; }
    private:
        GridReference<Camera> m_gridRef;
};

/// Object-observer, notifies farsight object state to cameras that attached to it
class MANGOS_DLL_SPEC ViewPoint
{
    friend class Camera;

    std::vector<Camera*> m_cameras;
    mutable GridType * m_grid;
    
    void Attach(Camera* c) { m_cameras.push_back(c); }
    void Detach(Camera* c) { m_cameras.erase(remove(m_cameras.begin(),m_cameras.end(), c)); }

    void CameraCall(bool (Camera::*handler)(void))
    {
        if(m_cameras.size() == 1)   // the most common case
        {
            if( (m_cameras.front()->*handler)() == true)
                m_cameras.clear();
        }
        else
        {
            for(std::vector<Camera*>::iterator it = m_cameras.begin(); it!=m_cameras.end(); ++it)
                if( ((*it)->*handler)() )
                    (*it) = 0; //mark poiners, that should be removed
            m_cameras.erase(remove(m_cameras.begin(),m_cameras.end(), (Camera*)NULL), m_cameras.end());
        }
    }

    void CameraCall(void (Camera::*const_handler)(void)) const
    {
        for(std::vector<Camera*>::const_iterator it = m_cameras.begin(); it!=m_cameras.end(); ++it)
            ((*it)->*const_handler)();
    }

public:

    ViewPoint() : m_grid(0) {}
    virtual ~ViewPoint();

    // these events are called when viewpoint changes visibility state
    void Event_AddedToMap(GridType *grid) const
    {
        m_grid = grid;
        if(!m_cameras.empty())
            CameraCall(&Camera::Event_AddedToWorld);
    }

    void Event_RemovedFromMap()
    {
        m_grid = NULL;
        if(!m_cameras.empty())
            CameraCall(&Camera::Event_RemovedFromWorld);
    }

    void Event_GridChanged(GridType *grid) const
    {
        m_grid = grid;
        if(!m_cameras.empty())
            CameraCall(&Camera::Event_Moved);
    }

    void Event_ResetView()
    {
        if(!m_cameras.empty())
            CameraCall(&Camera::Event_ResetView);
    }

    void Event_ViewPointVisibilityChanged()
    {
        if(!m_cameras.empty())
            CameraCall(&Camera::Event_ViewPointVisibilityChanged);
    }

    void Call_UpdateVisibilityForOwner() const
    {
        if(!m_cameras.empty())
            CameraCall(&Camera::UpdateVisibilityForOwner);
    }
};



#endif