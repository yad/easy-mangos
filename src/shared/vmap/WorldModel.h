/*
 * Copyright (C) 2005-2010 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _WORLDMODEL_H
#define _WORLDMODEL_H

#include <G3D/HashTrait.h>
#include <G3D/Vector3.h>
#include <G3D/AABox.h>
#include <G3D/Ray.h>
#include "BIH.h"

#include "Platform/Define.h"

namespace VMAP
{
    class TreeNode;
    struct AreaInfo;

    class MeshTriangle
    {
        public:
            MeshTriangle(){};
            MeshTriangle(uint32 na, uint32 nb, uint32 nc): idx0(na), idx1(nb), idx2(nc) {};

            uint32 idx0;
            uint32 idx1;
            uint32 idx2;
    };

    /*! holding additional info for WMO group files */
    class GroupModel
    {
        public:
            GroupModel() {}
            GroupModel(uint32 mogpFlags, uint32 groupWMOID, const AABox &bound):
                        iBound(bound), iMogpFlags(mogpFlags), iGroupWMOID(groupWMOID) {}

            //! pass mesh data to object and create BIH. Passed vectors get get swapped with old geometry!
            void setMeshData(std::vector<Vector3> &vert, std::vector<MeshTriangle> &tri);
            bool IntersectRay(const G3D::Ray &ray, float &distance, bool stopAtFirstHit) const;
            bool IsInsideObject(const Vector3 &pos, const Vector3 &down, float &z_dist) const;
            bool writeToFile(FILE *wf);
            bool readFromFile(FILE *rf);
            const G3D::AABox& GetBound() const { return iBound; }
            uint32 GetMogpFlags() const { return iMogpFlags; }
            uint32 GetWmoID() const { return iGroupWMOID; }
        protected:
            G3D::AABox iBound;
            uint32 iMogpFlags;// 0x8 outdor; 0x2000 indoor
            uint32 iGroupWMOID;
            std::vector<Vector3> vertices;
            std::vector<MeshTriangle> triangles;
            BIH meshTree;
    };
    /*! Holds a model (converted M2 or WMO) in its original coordinate space */
    class WorldModel
    {
        public:
            WorldModel(): RootWMOID(0) {}

            //! pass group models to WorldModel and create BIH. Passed vector is swapped with old geometry!
            void setGroupModels(std::vector<GroupModel> &models);
            void setRootWmoID(uint32 id) { RootWMOID = id; }
            bool IntersectRay(const G3D::Ray &ray, float &distance, bool stopAtFirstHit) const;
            bool IntersectPoint(const G3D::Vector3 &p, const G3D::Vector3 &down, float &dist, AreaInfo &info) const;
            bool writeFile(const std::string &filename);
            bool readFile(const std::string &filename);
        protected:
            uint32 RootWMOID;
            std::vector<GroupModel> groupModels;
            BIH groupTree;
    };
} // namespace VMAP

/* template<> struct HashTrait<VMAP::SoloTriangle>
{
    static size_t hashCode(const VMAP::SoloTriangle &key)
    {
        return HashTrait<G3D::uint32>::hashCode(key.idx0) ^ HashTrait<G3D::uint32>::hashCode(key.idx1) ^ HashTrait<G3D::uint32>::hashCode(key.idx2);
    }
}; */

#endif // _WORLDMODEL_H
