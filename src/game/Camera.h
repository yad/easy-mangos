
#ifndef _CAMERA_H
#define _CAMERA_H

#include "GridDefines.h"
#include "Object.h"


class ViewPoint;
class Unit;
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
        bool UpdateVisibilityForOwner();

    private:
        // called when viewpoint changes visibility state
        // returns true when need erase camera from viepoint's camera list
        // you can accesss to them only via viewpoint's CameraEvent_* functions
        bool Event_AddedToWorld();
        bool Event_RemovedFromWorld();
        bool Event_Moved();
        bool Event_ResetView();
        bool Event_ViewPointVisibilityChanged();

        Player & m_owner;
        WorldObject *m_source;
};

// only DynamicObject and Unit classes should inherit from this class (all WorldObject classes inherited now)
class MANGOS_DLL_SPEC ViewPoint
{
    friend class Camera;
    typedef std::vector<Camera*> CameraList;
    CameraList m_cameras;
    
    void AddCamera(Camera* c) { m_cameras.push_back(c); }
    void RemoveCamera(Camera* c) { m_cameras.erase(remove(m_cameras.begin(),m_cameras.end(), c), m_cameras.end() ); }

    void CameraCall(bool (Camera::*m_func)(void));

public:

    virtual ~ViewPoint();

    // why 'if (m_cameras.empty() == false)' placed here? not inside CameraCall function?
    // just because CameraEvent_*\CameraCall_* functions will be inlined and check 'if(!m_cameras.empty())' will be inlined too
    void CameraEvent_AddedToWorld() { if(!m_cameras.empty()) CameraCall(&Camera::Event_AddedToWorld); }
    void CameraEvent_RemovedFromWorld() { if(!m_cameras.empty()) CameraCall(&Camera::Event_RemovedFromWorld); }
    void CameraEvent_Moved() { if(!m_cameras.empty()) CameraCall(&Camera::Event_Moved); }
    void CameraEvent_ResetView() { if(!m_cameras.empty()) CameraCall(&Camera::Event_ResetView); }
    void CameraEvent_ViewPointVisibilityChanged() { if(!m_cameras.empty()) CameraCall(&Camera::Event_ViewPointVisibilityChanged); }

    void CameraCall_UpdateVisibilityForOwner() { if(!m_cameras.empty()) CameraCall(&Camera::UpdateVisibilityForOwner); }
};



#endif