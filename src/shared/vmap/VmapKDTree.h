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

#ifndef _VMAPKDTREE_H
#define _VMAPKDTREE_H

#include <G3D/KDTree.h>
#include "TreeNode.h"

namespace G3D
{
/*!
  Just a G3D::KDTree with a few special functions to transform it into a more disk-friendly format
*/

template< class T,
      class BoundsFunc = BoundsTrait<T>,
      class HashFunc   = HashTrait<T>,
      class EqualsFunc = EqualsTrait<T> >
class VmapKDTree : public KDTree <T, BoundsFunc, HashFunc, EqualsFunc>
{
    public:
        void countNodesAndElements(int &nNodes, int &nElements)
        {
            nNodes = 0;
            nElements = 0;
            if(!this->root) return;
            recursiveCountNodesAndElements(*(this->root), nNodes, nElements);
        }

        void serializeTree(Vector3& pLo, Vector3& pHi, VMAP::TreeNode *treeNodes, T *elements)
        {
            int NodePos = 0, ElementPos = 0;
            recursiveSerializeTree(*(this->root), NodePos, ElementPos, pLo, pHi, treeNodes, elements);
        }

    protected:
        void recursiveCountNodesAndElements(const typename KDTree<T, BoundsFunc, HashFunc, EqualsFunc>::Node& pNode, int &pNNodes, int &pNElements)
        {
            ++pNNodes;
            pNElements += pNode.valueArray.size();

            if(pNode.child[0] != 0)
                recursiveCountNodesAndElements(*pNode.child[0], pNNodes, pNElements);

            if(pNode.child[1] != 0)
                recursiveCountNodesAndElements(*pNode.child[1], pNNodes, pNElements);
        }

        void recursiveSerializeTree(const typename KDTree<T, BoundsFunc, HashFunc, EqualsFunc>::Node &pNode, int &NodePos, int &ElementPos, Vector3& pLo, Vector3& pHi,
                   VMAP::TreeNode *treeNodes, T *elements)
        {
            VMAP::TreeNode& treeNode = treeNodes[NodePos++];
            treeNode = VMAP::TreeNode(pNode.valueArray.size(), ElementPos);
            treeNode.setSplitAxis(pNode.splitAxis);
            treeNode.setSplitLocation(pNode.splitLocation);

            Vector3 lo = Vector3(inf(),inf(),inf());
            Vector3 hi = Vector3(-inf(),-inf(),-inf());

            for(int i=0;i<pNode.valueArray.size(); ++i)
            {
                typename KDTree<T, BoundsFunc, HashFunc, EqualsFunc>::Handle *h = pNode.valueArray[i];
                lo = lo.min(h->bounds.low());
                hi = hi.max(h->bounds.high());

                elements[ElementPos++] = h->value;
            }

            if(pNode.child[0] != 0)
            {
                treeNode.setChildPos(0, NodePos);
                recursiveSerializeTree(*pNode.child[0], NodePos, ElementPos, lo, hi, treeNodes, elements);
            }
            if(pNode.child[1] != 0)
            {
                treeNode.setChildPos(1, NodePos);
                recursiveSerializeTree(*pNode.child[1], NodePos, ElementPos, lo, hi, treeNodes, elements);
            }

            treeNode.setBounds(lo,hi);

            // get absolute bounds
            pLo = pLo.min(lo);
            pHi = pHi.max(hi);
        }
};

} // namespace G3D

#endif // _VMAPKDTREE_H
