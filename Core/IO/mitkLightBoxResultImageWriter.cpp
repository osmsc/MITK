/*=========================================================================

Program:   Medical Imaging & Interaction Toolkit
Module:    $RCSfile$
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) German Cancer Research Center, Division of Medical and
Biological Informatics. All rights reserved.
See MITKCopyright.txt or http://www.mitk.org/copyright.html for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#include "mitkLightBoxResultImageWriter.h"

#include <chili/plugin.h>
#include <chili/qclightbox.h>
#include <chili/isg.h>
#include <ipDicom/ipDicom.h>

#include <mitkImage.h>
#include <mitkImageSliceSelector.h>
#include <mitkFrameOfReferenceUIDManager.h>

#include <itkRootTreeIterator.h>
#include <itkImageFileReader.h>

mitk::LightBoxResultImageWriter::LightBoxResultImageWriter()
{
  m_ImageTypeName = "Image, Segmentation";
}

mitk::LightBoxResultImageWriter::~LightBoxResultImageWriter()
{
}

const mitk::Image *mitk::LightBoxResultImageWriter::GetInput(void)
{
  return static_cast< const mitk::Image * >(this->ProcessObject::GetInput(0));		
}

void mitk::LightBoxResultImageWriter::SetInput(const mitk::Image *image)
{
  this->ProcessObject::SetNthInput( 0, const_cast< mitk::Image * >( image ) );	
}

void mitk::LightBoxResultImageWriter::SetInputByNode(const mitk::DataTreeNode *node)
{
  if(node==NULL)
    return;

  SetInput(dynamic_cast<mitk::Image*>(node->GetData()));
  
  node->GetLevelWindow(m_LevelWindow, NULL, "levelWindow");
  node->GetLevelWindow(m_LevelWindow, NULL);
  const char * name=NULL;
  if(node->GetStringProperty("type name", name, NULL))
    SetImageTypeName(name);
}

const mitk::Image *mitk::LightBoxResultImageWriter::GetSourceImage(void)
{
  return static_cast< const mitk::Image * >(this->ProcessObject::GetInput(1));		
}

void mitk::LightBoxResultImageWriter::SetSourceImage(const mitk::Image *source)
{
  this->ProcessObject::SetNthInput( 1, const_cast< mitk::Image * >( source ) );	
}

bool mitk::LightBoxResultImageWriter::SetSourceByTreeSearch(mitk::DataTreeIteratorBase* iterator)
{
  if(iterator==NULL)
    return false;

  const mitk::Image* image = GetInput();

  if(image == NULL)
    return false;


  mitk::DataTreeIteratorClone it=new itk::RootTreeIterator<DataTreeBase>(iterator->GetTree(), iterator->GetNode());
  bool LoadedFromChili;
  while(!it->IsAtEnd())
  {
//const char * name;
//if(it->Get()->GetStringProperty("name", name, NULL))
//{
//  itkGenericOutputMacro(<<"candidate: "<<name);
//}
//else
//{
//  itkGenericOutputMacro(<<"candidate: no name");
//}
    if(it->Get()->GetBoolProperty("LoadedFromChili", LoadedFromChili) && LoadedFromChili)
    {
      mitk::Image::Pointer sourcecandidate=dynamic_cast<mitk::Image*>(it->Get()->GetData());
      if(sourcecandidate.IsNotNull())
      {
itkGenericOutputMacro(<<"found sourcecandidate: ");
        if(image->GetDimension()<=sourcecandidate->GetDimension())
        {
          int i, dim=image->GetDimension();
          for(i=0; i<dim;++i)
            if(image->GetDimension(i)!=sourcecandidate->GetDimension(i))
              break;
          if(i==dim)
          {
            SetSourceImage(sourcecandidate);
            return true;
itkGenericOutputMacro(<<"dim incorrect: "<<i <<" "<<dim);
          }
        }
      }
    }
    ++it;
  itkGenericOutputMacro(<<"xxxx");
  }
  itkGenericOutputMacro(<<"yyyyyyyyyyyyyyyyyyyyy");
  return false;
}

void mitk::LightBoxResultImageWriter::SetLightBox(QcLightbox* lightbox)
{
  if(lightbox!=m_LightBox)
  {
    m_LightBox=lightbox;
    Modified();    
  }
}

QcLightbox* mitk::LightBoxResultImageWriter::GetLightBox() const
{
  return m_LightBox;
}


void mitk::LightBoxResultImageWriter::GenerateData()
{
  itkGenericOutputMacro(<<"GenerateData ");
  const Image* image = GetInput();
  const Image* sourceimage = GetSourceImage();
  if((image==NULL) || (sourceimage==NULL) || (m_LightBox==NULL))
  {
    itk::ImageFileReaderException e(__FILE__, __LINE__);
    itk::OStringStream msg;
    if(image==NULL)
      msg << "image not set. ";
    if(sourceimage==NULL)
      msg << "source-image not set. ";
    if(m_LightBox==NULL)
      msg << "lightbox not set. ";
    e.SetDescription(msg.str().c_str());
    throw e;
    return;
  }

  ImageSliceSelector::Pointer resultslice = ImageSliceSelector::New();
  ImageSliceSelector::Pointer sourceslice = ImageSliceSelector::New();

  resultslice->SetInput(image);
  sourceslice->SetInput(sourceimage);

  mitk::SlicedGeometry3D* sourceSlicedGeometry;
  mitk::Geometry3D* geometry3DofSlice;
  Point3D origin;
  Vector3D v;

  sourceSlicedGeometry = sourceimage->GetSlicedGeometry();

  origin = sourceSlicedGeometry->GetCornerPoint();

  interSliceGeometry_t isg;
  memcpy(isg.forUID, mitk::FrameOfReferenceUIDManager::GetFrameOfReferenceUID(sourceSlicedGeometry->GetFrameOfReferenceID()), 128);
  isg.psu = ipPicUtilMillimeter;

  ipPicDescriptor *cur, *prev=NULL;
  LevelWindow levelwindow;
  int s, t;
  int smax, tmax;
  smax = image->GetDimension(2);
  tmax = image->GetDimension(3);
itkGenericOutputMacro(<<"writing image: "<<m_ImageTypeName);
itkGenericOutputMacro(<<"lv: "<<m_LevelWindow.GetMin() <<" "<<m_LevelWindow.GetMax());
  for(s=smax-1;s>=0;--s)
  {
    resultslice->SetSliceNr(s);
    sourceslice->SetSliceNr(s);
    for(t=0;t<tmax;++t)
    {
      resultslice->SetTimeNr(t);
      sourceslice->SetTimeNr(t);
      resultslice->Update();
      sourceslice->Update();

      geometry3DofSlice = sourceSlicedGeometry->GetGeometry2D(s);

      v=geometry3DofSlice->GetCornerPoint().GetVectorFromOrigin();
      itk2vtk(v, isg.o);

      v-=origin;
      isg.sl = v.GetNorm();

      v.Set_vnl_vector(geometry3DofSlice->GetMatrixColumn(0));
      isg.ps[0]=v.GetNorm();
      v/=isg.ps[0];
      itk2vtk(v, isg.u);

      v.Set_vnl_vector(geometry3DofSlice->GetMatrixColumn(1));
      isg.ps[1]=v.GetNorm();
      v/=isg.ps[1];
      itk2vtk(v, isg.v);

      v.Set_vnl_vector(geometry3DofSlice->GetMatrixColumn(2));
      isg.ps[2]=v.GetNorm();
      v/=isg.ps[2];
      itk2vtk(v, isg.w);

itkGenericOutputMacro(<<"isg: o("<<isg.o[0]<<" "<<isg.o[1]<<" "<<isg.o[2]<<") u("<<isg.u[0]<<" "<<isg.u[1]<<" "<<isg.u[2]<<") v("<<isg.v[0]<<" "<<isg.v[1]<<" "<<isg.v[2]<<") w("<<isg.w[0]<<" "<<isg.w[1]<<" "<<isg.w[2]<<") "<<t);

itkGenericOutputMacro(<<"writing slice: "<<s <<" "<<t);
      cur=ipPicClone(resultslice->GetOutput()->GetPic());
      ipPicDelTag( cur, "SOURCE HEADER" );

      QcPlugin::addTags( cur, sourceslice->GetOutput()->GetPic(), prev==NULL );

      QcPlugin::addDicomHeader( cur, &isg );
      QcPlugin::addIcon( cur, true );

      QcPlugin::addLevelWindow( cur, m_LevelWindow.GetLevel(), m_LevelWindow.GetWindow());

      QcPlugin::changeImageType( cur, const_cast<char*>(m_ImageTypeName.c_str()) ); 
      if( prev )
        QcPlugin::changeSeries( cur, prev );
      m_LightBox->addImage( cur );

      prev = cur;
    }
  }
}

void mitk::LightBoxResultImageWriter::Write() const
{
  const_cast<mitk::LightBoxResultImageWriter*>(this)->GenerateData();
}
