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

#include "ModelInstance.h"
#include "WorldModel.h"

using G3D::Vector3;
using G3D::Ray;

namespace VMAP
{
    ModelInstance::ModelInstance(const ModelSpawn &spawn, WorldModel *model): ModelSpawn(spawn), iModel(model)
    {
        iInvRot = (G3D::Matrix3::fromEulerAnglesZYX(G3D::pi()*iRot.y/180.f, G3D::pi()*iRot.x/180.f, G3D::pi()*iRot.z/180.f)).inverse();
        iInvScale = 1.f/iScale;
    }

    void ModelInstance::intersect(const G3D::Ray& pRay, float& pMaxDist, bool pStopAtFirstHit) const
    {
        if (!iModel) return;
        float time = pRay.intersectionTime(iBound);
        if (time == G3D::inf())
        {
            std::cout << "Ray does not hit '" << name << "'\n";

            return;
        }
        std::cout << "Ray crosses bound of '" << name << "'\n";
        std::cout << "ray from:" << pRay.origin().x << ", " << pRay.origin().y << ", " << pRay.origin().z
                  << " dir:" << pRay.direction().x << ", " << pRay.direction().y << ", " << pRay.direction().z
                  << " t/tmax:" << time << "/" << pMaxDist;
        std::cout << "\nBound lo:" << iBound.low().x << ", " << iBound.low().y << ", " << iBound.low().z << " hi: "
                  << iBound.high().x << ", " << iBound.high().y << ", " << iBound.high().z << std::endl;
        // child bounds are defined in object space:
        Vector3 p = iInvRot * (pRay.origin() - iPos) * iInvScale;
        Ray modRay(p, iInvRot * pRay.direction());
        float distance = pMaxDist * iInvScale;
        iModel->Intersect(modRay, distance, pStopAtFirstHit);
        distance *= iScale;
        pMaxDist = distance;
    }

    // move to new file... (WorldModel.cpp?)
    bool ModelSpawn::readFromFile(FILE *rf, ModelSpawn &spawn)
    {
        uint32 check=0, nameLen;
        check += fread(&spawn.flags, sizeof(uint32), 1, rf);
        check += fread(&spawn.ID, sizeof(uint32), 1, rf);
        check += fread(&spawn.iPos, sizeof(float), 3, rf);
        check += fread(&spawn.iRot, sizeof(float), 3, rf);
        check += fread(&spawn.iScale, sizeof(float), 1, rf);
        bool has_bound = (spawn.flags & MOD_HAS_BOUND);
        if(has_bound) // only WMOs have bound in MPQ, only available after computation
        {
            Vector3 bLow, bHigh;
            check += fread(&bLow, sizeof(float), 3, rf);
            check += fread(&bHigh, sizeof(float), 3, rf);
            spawn.iBound = G3D::AABox(bLow, bHigh);
        }
        check += fread(&nameLen, sizeof(uint32), 1, rf);
        if(check != (has_bound ? 16 : 10)) { printf("Error reading ModelSpawn!\n"); return false; }
        char nameBuff[500];
        if(nameLen>500) { printf("Error, file name too long!\n"); return false; }
        check = fread(nameBuff, sizeof(char), nameLen, rf);
        if(check != nameLen) { printf("Error reading ModelSpawn!\n"); return false; }
        spawn.name = std::string(nameBuff, nameLen);
        return true;
    }

    bool ModelSpawn::writeToFile(FILE *wf, const ModelSpawn &spawn)
    {
        uint32 check=0;
        check += fwrite(&spawn.flags, sizeof(uint32), 1, wf);
        check += fwrite(&spawn.ID, sizeof(uint32), 1, wf);
        check += fwrite(&spawn.iPos, sizeof(float), 3, wf);
        check += fwrite(&spawn.iRot, sizeof(float), 3, wf);
        check += fwrite(&spawn.iScale, sizeof(float), 1, wf);
        bool has_bound = (spawn.flags & MOD_HAS_BOUND);
        if(has_bound) // only WMOs have bound in MPQ, only available after computation
        {
            check += fwrite(&spawn.iBound.low(), sizeof(float), 3, wf);
            check += fwrite(&spawn.iBound.high(), sizeof(float), 3, wf);
        }
        uint32 nameLen = spawn.name.length();
        check += fwrite(&nameLen, sizeof(uint32), 1, wf);
        if(check != (has_bound ? 16 : 10)) return false;
        check = fwrite(spawn.name.c_str(), sizeof(char), nameLen, wf);
        if(check != nameLen) return false;
        return true;
    }
}
