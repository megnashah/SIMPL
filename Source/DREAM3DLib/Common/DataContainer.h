/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  This code was written under United States Air Force Contract number
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#ifndef _GrainGeneratorFunc_H
#define _GrainGeneratorFunc_H

#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#endif



#include <vector>
#include <map>


//#include <boost/shared_array.hpp>

#include "EbsdLib/EbsdConstants.h"

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/AIMArray.hpp"
#include "DREAM3DLib/Common/Field.h"
#include "DREAM3DLib/Common/Observable.h"

/**
 * @class GrainGeneratorFunc GrainGeneratorFunc.h AIM/Common/GrainGeneratorFunc.h
 * @brief
 * @author
 * @date
 * @version 1.0
 */
class DREAM3DLib_EXPORT DataContainer : public Observable
{
  public:
    DREAM3D_SHARED_POINTERS(DataContainer)
    DREAM3D_STATIC_NEW_MACRO(DataContainer)
    DREAM3D_TYPE_MACRO_SUPER(DataContainer, Observable);

    virtual ~DataContainer();

    // Volume Dimensional Information
    float resx;
    float resy;
    float resz;
    int xpoints;
    int ypoints;
    int zpoints;
    int totalpoints;

    // Cell Data
    DECLARE_WRAPPED_ARRAY(grain_indicies, m_GrainIndicies, int)
    DECLARE_WRAPPED_ARRAY(ellipfuncs, m_Ellipfuncs, float)
    DECLARE_WRAPPED_ARRAY(phases, m_Phases, int)
    DECLARE_WRAPPED_ARRAY(euler1s, m_Euler1s, float)
    DECLARE_WRAPPED_ARRAY(euler2s, m_Euler2s, float)
    DECLARE_WRAPPED_ARRAY(euler3s, m_Euler3s, float)
    DECLARE_WRAPPED_ARRAY(surfacevoxels, m_SurfaceVoxels, char)
    DECLARE_WRAPPED_ARRAY(neighbors, m_Neighbors, int);
    DECLARE_WRAPPED_ARRAY(quats, m_Quats, float); // n x 5 array
    DECLARE_WRAPPED_ARRAY(alreadychecked, m_AlreadyChecked, bool);
    DECLARE_WRAPPED_ARRAY(goodVoxels, m_GoodVoxels, bool);
    DECLARE_WRAPPED_ARRAY(nearestgrains, m_NearestGrains, int);
    DECLARE_WRAPPED_ARRAY(nearestneighbors, m_NearestNeighbors, int); // N x 3 Array
    DECLARE_WRAPPED_ARRAY(nearestneighbordistances, m_NearestNeighborDistances, float); // N x 3 Array
    DECLARE_WRAPPED_ARRAY(grainmisorientations, m_GrainMisorientations, float);
    DECLARE_WRAPPED_ARRAY(misorientationgradients, m_MisorientationGradients, float);
    DECLARE_WRAPPED_ARRAY(kernelmisorientations, m_KernelMisorientations, float);

    // Field Data Pointer Array
    std::vector<Field::Pointer> m_Grains;

    // Phase Information (crystal structures, phase types, and shape types)
    std::vector<Ebsd::CrystalStructure> crystruct;
    std::vector<DREAM3D::Reconstruction::PhaseType> phaseType;
    std::vector<float> pptFractions;
    std::vector<DREAM3D::SyntheticBuilder::ShapeType> shapeTypes;
    std::vector<float> phasefraction;

protected:
    DataContainer();
  private:

    DataContainer(const DataContainer&);
    void operator =(const DataContainer&);
};

#endif
