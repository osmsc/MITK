#include "QmitkCreatePolygonModelAction.h"

#include "mitkShowSegmentationAsSmoothedSurface.h"
#include "mitkShowSegmentationAsSurface.h"
#include "mitkProgressBar.h"
#include "mitkStatusBar.h"

#include <berryIPreferencesService.h>
#include <berryPlatform.h>

QmitkCreatePolygonModelAction::QmitkCreatePolygonModelAction()
{
}

QmitkCreatePolygonModelAction::~QmitkCreatePolygonModelAction()
{
}

void QmitkCreatePolygonModelAction::Run(const std::vector<mitk::DataNode*>& selectedNodes)
{
 NodeList selection = selectedNodes;

  for ( NodeList::iterator iter = selection.begin(); iter != selection.end(); ++iter )
  {
    mitk::DataNode* node = *iter;

    if (node)
    {
      mitk::Image::Pointer image = dynamic_cast<mitk::Image*>( node->GetData() );
      if (image.IsNull()) return;

      try
      {
        if (!m_IsSmoothed)
        {
          mitk::ShowSegmentationAsSurface::Pointer surfaceFilter = mitk::ShowSegmentationAsSurface::New();

          // attach observer to get notified about result
          itk::SimpleMemberCommand<QmitkCreatePolygonModelAction>::Pointer goodCommand = itk::SimpleMemberCommand<QmitkCreatePolygonModelAction>::New();
          goodCommand->SetCallbackFunction(this, &QmitkCreatePolygonModelAction::OnSurfaceCalculationDone);
          surfaceFilter->AddObserver(mitk::ResultAvailable(), goodCommand);
          itk::SimpleMemberCommand<QmitkCreatePolygonModelAction>::Pointer badCommand = itk::SimpleMemberCommand<QmitkCreatePolygonModelAction>::New();
          badCommand->SetCallbackFunction(this, &QmitkCreatePolygonModelAction::OnSurfaceCalculationDone);
          surfaceFilter->AddObserver(mitk::ProcessingError(), badCommand);

          mitk::DataNode::Pointer nodepointer = node;
          surfaceFilter->SetPointerParameter("Input", image);
          surfaceFilter->SetPointerParameter("Group node", nodepointer);
          surfaceFilter->SetParameter("Show result", true );
          surfaceFilter->SetParameter("Sync visibility", false );
          surfaceFilter->SetDataStorage( *m_DataStorage );

          /*if (this->m_IsSmoothed)
          {
            surfaceFilter->SetParameter("Smooth", true );
            //surfaceFilter->SetParameter("Apply median", true );
            surfaceFilter->SetParameter("Apply median", false );  // median makes the resulting surfaces look like lego models
            surfaceFilter->SetParameter("Median kernel size", 3u );
            surfaceFilter->SetParameter("Gaussian SD", 2.5f );
            surfaceFilter->SetParameter("Decimate mesh", m_IsDecimated );
            surfaceFilter->SetParameter("Decimation rate", 0.8f );
          }
          else
          {*/
            surfaceFilter->SetParameter("Smooth", false );
            surfaceFilter->SetParameter("Apply median", false );
            surfaceFilter->SetParameter("Median kernel size", 3u );
            surfaceFilter->SetParameter("Gaussian SD", 1.5f );
            surfaceFilter->SetParameter("Decimate mesh", m_IsDecimated );
            surfaceFilter->SetParameter("Decimation rate", 0.8f );
          //}

          mitk::ProgressBar::GetInstance()->AddStepsToDo(10);
          mitk::ProgressBar::GetInstance()->Progress(2);
          mitk::StatusBar::GetInstance()->DisplayText("Surface creation started in background...");
          surfaceFilter->StartAlgorithm();
        }
        else
        {
          mitk::ShowSegmentationAsSmoothedSurface::Pointer filter = mitk::ShowSegmentationAsSmoothedSurface::New();

          itk::SimpleMemberCommand<QmitkCreatePolygonModelAction>::Pointer goodCommand = itk::SimpleMemberCommand<QmitkCreatePolygonModelAction>::New();
          goodCommand->SetCallbackFunction(this, &QmitkCreatePolygonModelAction::OnSurfaceCalculationDone);
          filter->AddObserver(mitk::ResultAvailable(), goodCommand);

          itk::SimpleMemberCommand<QmitkCreatePolygonModelAction>::Pointer badCommand = itk::SimpleMemberCommand<QmitkCreatePolygonModelAction>::New();
          badCommand->SetCallbackFunction(this, &QmitkCreatePolygonModelAction::OnSurfaceCalculationDone);
          filter->AddObserver(mitk::ProcessingError(), badCommand);

          mitk::DataNode::Pointer nodepointer = node;
          filter->SetPointerParameter("Input", image);
          filter->SetPointerParameter("Group node", nodepointer);
          filter->SetDataStorage(*m_DataStorage);

          berry::IPreferencesService::Pointer prefService = berry::Platform::GetServiceRegistry().GetServiceById<berry::IPreferencesService>(berry::IPreferencesService::ID);
          berry::IPreferences::Pointer segPref = prefService->GetSystemPreferences()->Node("/org.mitk.views.segmentation");

          bool smoothingHint = segPref->GetBool("smoothing hint", true);
          float smoothing = (float)segPref->GetDouble("smoothing value", 1.0);
          float decimation = (float)segPref->GetDouble("decimation rate", 0.5);
          
          if (smoothingHint)
          {
            smoothing = 0.0;
            mitk::Vector3D spacing = image->GetGeometry()->GetSpacing();
            
            for (mitk::Vector3D::Iterator iter = spacing.Begin(); iter != spacing.End(); ++iter)
              smoothing = std::max(smoothing, *iter);

            filter->SetParameter("Smoothing", smoothing);
          }
          else
          {
            filter->SetParameter("Smoothing", smoothing);
          }

          filter->SetParameter("Decimation", decimation);

          mitk::ProgressBar::GetInstance()->AddStepsToDo(10);
          mitk::ProgressBar::GetInstance()->Progress(1);
          mitk::StatusBar::GetInstance()->DisplayText("Smoothed surface creation started in background (this may take a while)...");

          filter->StartAlgorithm();
        }
      }
      catch(...)
      {
        MITK_ERROR << "surface creation filter had an error";
      }
    }
    else
    {
      MITK_INFO << "   a NULL node selected";
    }
  }
}

void QmitkCreatePolygonModelAction::OnSurfaceCalculationDone()
{
  mitk::ProgressBar::GetInstance()->Progress(8);
}

void QmitkCreatePolygonModelAction::SetSmoothed(bool smoothed)
{
  this->m_IsSmoothed = smoothed;
}

void QmitkCreatePolygonModelAction::SetDecimated(bool decimated)
{
  this->m_IsDecimated = decimated;
}

void QmitkCreatePolygonModelAction::SetDataStorage(mitk::DataStorage* dataStorage)
{
  this->m_DataStorage = dataStorage;
}

void QmitkCreatePolygonModelAction::SetFunctionality(berry::QtViewPart *functionality)
{
  //not needed
}
