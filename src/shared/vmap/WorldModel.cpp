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

#include "WorldModel.h"
#include "VMapDefinitions.h"
#include "MapTree.h"

using G3D::Vector3;
using G3D::Ray;

template<> struct BoundsTrait<VMAP::GroupModel>
{
    static void getBounds(const VMAP::GroupModel& obj, G3D::AABox& out) { out = obj.GetBound(); }
};


namespace VMAP
{
    bool IntersectTriangle(const MeshTriangle &tri, std::vector<Vector3>::const_iterator points, const G3D::Ray &ray, float &distance)
    {
        static const float EPS = 1e-5f;

        // See RTR2 ch. 13.7 for the algorithm.

        const Vector3 e1 = points[tri.idx1] - points[tri.idx0];
        const Vector3 e2 = points[tri.idx2] - points[tri.idx0];
        const Vector3 p(ray.direction().cross(e2));
        const float a = e1.dot(p);

        if (abs(a) < EPS) {
            // Determinant is ill-conditioned; abort early
            return false;
        }

        const float f = 1.0f / a;
        const Vector3 s(ray.origin() - points[tri.idx0]);
        const float u = f * s.dot(p);

        if ((u < 0.0f) || (u > 1.0f)) {
            // We hit the plane of the m_geometry, but outside the m_geometry
            return false;
        }

        const Vector3 q(s.cross(e1));
        const float v = f * ray.direction().dot(q);

        if ((v < 0.0f) || ((u + v) > 1.0f)) {
            // We hit the plane of the triangle, but outside the triangle
            return false;
        }

        const float t = f * e2.dot(q);

        if ((t > 0.0f) && (t < distance))
        {
            // This is a new hit, closer than the previous one
            distance = t;

            /* baryCoord[0] = 1.0 - u - v;
            baryCoord[1] = u;
            baryCoord[2] = v; */

            return true;
        }
        // This hit is after the previous hit, so ignore it
        return false;
    }

    // ===================== GroupModel ==================================

    class TriBoundFunc
    {
        public:
            TriBoundFunc(std::vector<Vector3> &vert): vertices(vert.begin()) {}
            void operator()(const MeshTriangle &tri, G3D::AABox &out) const
            {
                G3D::Vector3 lo = vertices[tri.idx0];
                G3D::Vector3 hi = lo;

                lo = (lo.min(vertices[tri.idx1])).min(vertices[tri.idx2]);
                hi = (hi.max(vertices[tri.idx1])).max(vertices[tri.idx2]);

                out = G3D::AABox(lo, hi);
            }
        protected:
            const std::vector<Vector3>::const_iterator vertices;
    };

    void GroupModel::setMeshData(std::vector<Vector3> &vert, std::vector<MeshTriangle> &tri)
    {
        vertices.swap(vert);
        triangles.swap(tri);
        TriBoundFunc bFunc(vertices);
        meshTree.build(triangles, bFunc);
    }

    bool GroupModel::writeToFile(FILE *wf)
    {
        bool result = true;
        uint32 chunkSize, count;

        if (result && fwrite(&iBound, sizeof(G3D::AABox), 1, wf) != 1) result = false;
        if (result && fwrite(&iMogpFlags, sizeof(uint32), 1, wf) != 1) result = false;
        if (result && fwrite(&iGroupWMOID, sizeof(uint32), 1, wf) != 1) result = false;

        // write vertices
        if (result && fwrite("VERT", 1, 4, wf) != 4) result = false;
        count = vertices.size();
        chunkSize = sizeof(uint32)+ sizeof(Vector3)*count;
        if (result && fwrite(&chunkSize, sizeof(uint32), 1, wf) != 1) result = false;
        if (result && fwrite(&count, sizeof(uint32), 1, wf) != 1) result = false;
        if (!count) // models without (collision) geometry end here, unsure if they are useful
            return result;
        if (result && fwrite(&vertices[0], sizeof(Vector3), count, wf) != count) result = false;

        // write triangle mesh
        if (result && fwrite("TRIM", 1, 4, wf) != 4) result = false;
        count = triangles.size();
        chunkSize = sizeof(uint32)+ sizeof(MeshTriangle)*count;
        if (result && fwrite(&chunkSize, sizeof(uint32), 1, wf) != 1) result = false;
        if (result && fwrite(&count, sizeof(uint32), 1, wf) != 1) result = false;
        if (result && fwrite(&triangles[0], sizeof(MeshTriangle), count, wf) != count) result = false;

        // write mesh BIH
        if (result && fwrite("MBIH", 1, 4, wf) != 4) result = false;
        if (result) result = meshTree.writeToFile(wf);
        return result;
    }

    bool GroupModel::readFromFile(FILE *rf)
    {
        char chunk[8];
        bool result = true;
        uint32 chunkSize, count;
        triangles.clear();
        vertices.clear();

        if (result && fread(&iBound, sizeof(G3D::AABox), 1, rf) != 1) result = false;
        if (result && fread(&iMogpFlags, sizeof(uint32), 1, rf) != 1) result = false;
        if (result && fread(&iGroupWMOID, sizeof(uint32), 1, rf) != 1) result = false;

        // read vertices
        if (result && !readChunk(rf, chunk, "VERT", 4)) result = false;
        if (result && fread(&chunkSize, sizeof(uint32), 1, rf) != 1) result = false;
        if (result && fread(&count, sizeof(uint32), 1, rf) != 1) result = false;
        if (!count) // models without (collision) geometry end here, unsure if they are useful
            return result;
        if (result) vertices.resize(count);
        if (result && fread(&vertices[0], sizeof(Vector3), count, rf) != count) result = false;

        // read triangle mesh
        if (result && !readChunk(rf, chunk, "TRIM", 4)) result = false;
        if (result && fread(&chunkSize, sizeof(uint32), 1, rf) != 1) result = false;
        if (result && fread(&count, sizeof(uint32), 1, rf) != 1) result = false;
        if (result) triangles.resize(count);
        if (result && fread(&triangles[0], sizeof(MeshTriangle), count, rf) != count) result = false;

        // read mesh BIH
        if (result && !readChunk(rf, chunk, "MBIH", 4)) result = false;
        if (result) result = meshTree.readFromFile(rf);
        return result;
    }

    struct GModelRayCallback
    {
        GModelRayCallback(const std::vector<MeshTriangle> &tris, const std::vector<Vector3> &vert):
            vertices(vert.begin()), triangles(tris.begin()), hit(false) {}
        bool operator()(const G3D::Ray& ray, uint32 entry, float& distance, bool pStopAtFirstHit)
        {
            bool result = IntersectTriangle(triangles[entry], vertices, ray, distance);
            if (result)  hit=true;
            return hit;
        }
        std::vector<Vector3>::const_iterator vertices;
        std::vector<MeshTriangle>::const_iterator triangles;
        bool hit;
    };

    bool GroupModel::IntersectRay(const G3D::Ray &ray, float &distance, bool stopAtFirstHit) const
    {
        if (!triangles.size())
            return false;
        GModelRayCallback callback(triangles, vertices);
        meshTree.intersectRay(ray, callback, distance, stopAtFirstHit);
        return callback.hit;
    }

    bool GroupModel::IsInsideObject(const Vector3 &pos, const Vector3 &down, float &z_dist) const
    {
        if (!triangles.size() || !iBound.contains(pos))
            return false;
        GModelRayCallback callback(triangles, vertices);
        Vector3 rPos = pos - 0.1f * down;
        float dist = G3D::inf();
        G3D::Ray ray(rPos, down);
        bool hit = IntersectRay(ray, dist, false);
        if (hit)
            z_dist = dist - 0.1f;
        return hit;
    }

    // ===================== WorldModel ==================================

    void WorldModel::setGroupModels(std::vector<GroupModel> &models)
    {
        groupModels.swap(models);
        groupTree.build(groupModels, BoundsTrait<GroupModel>::getBounds, 1);
    }

    struct WModelRayCallBack
    {
        WModelRayCallBack(const std::vector<GroupModel> &mod): models(mod.begin()), hit(false) {}
        bool operator()(const G3D::Ray& ray, uint32 entry, float& distance, bool pStopAtFirstHit)
        {
            bool result = models[entry].IntersectRay(ray, distance, pStopAtFirstHit);
            if (result)  hit=true;
            return hit;
        }
        std::vector<GroupModel>::const_iterator models;
        bool hit;
    };

    bool WorldModel::IntersectRay(const G3D::Ray &ray, float &distance, bool stopAtFirstHit) const
    {
        // small M2 workaround, maybe better make separate class with virtual intersection funcs
        // in any case, there's no need to use a bound tree if we only have one submodel
        if (groupModels.size() == 1)
            return groupModels[0].IntersectRay(ray, distance, stopAtFirstHit);

        WModelRayCallBack isc(groupModels);
        groupTree.intersectRay(ray, isc, distance, stopAtFirstHit);
        return isc.hit;
    }

    class WModelAreaCallback {
        public:
            WModelAreaCallback(const std::vector<GroupModel> &vals, const Vector3 &down):
                prims(vals.begin()), hit(vals.end()), minVol(G3D::inf()), zDist(G3D::inf()), zVec(down) {}
            std::vector<GroupModel>::const_iterator prims;
            std::vector<GroupModel>::const_iterator hit;
            float minVol;
            float zDist;
            Vector3 zVec;
            void operator()(const Vector3& point, uint32 entry)
            {
                 float group_Z;
                //float pVol = prims[entry].GetBound().volume();
                //if(pVol < minVol)
                //{
                    /* if (prims[entry].iBound.contains(point)) */
                    if (prims[entry].IsInsideObject(point, zVec, group_Z))
                    {
                        //minVol = pVol;
                        //hit = prims + entry;
                        if (group_Z < zDist)
                        {
                            zDist = group_Z;
                            hit = prims + entry;
                        }
#ifdef VMAP_DEBUG
                        const GroupModel &gm = prims[entry];
                        printf("%10u %8X %7.3f,%7.3f,%7.3f | %7.3f,%7.3f,%7.3f | z=%f, p_z=%f\n", gm.GetWmoID(), gm.GetMogpFlags(),
                        gm.GetBound().low().x, gm.GetBound().low().y, gm.GetBound().low().z,
                        gm.GetBound().high().x, gm.GetBound().high().y, gm.GetBound().high().z, group_Z, point.z);
#endif
                    }
                //}
                //std::cout << "trying to intersect '" << prims[entry].name << "'\n";
            }
    };

    bool WorldModel::IntersectPoint(const G3D::Vector3 &p, const G3D::Vector3 &down, float &dist, AreaInfo &info) const
    {
        if (!groupModels.size())
            return false;
        WModelAreaCallback callback(groupModels, down);
        groupTree.intersectPoint(p, callback);
        if (callback.hit != groupModels.end())
        {
            info.rootId = RootWMOID;
            info.groupId = callback.hit->GetWmoID();
            info.flags = callback.hit->GetMogpFlags();
            info.result = true;
            dist = callback.zDist;
            return true;
        }
        return false;
    }


    bool WorldModel::writeFile(const std::string &filename)
    {
        FILE *wf = fopen(filename.c_str(), "wb");
        if (!wf)
            return false;

        bool result = true;
        uint32 chunkSize, count;
        result = fwrite(VMAP_MAGIC,1,8,wf) == 8;
        if (result && fwrite("WMOD", 1, 4, wf) != 4) result = false;
        chunkSize = sizeof(uint32) + sizeof(uint32);
        if (result && fwrite(&chunkSize, sizeof(uint32), 1, wf) != 1) result = false;
        if (result && fwrite(&RootWMOID, sizeof(uint32), 1, wf) != 1) result = false;

        // write group models
        count=groupModels.size();
        if (count)
        {
            if (result && fwrite("GMOD", 1, 4, wf) != 4) result = false;
            //chunkSize = sizeof(uint32)+ sizeof(GroupModel)*count;
            //if (result && fwrite(&chunkSize, sizeof(uint32), 1, wf) != 1) result = false;
            if (result && fwrite(&count, sizeof(uint32), 1, wf) != 1) result = false;
            for (uint32 i=0; i<groupModels.size() && result; ++i)
                result = groupModels[i].writeToFile(wf);

            // write group BIH
            if (result && fwrite("GBIH", 1, 4, wf) != 4) result = false;
            if (result) result = groupTree.writeToFile(wf);
        }

        fclose(wf);
        return result;
    }

    bool WorldModel::readFile(const std::string &filename)
    {
        FILE *rf = fopen(filename.c_str(), "rb");
        if (!rf)
            return false;

        bool result = true;
        uint32 chunkSize, count;
        char chunk[8];                          // Ignore the added magic header
        if (!readChunk(rf, chunk, VMAP_MAGIC, 8)) result = false;

        if (result && !readChunk(rf, chunk, "WMOD", 4)) result = false;
        if (result && fread(&chunkSize, sizeof(uint32), 1, rf) != 1) result = false;
        if (result && fread(&RootWMOID, sizeof(uint32), 1, rf) != 1) result = false;

        // read group models
        if (result && readChunk(rf, chunk, "GMOD", 4))
        {
            //if (fread(&chunkSize, sizeof(uint32), 1, rf) != 1) result = false;

            if (result && fread(&count, sizeof(uint32), 1, rf) != 1) result = false;
            if (result) groupModels.resize(count);
            //if (result && fread(&groupModels[0], sizeof(GroupModel), count, rf) != count) result = false;
            for (uint32 i=0; i<count && result; ++i)
                result = groupModels[i].readFromFile(rf);

            // read group BIH
            if (result && !readChunk(rf, chunk, "GBIH", 4)) result = false;
            if (result) result = groupTree.readFromFile(rf);
        }

        fclose(rf);
        return result;
    }
}
