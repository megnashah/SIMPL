/* ============================================================================
 * Copyright (c) 2011, Michael A. Jackson (BlueQuartz Software)
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
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */



#include <string>
#include <iostream>

#include <tclap/CmdLine.h>
#include <tclap/ValueArg.h>

#include <MXA/Common/LogTime.h>
#include <MXA/Utilities/MXALogger.h>
#include <MXA/Utilities/MXADir.h>

#include "DREAM3D/Common/Constants.h"
#include "DREAM3D/DREAM3DVersion.h"
#include "DREAM3D/Common/AIMArray.hpp"

#include "MicrostructureStatistics/MicrostructureStatistics.h"


template<typename T>
int parseValues(const std::string &values, const char* format, T* output)
{
  std::string::size_type pos = values.find(",", 0);
  size_t index = 0;
  int n = sscanf(values.substr(0, pos).c_str(), format, &(output[index]) );
  if (n != 1)
  {
    return -1;
  }

  ++index;
  while(pos != std::string::npos && pos != values.size() - 1)
  {
    n = sscanf(values.substr(pos+1).c_str(), format, &(output[index]) );
    pos = values.find(",", pos+1);
    ++index;
  }
  return 0;
}

template<typename T>
int parseUnknownArray(const std::string &values, const char* format, std::vector<T> &output)
{
  std::string::size_type pos = values.find(",", 0);
  size_t index = 0;
  T t;
  int n = sscanf(values.substr(0, pos).c_str(), format, &t );
  if (n != 1)
  {
    return -1;
  }
  output.push_back(t);

  ++index;
  while(pos != std::string::npos && pos != values.size() - 1)
  {
    n = sscanf(values.substr(pos+1).c_str(), format, &(t) );
    output.push_back(t);
    pos = values.find(",", pos+1);
    ++index;
  }
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int main(int argc, char **argv)
{
  std::cout << logTime() << "Starting Microstructure Statistics... " << std::endl;
  MXALOGGER_METHOD_VARIABLE_INSTANCE



  int err = 0;
  try
  {

    // Handle program options passed on command line.
    TCLAP::CmdLine cmd("DREAM.3D Microstructure Statistics", ' ', DREAM3D::Version::Complete);

    TCLAP::ValueArg<std::string>   voxelFile( "i", "input", "HDF5 Voxel File", true, "", "HDF5 Voxel File");
    cmd.add(voxelFile);
    TCLAP::ValueArg<float>  m_BinStepSize( "", "binstepsize", "Size of each Bin in the Histogram", false, 1.0, "Size of each Bin in the Histogram");
    cmd.add(m_BinStepSize);
    TCLAP::SwitchArg   m_H5StatisticsFile( "", "no-h5stats-file", "Disable Writing of the H5 Statistics file", true);
    cmd.add(m_H5StatisticsFile);
    TCLAP::SwitchArg   m_GrainDataFile( "", "no-grain-file", "Disable Writing of the Grain Data file", true);
    cmd.add(m_GrainDataFile);


    TCLAP::SwitchArg   m_VisualizationVizFile( "", "no-vtk", "Disable Writing of VTK File", true);
    cmd.add(m_VisualizationVizFile);
    TCLAP::SwitchArg   m_WriteSurfaceVoxelScalars( "", "no-surfacevoxels", "Disable Writing of Surface Voxel Scalars", true);
    cmd.add(m_WriteSurfaceVoxelScalars);
    TCLAP::SwitchArg   m_WritePhaseIdScalars( "", "no-phase", "Disable Writing of Phase ID Scalars", true);
    cmd.add(m_WritePhaseIdScalars);
    TCLAP::SwitchArg   m_WriteIPFColorScalars( "", "no-ipf", "Disable Writing of IPF Colors Scalars", true);
    cmd.add(m_WriteIPFColorScalars);
    TCLAP::SwitchArg   m_WriteKernelMisorientationsScalars( "", "no-kernel-miso", "Enable Periodic Boundary Conditions", true);
    cmd.add(m_WriteKernelMisorientationsScalars);
    TCLAP::SwitchArg   m_WriteBinaryVTKFile( "", "vtk_ascii", "Write ASCII Vtk Files instead of Binary Files", true);
    cmd.add(m_WriteBinaryVTKFile);


    TCLAP::ValueArg<std::string > outputDir("", "outputdir", "Output Directory For Files", true, "", "Output Directory");
    cmd.add(outputDir);

    TCLAP::ValueArg<std::string > outputPrefix("", "outputprefix", "Prefix for generated files.", true, "", "Output File Prefix");
    cmd.add(outputPrefix);


    if (argc < 2)
    {
      std::cout << "DREAM.3D Grain Generator Command Line Version " << cmd.getVersion() << std::endl;
      std::vector<std::string> args;
      args.push_back(argv[0]);
      args.push_back("-h");
      cmd.parse(args);
      return -1;
    }

    // Parse the argv array.
    cmd.parse(argc, argv);
    if (argc == 1)
    {
      std::cout << "Grain Generator program was not provided any arguments. Use the --help argument to show the help listing." << std::endl;
      return EXIT_FAILURE;
    }

    MXADir::mkdir(outputDir.getValue(), true);

    MicrostructureStatistics::Pointer m_MicrostructureStatistics = MicrostructureStatistics::New();
    m_MicrostructureStatistics->setInputFile(voxelFile.getValue());
    m_MicrostructureStatistics->setOutputDirectory(outputDir.getValue());
    m_MicrostructureStatistics->setOutputFilePrefix(outputPrefix.getValue());

    bool                        m_ComputeGrainSize = false;
    bool                        m_ComputeGrainShapes = false;
    bool                        m_ComputeNumNeighbors = false;
  //  bool                        m_ComputeAverageOrientations = false;
    bool                        m_ComputeODF = false;
    bool                        m_ComputeMDF = false;

    bool                        m_WriteGrainSize = true;
    bool                        m_WriteGrainShapes = true;
    bool                        m_WriteNumNeighbors = true;
    bool                        m_WriteAverageOrientations = true;

    if(m_H5StatisticsFile.getValue() == true || m_WriteGrainSize == true) m_ComputeGrainSize = true;
    if(m_H5StatisticsFile.getValue() == true || m_WriteGrainShapes == true) m_ComputeGrainShapes = true;
    if(m_H5StatisticsFile.getValue() == true || m_WriteNumNeighbors == true) m_ComputeNumNeighbors = true;
    if(m_H5StatisticsFile.getValue() == true) m_ComputeODF = true;
    if(m_H5StatisticsFile.getValue() == true) m_ComputeMDF = true;

    /********************************
     * Gather any other values from the User Interface and send those to the lower level code
     */
    m_MicrostructureStatistics->setComputeGrainSize(m_ComputeGrainSize);
    m_MicrostructureStatistics->setComputeGrainShapes(m_ComputeGrainShapes);
    m_MicrostructureStatistics->setComputeNumNeighbors(m_ComputeNumNeighbors);
    m_MicrostructureStatistics->setComputeODF(m_ComputeODF);
    m_MicrostructureStatistics->setComputeMDF(m_ComputeMDF);

    m_MicrostructureStatistics->setBinStepSize(m_BinStepSize.getValue());
    m_MicrostructureStatistics->setWriteH5StatsFile(m_H5StatisticsFile.getValue());

    m_MicrostructureStatistics->setWriteGrainFile(m_GrainDataFile.getValue());
    m_MicrostructureStatistics->setWriteGrainSize(m_WriteGrainSize);
    m_MicrostructureStatistics->setWriteGrainShapes(m_WriteGrainShapes);
    m_MicrostructureStatistics->setWriteNumNeighbors(m_WriteNumNeighbors);
    m_MicrostructureStatistics->setWriteAverageOrientations(m_WriteAverageOrientations);

    m_MicrostructureStatistics->setWriteVtkFile(m_VisualizationVizFile.getValue());
    m_MicrostructureStatistics->setWriteSurfaceVoxel(m_WriteSurfaceVoxelScalars.getValue());
    m_MicrostructureStatistics->setWritePhaseId(m_WritePhaseIdScalars.getValue());
    m_MicrostructureStatistics->setWriteKernelMisorientations(m_WriteKernelMisorientationsScalars.getValue());
    m_MicrostructureStatistics->setWriteIPFColor(m_WriteIPFColorScalars.getValue());
    m_MicrostructureStatistics->setWriteBinaryVTKFiles(m_WriteBinaryVTKFile.getValue());

    m_MicrostructureStatistics->printSettings(std::cout);

    m_MicrostructureStatistics->run();
    err = m_MicrostructureStatistics->getErrorCondition();
  }

  catch (TCLAP::ArgException &e) // catch any exceptions
  {
    std::cerr << logTime() << " error: " << e.error() << " for arg " << e.argId() << std::endl;
    return EXIT_FAILURE;
  }
  std::cout << "++++++++++++ Synthetic Builder Complete ++++++++++++" << std::endl;
  return err;
}
