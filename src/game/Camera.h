
#ifndef _CAMERA_H
#define _CAMERA_H

#include "GridDefines.h"


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
        void UpdateVisibilityForOwner();

    private:
        // called when viewpoint changes visibility state
        // returns true when need erase camera from viepoint's camera list
        // you can accesss to them only via viewpoint's CameraEvent_* functions
        // all events, which returns (void) are constant for viewpoint
        static void _Event_AddedToWorld(Camera *);
        static bool _Event_RemovedFromWorld(Camera *);
        static void _Event_Moved(Camera *);
        static bool _Event_ResetView(Camera *);
        static bool _Event_ViewPointVisibilityChanged(Camera *);
        static void _UpdateVisibilityForOwner(Camera *c) { c->UpdateVisibilityForOwner(); }

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

    void CameraCall( bool (*m_func)(Camera*) );
    void CameraCall( void (*m_func)(Camera*) ) const;

public:

    virtual ~ViewPoint();

    // why 'if (m_cameras.empty() == false)' placed here? not inside CameraCall function?
    // just because CameraEvent_*\CameraCall_* functions will be inlined and check 'if(!m_cameras.empty())' will be inlined too
    void CameraEvent_AddedToWorld() const { if(!m_cameras.empty()) CameraCall(&Camera::_Event_AddedToWorld); }
    void CameraEvent_RemovedFromWorld() { if(!m_cameras.empty()) CameraCall(&Camera::_Event_RemovedFromWorld); }
    void CameraEvent_Moved() const { if(!m_cameras.empty()) CameraCall(&Camera::_Event_Moved); }
    void CameraEvent_ResetView() { if(!m_cameras.empty()) CameraCall(&Camera::_Event_ResetView); }
    void CameraEvent_ViewPointVisibilityChanged() { if(!m_cameras.empty()) CameraCall(&Camera::_Event_ViewPointVisibilityChanged); }

    void CameraCall_UpdateVisibilityForOwner() const { if(!m_cameras.empty()) CameraCall(&Camera::_UpdateVisibilityForOwner); }
};



#endif