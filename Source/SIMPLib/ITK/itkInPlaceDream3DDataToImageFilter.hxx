#pragma once

#include "itkInPlaceDream3DDataToImageFilter.h"
#include "SIMPLib/Geometry/ImageGeom.h"

namespace itk
{

template <typename PixelType, unsigned int VDimension>
InPlaceDream3DDataToImageFilter<PixelType, VDimension>::InPlaceDream3DDataToImageFilter() = default;
template <typename PixelType, unsigned int VDimension>
InPlaceDream3DDataToImageFilter<PixelType, VDimension>::~InPlaceDream3DDataToImageFilter() = default;

template <typename PixelType, unsigned int VDimension>
void InPlaceDream3DDataToImageFilter<PixelType, VDimension>::SetInput(DataContainer::Pointer& dc)
{
  if (!(dc == m_DataContainer))
  {
    m_DataContainer = dc;
    this->Modified();
  }
}

template <typename PixelType, unsigned int VDimension>
void InPlaceDream3DDataToImageFilter<PixelType, VDimension>::VerifyPreconditions() ITKv5_CONST
{
  if (m_DataContainer == DataContainer::NullPointer() )
  {
    itkExceptionMacro("DataContainer not set!");
  }
  IGeometry::Pointer geom = m_DataContainer->getGeometry();
  // Verifies that data container geometry is ImageGeom
  QString geomTypeName = geom->getGeometryTypeAsString();
  if (geomTypeName != "ImageGeometry")
  {
    itkExceptionMacro("DataContainer geometry is not ImageGeometry. It is " + geomTypeName.toStdString());
  }
  // Verifies that data container contains matrix array
  if( !m_DataContainer->doesAttributeMatrixExist(m_AttributeMatrixArrayName.c_str()))
  {
    itkExceptionMacro( "Matrix array (" + m_AttributeMatrixArrayName + ") does not exist" );
  }
  AttributeMatrix::Pointer ma = m_DataContainer->getAttributeMatrix(m_AttributeMatrixArrayName.c_str());
  // Verifies that matrix array contains data array
  if (!ma->doesAttributeArrayExist(m_DataArrayName.c_str()))
  {
    itkExceptionMacro("Attribute array (" + m_DataArrayName + ") does not exist");
  }
  Superclass::VerifyPreconditions();
}


template< typename PixelType, unsigned int VDimension>
void InPlaceDream3DDataToImageFilter< PixelType, VDimension >::GenerateOutputInformation()
{
  // call the superclass' implementation of this method
  Superclass::GenerateOutputInformation();

  IGeometry::Pointer geom = m_DataContainer->getGeometry();
  ImageGeom* imageGeom = dynamic_cast<ImageGeom*>(geom.get());
  if (!imageGeom)
  {
    // This should never happen as VerifyPreconditions() verifies that the geometry of
    // the data container is ImageGeom.
    itkExceptionMacro("Error casting geometry!!!");
  }
  // Get data container properties
  FloatVec3Type torigin = imageGeom->getOrigin();
  // Setting spacing to 0 means dimension is not used. Used to know if
  // image is 2D or 3D. Setting dimensions to 0 leads to crashes
  FloatVec3Type tspacing = imageGeom->getSpacing();
  SizeVec3Type tDims = imageGeom->getDimensions();
  typename ImageType::PointType origin;
  typename ImageType::SizeType size;
  typename ImageType::SpacingType spacing;
  typename ImageType::DirectionType direction;
  direction.SetIdentity();
  for (size_t i = 0; i < VDimension; i++)
  {
    spacing[i] = tspacing[i];
    origin[i] = torigin[i];
    size[i] = tDims[i];
  }
  // get pointer to the output
  ImagePointer outputPtr = this->GetOutput();

  // we need to compute the output spacing, the output origin, the
  // output image size, and the output image start index
  outputPtr->SetSpacing( spacing );
  outputPtr->SetOrigin( origin );
  outputPtr->SetDirection( direction );
  outputPtr->SetLargestPossibleRegion( size );
}


template< typename PixelType, unsigned int VDimension>
void InPlaceDream3DDataToImageFilter< PixelType, VDimension >::GenerateData()
{
  // Get data pointer
  AttributeMatrix::Pointer ma = m_DataContainer->getAttributeMatrix(m_AttributeMatrixArrayName.c_str());
  IDataArray::Pointer dataArray = ma->getAttributeArray(m_DataArrayName.c_str());
  size_t size = dataArray->getSize();
  PixelType *buffer;
  if(m_InPlace)
  {
    if (m_PixelContainerWillOwnTheBuffer)
    {
      dataArray->releaseOwnership();
    }
    buffer = static_cast<PixelType*>(dataArray->getVoidPointer( 0 ));
  }
  else
  {
    m_PixelContainerWillOwnTheBuffer = true;
    buffer = new PixelType[size];
    ::memcpy( buffer, static_cast<PixelType*>(dataArray->getVoidPointer( 0 )), size * sizeof( PixelType ) );
  }
  if( !m_ImportImageContainer || buffer != m_ImportImageContainer->GetImportPointer() )
  {
    m_ImportImageContainer = ImportImageContainerType::New();
    m_ImportImageContainer->SetImportPointer( buffer,
        size, m_PixelContainerWillOwnTheBuffer);
  }
  // get pointer to the output
  ImagePointer outputPtr = this->GetOutput();
  outputPtr->SetBufferedRegion( outputPtr->GetLargestPossibleRegion() );
  outputPtr->SetPixelContainer( m_ImportImageContainer );
 }

}// end of itk namespace
