
#ifndef _CAMERA_H
#define _CAMERA_H

#include "GridDefines.h"


class ViewPoint;
class WorldObject;
class UpdateData;
class WorldPacket;

class MANGOS_DLL_SPEC Camera : public GridObject, public GridCamera
{
    friend class ViewPoint;
    public:

        Camera(Player* pl);
        virtual ~Camera();

        WorldObject* getBody() { return m_source;}
        Player* getOwner() { return &m_owner;}

        // set view to camera's owner
        void SetView(WorldObject *obj);
        void ResetView();

        template<class T>
        void UpdateVisibilityOf(T * obj, UpdateData &d, std::set<WorldObject*>& vis);
        void UpdateVisibilityOf(WorldObject* obj);

        void ReceivePacket(WorldPacket *data);
        void UpdateVisibilityForOwner();

    private:
        // called when viewpoint changes visibility state
        static void Event_AddedToWorld(Camera *);
        static bool Event_RemovedFromWorld(Camera *);
        static void Event_Moved(Camera *);
        static bool Event_ResetView(Camera *);
        static bool Event_ViewPointVisibilityChanged(Camera *);
        static void Call_UpdateVisibilityForOwner(Camera *c) { c->UpdateVisibilityForOwner(); }

        Player & m_owner;
        WorldObject *m_source;
};

class MANGOS_DLL_SPEC ViewPoint
{
    friend class Camera;

    std::vector<Camera*> m_cameras;
    
    void AddCamera(Camera* c) { m_cameras.push_back(c); }
    void RemoveCamera(Camera* c) { m_cameras.erase(remove(m_cameras.begin(),m_cameras.end(), c)); }

    template<typename Handler>
    void CameraCall(Handler h)
    {
        if(m_cameras.size() == 1)   // the most common case
        {
            if( h(m_cameras.front()) == true)
                m_cameras.clear();
        }
        else
            m_cameras.erase( remove_if(m_cameras.begin(),m_cameras.end(),h), m_cameras.end() );
    }

    template<typename Handler>
    void CameraCall(Handler h) const { std::for_each( m_cameras.begin(),m_cameras.end(),h ); }

public:

    virtual ~ViewPoint();

    void CameraEvent_AddedToWorld() const            { if(!m_cameras.empty()) CameraCall(Camera::Event_AddedToWorld); }
    void CameraEvent_RemovedFromWorld()              { if(!m_cameras.empty()) CameraCall(Camera::Event_RemovedFromWorld); }
    void CameraEvent_Moved() const                   { if(!m_cameras.empty()) CameraCall(Camera::Event_Moved); }
    void CameraEvent_ResetView()                     { if(!m_cameras.empty()) CameraCall(Camera::Event_ResetView); }
    void CameraEvent_ViewPointVisibilityChanged()    { if(!m_cameras.empty()) CameraCall(Camera::Event_ViewPointVisibilityChanged); }

    void CameraCall_UpdateVisibilityForOwner() const { if(!m_cameras.empty()) CameraCall(Camera::Call_UpdateVisibilityForOwner); }
};



#endif