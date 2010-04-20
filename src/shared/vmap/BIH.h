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

#ifndef _BIH_H
#define _BIH_H

#include <G3D/Vector3.h>
#include <G3D/Ray.h>
#include <G3D/AABox.h>

#include <Platform/Define.h>

#include <stdexcept>
#include <vector>
#include <algorithm>
#include <limits>
#include <cmath>

#define MAX_STACK_SIZE 64

#ifdef _MSC_VER
	#define isnan(x) _isnan(x)
#endif

using G3D::Vector3;
using G3D::AABox;
using G3D::Ray;

static inline uint32 floatToRawIntBits(float f)
{
    union
    {
        uint32 ival;
        float fval;
    } temp;
    temp.fval=f;
    return temp.ival;
}

static inline float intBitsToFloat(uint32 i)
{
    union
    {
        uint32 ival;
        float fval;
    } temp;
    temp.ival=i;
    return temp.fval;
}

struct AABound
{
    Vector3 lo, hi;
};

/** Bounding Interval Hierarchy Class.
    Building and Ray-Intersection functions based on BIH from
    Sunflow, a Java Raytracer, released under MIT/X11 License
    http://sunflow.sourceforge.net/
    Copyright (c) 2003-2007 Christopher Kulla
*/

class BIH
{
    protected:
        std::vector<uint32> tree;
        std::vector<uint32> objects;
        //uint32 *objects;
        //uint32 nObjects;
        AABox bounds;
    public:
        BIH() /*:  objects(0), nObjects(0) */ {};
        template< class T, class BoundsFunc >
        void build(const std::vector<T> &primitives, BoundsFunc &getBounds, uint32 leafSize = 3, bool printStats=false)
        {
            if(primitives.size() == 0)
                return;
            buildData dat;
            dat.maxPrims = leafSize;
            dat.numPrims = primitives.size();
            dat.indices = new uint32[dat.numPrims];
            dat.primBound = new AABox[dat.numPrims];
            getBounds(primitives[0], bounds);
            for (uint32 i=0; i<dat.numPrims; ++i)
            {
                dat.indices[i] = i;
                AABox tb;
                getBounds(primitives[i], dat.primBound[i]);
                bounds.merge(dat.primBound[i]);
            }
            std::vector<uint32> tempTree;
            BuildStats stats;
            buildHierarchy(tempTree, dat, stats);
            if (printStats)
                stats.printStats();

            objects.resize(dat.numPrims);
            for (uint32 i=0; i<dat.numPrims; ++i)
                objects[i] = dat.indices[i];
            //nObjects = dat.numPrims;
            tree = tempTree;
            delete[] dat.primBound;
            delete[] dat.indices;
        }
        uint32 primCount() { return objects.size(); }

    protected:
        struct buildData
        {
            uint32 *indices;
            AABox *primBound;
            uint32 numPrims;
            int maxPrims;
        };
        struct StackNode
        {
            uint32 node;
            float tnear;
            float tfar;
        };

        class BuildStats {
            private:
                int numNodes;
                int numLeaves;
                int sumObjects;
                int minObjects;
                int maxObjects;
                int sumDepth;
                int minDepth;
                int maxDepth;
                int numLeavesN[6];
                int numBVH2;

            public:
            BuildStats():
                numNodes(0), numLeaves(0), sumObjects(0), minObjects(0x0FFFFFFF),
                maxObjects(0xFFFFFFFF), sumDepth(0), minDepth(0x0FFFFFFF),
                maxDepth(0xFFFFFFFF), numBVH2(0)
            {
                for(int i=0; i<6; ++i) numLeavesN[i] = 0;
            }

            void updateInner() { numNodes++; }
            void updateBVH2() { numBVH2++; }
            void updateLeaf(int depth, int n)
            {
                numLeaves++;
                minDepth = std::min(depth, minDepth);
                maxDepth = std::max(depth, maxDepth);
                sumDepth += depth;
                minObjects = std::min(n, minObjects);
                maxObjects = std::max(n, maxObjects);
                sumObjects += n;
                int nl = std::min(n, 5);
                ++numLeavesN[nl];
            }
            void printStats() {
                printf("Tree stats:\n");
                printf("  * Nodes:          %d\n", numNodes);
                printf("  * Leaves:         %d\n", numLeaves);
                printf("  * Objects: min    %d\n", minObjects);
                printf("             avg    %.2f\n", (float) sumObjects / numLeaves);
                printf("           avg(n>0) %.2f\n", (float) sumObjects / (numLeaves - numLeavesN[0]));
                printf("             max    %d\n", maxObjects);
                printf("  * Depth:   min    %d\n", minDepth);
                printf("             avg    %.2f\n", (float) sumDepth / numLeaves);
                printf("             max    %d\n", maxDepth);
                printf("  * Leaves w/: N=0  %3d%%\n", 100 * numLeavesN[0] / numLeaves);
                printf("               N=1  %3d%%\n", 100 * numLeavesN[1] / numLeaves);
                printf("               N=2  %3d%%\n", 100 * numLeavesN[2] / numLeaves);
                printf("               N=3  %3d%%\n", 100 * numLeavesN[3] / numLeaves);
                printf("               N=4  %3d%%\n", 100 * numLeavesN[4] / numLeaves);
                printf("               N>4  %3d%%\n", 100 * numLeavesN[5] / numLeaves);
                printf("  * BVH2 nodes:     %d (%3d%%)\n", numBVH2, 100 * numBVH2 / (numNodes + numLeaves - 2 * numBVH2));
            }
        };

        void buildHierarchy(std::vector<uint32> &tempTree, buildData &dat, BuildStats &stats)
        {
            // create space for the first node
            tempTree.push_back(3 << 30); // dummy leaf
            tempTree.insert(tempTree.end(), 2, 0);
            //tempTree.add(0);

            // seed bbox
            AABound gridBox = { bounds.low(), bounds.high() };
            AABound nodeBox = gridBox;
            // seed subdivide function
            subdivide(0, dat.numPrims - 1, tempTree, dat, gridBox, nodeBox, 0, 1, stats);
        }

        void createNode(std::vector<uint32> &tempTree, int nodeIndex, uint32 left, uint32 right) {
            // write leaf node
            tempTree[nodeIndex + 0] = (3 << 30) | left;
            tempTree[nodeIndex + 1] = right - left + 1;
        }

        void subdivide(int left, int right, std::vector<uint32> &tempTree, buildData &dat, AABound &gridBox, AABound &nodeBox, int nodeIndex, int depth, BuildStats &stats)
        {
            if ((right - left + 1) <= dat.maxPrims || depth >= MAX_STACK_SIZE)
            {
                // write leaf node
                stats.updateLeaf(depth, right - left + 1);
                createNode(tempTree, nodeIndex, left, right);
                return;
            }
            // calculate extents
            int axis = -1, prevAxis, rightOrig;
            float clipL = G3D::fnan(), clipR = G3D::fnan(), prevClip = G3D::fnan();
            float split = G3D::fnan(), prevSplit;
            bool wasLeft = true;
            while (true)
            {
                prevAxis = axis;
                prevSplit = split;
                // perform quick consistency checks
                Vector3 d( gridBox.hi - gridBox.lo );
                if (d.x < 0 || d.y < 0 || d.z < 0)
                    throw std::logic_error("negative node extents");
                for (int i = 0; i < 3; i++)
                {
                    if (nodeBox.hi[i] < gridBox.lo[i] || nodeBox.lo[i] > gridBox.hi[i])
                    {
                        //UI.printError(Module.ACCEL, "Reached tree area in error - discarding node with: %d objects", right - left + 1);
                        throw std::logic_error("invalid node overlap");
                    }
                }
                // find longest axis
                axis = d.primaryAxis();
                split = 0.5f * (gridBox.lo[axis] + gridBox.hi[axis]);
                // partition L/R subsets
                clipL = -G3D::inf();
                clipR = G3D::inf();
                rightOrig = right; // save this for later
                float nodeL = G3D::inf();
                float nodeR = -G3D::inf();
                for (int i = left; i <= right;)
                {
                    int obj = dat.indices[i];
                    float minb = dat.primBound[obj].low()[axis];
                    float maxb = dat.primBound[obj].high()[axis];
                    float center = (minb + maxb) * 0.5f;
                    if (center <= split)
                    {
                        // stay left
                        i++;
                        if (clipL < maxb)
                            clipL = maxb;
                    }
                    else
                    {
                        // move to the right most
                        int t = dat.indices[i];
                        dat.indices[i] = dat.indices[right];
                        dat.indices[right] = t;
                        right--;
                        if (clipR > minb)
                            clipR = minb;
                    }
                    nodeL = std::min(nodeL, minb);
                    nodeR = std::max(nodeR, maxb);
                }
                // check for empty space
                if (nodeL > nodeBox.lo[axis] && nodeR < nodeBox.hi[axis])
                {
                    float nodeBoxW = nodeBox.hi[axis] - nodeBox.lo[axis];
                    float nodeNewW = nodeR - nodeL;
                    // node box is too big compare to space occupied by primitives?
                    if (1.3f * nodeNewW < nodeBoxW)
                    {
                        stats.updateBVH2();
                        int nextIndex = tempTree.size();
                        // allocate child
                        tempTree.push_back(0);
                        tempTree.push_back(0);
                        tempTree.push_back(0);
                        // write bvh2 clip node
                        stats.updateInner();
                        tempTree[nodeIndex + 0] = (axis << 30) | (1 << 29) | nextIndex;
                        tempTree[nodeIndex + 1] = floatToRawIntBits(nodeL);
                        tempTree[nodeIndex + 2] = floatToRawIntBits(nodeR);
                        // update nodebox and recurse
                        nodeBox.lo[axis] = nodeL;
                        nodeBox.hi[axis] = nodeR;
                        subdivide(left, rightOrig, tempTree, dat, gridBox, nodeBox, nextIndex, depth + 1, stats);
                        return;
                    }
                }
                // ensure we are making progress in the subdivision
                if (right == rightOrig)
                {
                    // all left
                    if (prevAxis == axis && prevSplit == split) {
                        // we are stuck here - create a leaf
                        stats.updateLeaf(depth, right - left + 1);
                        createNode(tempTree, nodeIndex, left, right);
                        return;
                    }
                    if (clipL <= split) {
                        // keep looping on left half
                        gridBox.hi[axis] = split;
                        prevClip = clipL;
                        wasLeft = true;
                        continue;
                    }
                    gridBox.hi[axis] = split;
                    prevClip = G3D::fnan();
                }
                else if (left > right)
                {
                    // all right
                    if (prevAxis == axis && prevSplit == split) {
                        // we are stuck here - create a leaf
                        stats.updateLeaf(depth, right - left + 1);
                        createNode(tempTree, nodeIndex, left, right);
                        return;
                    }
                    right = rightOrig;
                    if (clipR >= split) {
                        // keep looping on right half
                        gridBox.lo[axis] = split;
                        prevClip = clipR;
                        wasLeft = false;
                        continue;
                    }
                    gridBox.lo[axis] = split;
                    prevClip = G3D::fnan();
                }
                else
                {
                    // we are actually splitting stuff
                    if (prevAxis != -1 && !isnan(prevClip))
                    {
                        // second time through - lets create the previous split
                        // since it produced empty space
                        int nextIndex = tempTree.size();
                        // allocate child node
                        tempTree.push_back(0);
                        tempTree.push_back(0);
                        tempTree.push_back(0);
                        if (wasLeft) {
                            // create a node with a left child
                            // write leaf node
                            stats.updateInner();
                            tempTree[nodeIndex + 0] = (prevAxis << 30) | nextIndex;
                            tempTree[nodeIndex + 1] = floatToRawIntBits(prevClip);
                            tempTree[nodeIndex + 2] = floatToRawIntBits(G3D::inf());
                        } else {
                            // create a node with a right child
                            // write leaf node
                            stats.updateInner();
                            tempTree[nodeIndex + 0] = (prevAxis << 30) | (nextIndex - 3);
                            tempTree[nodeIndex + 1] = floatToRawIntBits(-G3D::inf());
                            tempTree[nodeIndex + 2] = floatToRawIntBits(prevClip);
                        }
                        // count stats for the unused leaf
                        depth++;
                        stats.updateLeaf(depth, 0);
                        // now we keep going as we are, with a new nodeIndex:
                        nodeIndex = nextIndex;
                    }
                    break;
                }
            }
            // compute index of child nodes
            int nextIndex = tempTree.size();
            // allocate left node
            int nl = right - left + 1;
            int nr = rightOrig - (right + 1) + 1;
            if (nl > 0) {
                tempTree.push_back(0);
                tempTree.push_back(0);
                tempTree.push_back(0);
            } else
                nextIndex -= 3;
            // allocate right node
            if (nr > 0) {
                tempTree.push_back(0);
                tempTree.push_back(0);
                tempTree.push_back(0);
            }
            // write leaf node
            stats.updateInner();
            tempTree[nodeIndex + 0] = (axis << 30) | nextIndex;
            tempTree[nodeIndex + 1] = floatToRawIntBits(clipL);
            tempTree[nodeIndex + 2] = floatToRawIntBits(clipR);
            // prepare L/R child boxes
            AABound gridBoxL(gridBox), gridBoxR(gridBox);
            AABound nodeBoxL(nodeBox), nodeBoxR(nodeBox);
            gridBoxL.hi[axis] = gridBoxR.lo[axis] = split;
            nodeBoxL.hi[axis] = clipL;
            nodeBoxR.lo[axis] = clipR;
            // recurse
            if (nl > 0)
                subdivide(left, right, tempTree, dat, gridBoxL, nodeBoxL, nextIndex, depth + 1, stats);
            else
                stats.updateLeaf(depth + 1, 0);
            if (nr > 0)
                subdivide(right + 1, rightOrig, tempTree, dat, gridBoxR, nodeBoxR, nextIndex + 3, depth + 1, stats);
            else
                stats.updateLeaf(depth + 1, 0);
        }

    public:
        template<typename RayCallback>
        void intersectRay(const Ray &r, RayCallback& intersectCallback, float &maxDist, bool stopAtFirst=false) const
        {
            float intervalMin = 0.f;
            float intervalMax = maxDist;
            Vector3 org = r.origin();
            Vector3 dir = r.direction();
            Vector3 invDir;
            float t1, t2;
            for(int i=0; i<3; ++i)
            {
                invDir[i] = 1.f / dir[i];
                t1 = (bounds.low()[i] - org[i]) * invDir[i];
                t2 = (bounds.high()[i] - org[i]) * invDir[i];
                if (invDir[i] > 0) {
                    if (t1 > intervalMin)
                        intervalMin = t1;
                    if (t2 < intervalMax)
                        intervalMax = t2;
                } else {
                    if (t2 > intervalMin)
                        intervalMin = t2;
                    if (t1 < intervalMax)
                        intervalMax = t1;
                }
                if (intervalMin > intervalMax)
                    return;
            }

            uint32 offsetFront[3];
            uint32 offsetBack[3];
            uint32 offsetFront3[3];
            uint32 offsetBack3[3];
            // compute custom offsets from direction sign bit

            for(int i=0; i<3; ++i)
            {
                offsetFront[i] = floatToRawIntBits(dir[i]) >> 31;
                offsetBack[i] = offsetFront[i] ^ 1;
                offsetFront3[i] = offsetFront[i] * 3;
                offsetBack3[i] = offsetBack[i] * 3;

                // avoid always adding 1 during the inner loop
                ++offsetFront[i];
                ++offsetBack[i];
            }

            StackNode stack[MAX_STACK_SIZE];
            int stackPos = 0;
            int node = 0;

            while (true) {
                while (true)
                {
                    uint32 tn = tree[node];
                    uint32 axis = (tn & (3 << 30)) >> 30;
                    bool BVH2 = tn & (1 << 29);
                    int offset = tn & ~(7 << 29);
                    if (!BVH2)
                    {
                        if (axis < 3)
                        {
                            // "normal" interior node
                            float tf = (intBitsToFloat(tree[node + offsetFront[axis]]) - org[axis]) * invDir[axis];
                            float tb = (intBitsToFloat(tree[node + offsetBack[axis]]) - org[axis]) * invDir[axis];
                            // ray passes between clip zones
                            if (tf < intervalMin && tb > intervalMax)
                                break;
                            int back = offset + offsetBack3[axis];
                            node = back;
                            // ray passes through far node only
                            if (tf < intervalMin) {
                                intervalMin = (tb >= intervalMin) ? tb : intervalMin;
                                continue;
                            }
                            node = offset + offsetFront3[axis]; // front
                            // ray passes through near node only
                            if (tb > intervalMax) {
                                intervalMax = (tf <= intervalMax) ? tf : intervalMax;
                                continue;
                            }
                            // ray passes through both nodes
                            // push back node
                            stack[stackPos].node = back;
                            stack[stackPos].tnear = (tb >= intervalMin) ? tb : intervalMin;
                            stack[stackPos].tfar = intervalMax;
                            stackPos++;
                            // update ray interval for front node
                            intervalMax = (tf <= intervalMax) ? tf : intervalMax;
                            continue;
                        }
                        else
                        {
                            // leaf - test some objects
                            int n = tree[node + 1];
                            while (n > 0) {
                                bool hit = intersectCallback(r, objects[offset], maxDist, stopAtFirst);
                                if(stopAtFirst && hit) return;
                                --n;
                                ++offset;
                            }
                            break;
                        }
                    }
                    else
                    {
                        if (axis>2)
                            return; // should not happen
                        float tf = (intBitsToFloat(tree[node + offsetFront[axis]]) - org[axis]) * invDir[axis];
                        float tb = (intBitsToFloat(tree[node + offsetBack[axis]]) - org[axis]) * invDir[axis];
                        node = offset;
                        intervalMin = (tf >= intervalMin) ? tf : intervalMin;
                        intervalMax = (tb <= intervalMax) ? tb : intervalMax;
                        if (intervalMin > intervalMax)
                            break;
                        continue;
                    }
                } // traversal loop
                do
                {
                    // stack is empty?
                    if (stackPos == 0)
                        return;
                    // move back up the stack
                    stackPos--;
                    intervalMin = stack[stackPos].tnear;
                    if (maxDist < intervalMin)
                        continue;
                    node = stack[stackPos].node;
                    intervalMax = stack[stackPos].tfar;
                    break;
                } while (true);
            }
        }

        template<typename IsectCallback>
        void intersectPoint(const Vector3 &p, IsectCallback& intersectCallback) const
        {
            if (!bounds.contains(p))
                return;

            StackNode stack[MAX_STACK_SIZE];
            int stackPos = 0;
            int node = 0;

            while (true) {
                while (true)
                {
                    uint32 tn = tree[node];
                    uint32 axis = (tn & (3 << 30)) >> 30;
                    bool BVH2 = tn & (1 << 29);
                    int offset = tn & ~(7 << 29);
                    if (!BVH2)
                    {
                        if (axis < 3)
                        {
                            // "normal" interior node
                            float tl = intBitsToFloat(tree[node + 1]);
                            float tr = intBitsToFloat(tree[node + 2]);
                            // point is between clip zones
                            if (tl < p[axis] && tr > p[axis])
                                break;
                            int right = offset + 3;
                            node = right;
                            // point is in right node only
                            if (tl < p[axis]) {
                                continue;
                            }
                            node = offset; // left
                            // point is in left node only
                            if (tr > p[axis]) {
                                continue;
                            }
                            // point is in both nodes
                            // push back right node
                            stack[stackPos].node = right;
                            stackPos++;
                            continue;
                        }
                        else
                        {
                            // leaf - test some objects
                            int n = tree[node + 1];
                            while (n > 0) {
                                intersectCallback(p, objects[offset]); // !!!
                                --n;
                                ++offset;
                            }
                            break;
                        }
                    }
                    else // BVH2 node (empty space cut off left and right)
                    {
                        if (axis>2)
                            return; // should not happen
                        float tl = intBitsToFloat(tree[node + 1]);
                        float tr = intBitsToFloat(tree[node + 2]);
                        node = offset;
                        if (tl > p[axis] || tr < p[axis])
                            break;
                        continue;
                    }
                } // traversal loop

                // stack is empty?
                if (stackPos == 0)
                    return;
                // move back up the stack
                stackPos--;
                node = stack[stackPos].node;
            }
        }

        bool writeToFile(FILE *wf) const
        {
            uint32 treeSize = tree.size();
            uint32 check=0, count=0;
            check += fwrite(&bounds.low(), sizeof(float), 3, wf);
            check += fwrite(&bounds.high(), sizeof(float), 3, wf);
            check += fwrite(&treeSize, sizeof(uint32), 1, wf);
            check += fwrite(&tree[0], sizeof(uint32), treeSize, wf);
            count = objects.size();
            check += fwrite(&count, sizeof(uint32), 1, wf);
            check += fwrite(&objects[0], sizeof(uint32), count, wf);
            return check == (3 + 3 + 2 + treeSize + count);
        }
        bool readFromFile(FILE *rf)
        {
            uint32 treeSize;
            Vector3 lo, hi;
            uint32 check=0, count=0;
            check += fread(&lo, sizeof(float), 3, rf);
            check += fread(&hi, sizeof(float), 3, rf);
            bounds = AABox(lo, hi);
            check += fread(&treeSize, sizeof(uint32), 1, rf);
            tree.resize(treeSize);
            check += fread(&tree[0], sizeof(uint32), treeSize, rf);
            check += fread(&count, sizeof(uint32), 1, rf);
            objects.resize(count); // = new uint32[nObjects];
            check += fread(&objects[0], sizeof(uint32), count, rf);
            return check == (3 + 3 + 2 + treeSize + count);
        }
};

#endif // _BIH_H
