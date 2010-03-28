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
#include "TreeNode.h"

using G3D::Vector3;
using G3D::Ray;

namespace VMAP
{
    bool readChunk(FILE *rf, char *dest, const char *compare, uint32 len);

    bool WorldModel::IntersectTriangle(const MeshTriangle &tri, const G3D::Ray &ray, float &distance) const
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

    bool WorldModel::Intersect(const G3D::Ray &ray, float &distance, bool stopAtFirstHit) const
    {
        IntersectionCallBack isc(this);
        NodeValueAccess<TreeNode, MeshTriangle> vna(treeNodes, triangles);
        treeNodes[0].intersectRay(ray, isc, distance, vna, stopAtFirstHit, true);
    }

    bool WorldModel::writeFile(const std::string &filename)
    {
        FILE *wf = fopen(filename.c_str(), "wb");
        if (!wf)
            return false;

        bool result = true;
        uint32 chunkSize;
        result = fwrite(VMAP_MAGIC,1,8,wf) == 8;
        if (result && fwrite("WMOD", 1, 4, wf) != 4) result = false;

        // write vertices
        if (result && fwrite("VERT", 1, 4, wf) != 4) result = false;
        chunkSize = sizeof(uint32)+ sizeof(Vector3)*nPoints;
        if (result && fwrite(&chunkSize, sizeof(uint32), 1, wf) != 1) result = false;
        if (result && fwrite(&nPoints, sizeof(uint32), 1, wf) != 1) result = false;
        if (result && fwrite(points, sizeof(Vector3), nPoints, wf) != nPoints) result = false;

        // write triangle mesh
        if (result && fwrite("TRIM", 1, 4, wf) != 4) result = false;
        chunkSize = sizeof(uint32)+ sizeof(MeshTriangle)*nTriangles;
        if (result && fwrite(&chunkSize, sizeof(uint32), 1, wf) != 1) result = false;
        if (result && fwrite(&nTriangles, sizeof(uint32), 1, wf) != 1) result = false;
        if (result && fwrite(triangles, sizeof(MeshTriangle), nTriangles, wf) != nTriangles) result = false;

        // write mesh tree
        if (result && fwrite("NODE", 1, 4, wf) != 4) result = false;
        chunkSize = sizeof(uint32)+ sizeof(TreeNode)*nNodes;
        if (result && fwrite(&chunkSize, sizeof(uint32), 1, wf) != 1) result = false;
        if (result && fwrite(&nNodes, sizeof(uint32), 1, wf) != 1) result = false;
        if (result && fwrite(treeNodes, sizeof(TreeNode), nNodes, wf) != nNodes) result = false;

        fclose(wf);
        return result;
    }

    bool WorldModel::readFile(const std::string &filename)
    {
        FILE *rf = fopen(filename.c_str(), "rb");
        if (!rf)
            return false;

        bool result = true;
        uint32 chunkSize;
        char chunk[8];                          // Ignore the added magic header
        if (!readChunk(rf, chunk, VMAP_MAGIC, 8)) result = false;

        if (result && !readChunk(rf, chunk, "WMOD", 4)) result = false;
        // read vertices
        if (result && !readChunk(rf, chunk, "VERT", 4)) result = false;
        if (result && fread(&chunkSize, sizeof(uint32), 1, rf) != 1) result = false;
        if (result && fread(&nPoints, sizeof(uint32), 1, rf) != 1) result = false;
        if (result) points = new Vector3[nPoints];
        if (result && fread(points, sizeof(Vector3), nPoints, rf) != nPoints) result = false;

        // read triangle mesh
        if (result && !readChunk(rf, chunk, "TRIM", 4)) result = false;
        if (result && fread(&chunkSize, sizeof(uint32), 1, rf) != 1) result = false;
        if (result && fread(&nTriangles, sizeof(uint32), 1, rf) != 1) result = false;
        if (result) triangles = new MeshTriangle[nTriangles];
        if (result && fread(triangles, sizeof(MeshTriangle), nTriangles, rf) != nTriangles) result = false;

        // read mesh tree
        if (result && !readChunk(rf, chunk, "NODE", 4)) result = false;
        if (result && fread(&chunkSize, sizeof(uint32), 1, rf) != 1) result = false;
        if (result && fread(&nNodes, sizeof(uint32), 1, rf) != 1) result = false;
        if (result) treeNodes = new TreeNode[nNodes];
        if (result && fread(treeNodes, sizeof(TreeNode), nNodes, rf) != nNodes) result = false;

        fclose(rf);
        return result;
    }
}
