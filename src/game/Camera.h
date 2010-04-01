
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

        //Camera(Player* pl, WorldObject* v_p);
        Camera(Player* pl);
        virtual ~Camera();

        inline WorldObject* getBody() const { return m_source;}
        inline Player* getOwner() { return m_owner;}

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
        bool Event_AddedToWorld();
        bool Event_RemovedFromWorld();
        bool Event_Moved();
        bool Event_ResetView();
        bool Event_ViewPointVisibilityChanged();

        Player *const m_owner;
        WorldObject *m_source;
};

// only DynamicObject and Unit classes should inherit from this class (all WorldObject classes inherited now)
class MANGOS_DLL_SPEC ViewPoint
{
    friend class Camera;
    typedef std::list<Camera*> CameraList;
    CameraList m_cameras;
    
    void AddCamera(Camera* c) { m_cameras.push_back(c); }
    void RemoveCamera(Camera* c) { m_cameras.remove(c); }

public:

    virtual ~ViewPoint();

    void CameraEvent_AddedToWorld() { CameraCall(&Camera::Event_AddedToWorld); }
    void CameraEvent_RemovedFromWorld() { CameraCall(&Camera::Event_RemovedFromWorld); }
    void CameraEvent_Moved() { CameraCall(&Camera::Event_Moved); }
    void CameraEvent_ResetView() { CameraCall(&Camera::Event_ResetView); }
    void CameraEvent_ViewPointVisibilityChanged() { CameraCall(&Camera::Event_ViewPointVisibilityChanged); }

    void CameraCall(bool (Camera::*m_func)(void));
};



#endif