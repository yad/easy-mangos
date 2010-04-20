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
    class AreaInfo;

    class MeshTriangle
    {
        public:
            MeshTriangle(){};
            MeshTriangle(uint32 na, uint32 nb, uint32 nc): idx0(na), idx1(nb), idx2(nc) {};

            /* required dummy due to current TreeNode code...make sure to call intersectRay with intersectCallbackIsFast=true */
            //G3D::AABox getAABoxBounds() const { return G3D::AABox(); }

            uint32 idx0;
            uint32 idx1;
            uint32 idx2;
    };

    /*! only for creating KDTrees, required because MeshTriangle cannot its bound by itself */
    /* class SoloTriangle: public MeshTriangle
    {
        public:
            SoloTriangle() {}
            SoloTriangle(uint32 na, uint32 nb, uint32 nc): MeshTriangle(na, nb, nc), vertices(0) {}
            void setVertexData(const G3D::Vector3 *v){ vertices = v; }
            void getBounds (G3D::AABox &out) const
            {
                G3D::Vector3 lo = vertices[idx0];
                G3D::Vector3 hi = lo;

                lo = (lo.min(vertices[idx1])).min(vertices[idx2]);
                hi = (hi.max(vertices[idx1])).max(vertices[idx2]);

                out = G3D::AABox(lo, hi);
            }
            bool operator==(const MeshTriangle &other) const { return idx0==other.idx0 && idx1==other.idx1 && idx2==other.idx2; }
        protected:
            const G3D::Vector3 *vertices;
    }; */

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
            bool IsInsideObject(const Vector3 &pos, float &ground_z) const;
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
            /* WorldModel(G3D::Vector3 *pts, uint32 nPts, MeshTriangle *tri, uint32 nTri, TreeNode *tree, uint32 nNode):
                points(pts), triangles(tri), treeNodes(tree), nPoints(nPts), nTriangles(nTri), nNodes(nNode) {} */
            /* WorldModel(G3D::Vector3 *pts, uint32 nPts, SoloTriangle *tri, uint32 nTri, TreeNode *tree, uint32 nNode):
                points(pts), treeNodes(tree), nPoints(nPts), nTriangles(nTri), nNodes(nNode)
            {
                triangles = new MeshTriangle[nTri];
                for(uint32 i=0; i<nTri; ++i)
                    triangles[i] = MeshTriangle(tri[i].idx0, tri[i].idx1, tri[i].idx2);
            } */
            //! pass group models to WorldModel and create BIH. Passed vector is swapped with old geometry!
            void setGroupModels(std::vector<GroupModel> &models);
            void setRootWmoID(uint32 id) { RootWMOID = id; }
            //bool Intersect(const G3D::Ray &ray, float &distance, bool stopAtFirstHit) const;
            bool IntersectRay(const G3D::Ray &ray, float &distance, bool stopAtFirstHit) const;
            bool IntersectPoint(const G3D::Vector3 &p, AreaInfo &info) const;
            bool writeFile(const std::string &filename);
            bool readFile(const std::string &filename);
        protected:
            /* struct IntersectionCallBack
            {
                IntersectionCallBack(const WorldModel *m): model(m), hit(false) {}
                bool operator()(const G3D::Ray& ray, const MeshTriangle* entity, bool pStopAtFirstHit, float& distance)
                {
                    bool result = model->IntersectTriangle(*entity, ray, distance);
                    if(result)  hit=true;
                    return hit;
                }
                const WorldModel *model;
                bool hit;
            }; */

            //bool IntersectTriangle(const MeshTriangle &t, const G3D::Ray &ray, float &distance) const;
            //G3D::Vector3 *points;
            //MeshTriangle *triangles;
            //TreeNode *treeNodes;
            //uint32 nPoints;
            //uint32 nTriangles;
            //uint32 nNodes;
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
};
template<> struct BoundsTrait<VMAP::SoloTriangle>
{
    static void getBounds(const VMAP::SoloTriangle& obj, G3D::AABox& out) { obj.getBounds(out); }
}; */

#endif // _WORLDMODEL_H
