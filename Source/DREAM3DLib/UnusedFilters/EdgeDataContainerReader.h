/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
#ifndef _EdgeDataContainerReader_H_
#define _EdgeDataContainerReader_H_

#include <QtCore/QString>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/IDataArray.h"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/IOFilters/util/VertexDataContainerReader.h"

/**
 * @class EdgeDataContainerReader EdgeDataContainerReader.h DREAM3DLib/IOFilters/util/EdgeDataContainerReader.h
 * @brief
 * @author
 * @date
 * @version 1.0
 */
class DREAM3DLib_EXPORT EdgeDataContainerReader : public VertexDataContainerReader
{
  public:
    DREAM3D_SHARED_POINTERS(EdgeDataContainerReader)
    DREAM3D_STATIC_NEW_MACRO(EdgeDataContainerReader)
    DREAM3D_TYPE_MACRO_SUPER(EdgeDataContainerReader, VertexDataContainerReader)

    virtual ~EdgeDataContainerReader();

    /* Place your input parameters here. You can use some of the DREAM3D Macros if you want to */


    typedef QList<QString> NameListType;

    DREAM3D_INSTANCE_PROPERTY(bool, ReadEdgeData)
    DREAM3D_INSTANCE_PROPERTY(bool, ReadEdgeFeatureData)
    DREAM3D_INSTANCE_PROPERTY(bool, ReadEdgeEnsembleData)

    DREAM3D_INSTANCE_PROPERTY(bool, ReadAllEdgeArrays)
    DREAM3D_INSTANCE_PROPERTY(bool, ReadAllEdgeFeatureArrays)
    DREAM3D_INSTANCE_PROPERTY(bool, ReadAllEdgeEnsembleArrays)

    DREAM3D_INSTANCE_PROPERTY(QSet<QString>, EdgeArraysToRead)
    DREAM3D_INSTANCE_PROPERTY(QSet<QString>, EdgeFeatureArraysToRead)
    DREAM3D_INSTANCE_PROPERTY(QSet<QString>, EdgeEnsembleArraysToRead)


    /**
    * @brief
    */
    virtual void execute();

    /**
    * @brief This function runs some sanity checks on the DataContainer and inputs
    * in an attempt to ensure the filter can process the inputs.
    */
    virtual void preflight();

    void setReadAllArrays();

  protected:
    EdgeDataContainerReader();

    void dataCheck();

    int gatherData(bool preflight);

    int readMeshData(hid_t dcGid, bool preflight);

    int readGroupsData(hid_t dcGid, const QString& groupName, bool preflight,
                       QVector<QString>& namesRead,
                       QSet<QString>& namesToRead,
                       bool readAllCurrentArrays);


  private:

    EdgeDataContainerReader(const EdgeDataContainerReader&); // Copy Constructor Not Implemented
    void operator=(const EdgeDataContainerReader&); // Operator '=' Not Implemented
};

#endif /* _EdgeDataContainerReader_H_ */
