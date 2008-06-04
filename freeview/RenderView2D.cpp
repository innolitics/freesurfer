/**
 * @file  RenderView2D.cpp
 * @brief View for rendering.
 *
 */
/*
 * Original Author: Ruopeng Wang
 * CVS Revision Info:
 *    $Author: nicks $
 *    $Date: 2008/06/04 20:43:25 $
 *    $Revision: 1.5.2.1 $
 *
 * Copyright (C) 2002-2007,
 * The General Hospital Corporation (Boston, MA). 
 * All rights reserved.
 *
 * Distribution, usage and copying of this software is covered under the
 * terms found in the License Agreement file named 'COPYING' found in the
 * FreeSurfer source code root directory, and duplicated here:
 * https://surfer.nmr.mgh.harvard.edu/fswiki/FreeSurferOpenSourceLicense
 *
 * General inquiries: freesurfer@nmr.mgh.harvard.edu
 * Bug reports: analysis-bugs@nmr.mgh.harvard.edu
 *
 */
 
#include <wx/xrc/xmlres.h> 
#include "RenderView2D.h"
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <vtkCommand.h>
#include <vtkMath.h>
#include <vtkActor2D.h>
#include "LayerCollection.h"
#include "MainWindow.h"
#include "vtkPropCollection.h"
#include "LayerMRI.h"
#include "LayerPropertiesMRI.h"
#include "Annotation2D.h"
#include "Cursor2D.h"
#include "Interactor2DNavigate.h"
#include "Interactor2DROIEdit.h"
#include "Interactor2DVoxelEdit.h"
#include "MyUtils.h"

#define max(a,b)  (((a) > (b)) ? (a) : (b))
#define min(a,b)  (((a) < (b)) ? (a) : (b))
#define SCALE_FACTOR		200

IMPLEMENT_DYNAMIC_CLASS(RenderView2D, RenderView)

BEGIN_EVENT_TABLE(RenderView2D, RenderView) 
	EVT_SIZE        ( RenderView2D::OnSize )
END_EVENT_TABLE()

RenderView2D::RenderView2D() : RenderView(), m_nViewPlane( 0 )
{
	Initialize2D();
}

RenderView2D::RenderView2D( wxWindow* parent, int id ) : RenderView( parent, id )
{
	Initialize2D();
}

void RenderView2D::Initialize2D()
{
	m_nViewPlane = 0;
	m_renderer->GetActiveCamera()->ParallelProjectionOn();
	m_annotation2D = new Annotation2D;	
	m_cursor2D = new Cursor2D( this );
	
	m_interactorNavigate = new Interactor2DNavigate();
	m_interactorVoxelEdit = new Interactor2DVoxelEdit();
	m_interactorROIEdit = new Interactor2DROIEdit();
	
	m_interactorNavigate->AddListener( MainWindow::GetMainWindowPointer() );
	m_interactorVoxelEdit->AddListener( MainWindow::GetMainWindowPointer() );
	m_interactorROIEdit->AddListener( MainWindow::GetMainWindowPointer() );
	
	SetInteractionMode( IM_Navigate );
}

RenderView2D* RenderView2D::New()
{
  // we don't make use of the objectfactory, because we're not registered
  return new RenderView2D;
}

RenderView2D::~RenderView2D()
{
	m_interactor = NULL;
	delete m_annotation2D;
	delete m_cursor2D;
	delete m_interactorNavigate;
	delete m_interactorVoxelEdit;
	delete m_interactorROIEdit;
}

void RenderView2D::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);
}

void RenderView2D::SetInteractionMode( int nMode )
{
	RenderView::SetInteractionMode( nMode );
	
//	if ( m_interactor )
//		delete m_interactor;
	
	switch ( nMode )
	{
		case IM_ROIEdit:
			m_interactor = m_interactorROIEdit;
			break;
		case IM_VoxelEdit:
			m_interactor = m_interactorVoxelEdit;
			break;
		default:
			m_interactor = m_interactorNavigate;
			break;
	}
		
//	m_interactor->AddListener( MainWindow::GetMainWindowPointer() );
}

void RenderView2D::RefreshAllActors()
{
	LayerCollectionManager* lcm = MainWindow::GetMainWindowPointer()->GetLayerCollectionManager();
	
	m_renderer->RemoveAllViewProps();
	lcm->Append2DProps( m_renderer, m_nViewPlane );
	
	// add coordinate annotation
	m_annotation2D->AppendAnnotations( m_renderer );
	m_cursor2D->AppendCursor( m_renderer );
	
	// add focus frame
	m_renderer->AddViewProp( m_actorFocusFrame );
	
	Render();
}

void RenderView2D::SetViewPlane( int nPlane )
{
	m_nViewPlane = nPlane;
}
	
int RenderView2D::GetViewPlane()
{
	return m_nViewPlane;
}

void RenderView2D::UpdateViewByWorldCoordinate()
{
	vtkCamera* cam = m_renderer->GetActiveCamera();
	double wcenter[3];
	for ( int i = 0; i < 3; i++ )
	{
		wcenter[i] = m_dWorldOrigin[i] + m_dWorldSize[i] / 2;
	}
	cam->SetFocalPoint( wcenter );
	switch ( m_nViewPlane )
	{
		case 0:
			cam->SetPosition( wcenter[0] + m_dWorldSize[0], wcenter[1], wcenter[2] );
			cam->SetViewUp( 0, 0, 1 );
			break;
		case 1:
			cam->SetPosition( wcenter[0], wcenter[1] + m_dWorldSize[1], wcenter[2] );
			cam->SetViewUp( 0, 0, 1 );
			break;
		case 2:
			cam->SetPosition( wcenter[0], wcenter[1], wcenter[2] - m_dWorldSize[2] );
			break;
	}
	cam->SetParallelScale( max( max(m_dWorldSize[0], m_dWorldSize[1]), m_dWorldSize[2]) / 2 );
//	m_renderer->ResetCameraClippingRange(); 
}

void RenderView2D::UpdateAnnotation()
{
	m_annotation2D->Update( m_renderer, m_nViewPlane );
}

void RenderView2D::DoListenToMessage ( std::string const iMsg, void* const iData )
{
	if ( iMsg == "LayerActorUpdated" || 
			iMsg == "LayerAdded" || 
			iMsg == "LayerMoved" || 
			iMsg == "LayerRemoved" )
	{
		UpdateAnnotation();	
	}
	else if ( iMsg == "CursorRASPositionChanged" )
	{
		LayerCollection* lc = MainWindow::GetMainWindowPointer()->GetLayerCollection( "MRI" );
		m_cursor2D->SetPosition( lc->GetCursorRASPosition() );
	}
	
	RenderView::DoListenToMessage( iMsg, iData );
}
		
void RenderView2D::TriggerContextMenu( const wxPoint& pos )
{
/*	wxMenu menu;
	menu.Append(XRCID("ID_FILE_EXIT"), _T("E&xit"));
	PopupMenu(&menu, pos.x, pos.y);
	*/
}


void RenderView2D::UpdateMouseRASPosition( int posX, int posY )
{	
	LayerCollection* lc = MainWindow::GetMainWindowPointer()->GetLayerCollection( "MRI" );	
	if ( !lc )
		return;
	
	double pos[3];
	MousePositionToRAS( posX, posY, pos );
	
	lc->SetCurrentRASPosition( pos );
}

void RenderView2D::UpdateCursorRASPosition( int posX, int posY, bool bConnectPrevious )
{
	LayerCollection* lc = MainWindow::GetMainWindowPointer()->GetLayerCollection( "MRI" );	
	if ( !lc )
		return;
	
	double pos[3];
	MousePositionToRAS( posX, posY, pos );
	
	lc->SetCursorRASPosition( pos );
	lc->SetSlicePosition( pos );
	
//	m_cursor2D->SetPosition( pos, bConnectPrevious );
}

void RenderView2D::MousePositionToRAS( int posX, int posY, double* pos )
{
	wxSize sz = GetClientSize();
	pos[0] = posX;
	pos[1] = sz.GetHeight() - posY;
	pos[2] = 0;
//	m_renderer->DisplayToNormalizedDisplay( pos[0], pos[1] );
//	m_renderer->NormalizedDisplayToViewport( pos[0], pos[1] );
	m_renderer->ViewportToNormalizedViewport( pos[0], pos[1] );
	m_renderer->NormalizedViewportToView( pos[0], pos[1], pos[2] );
	m_renderer->ViewToWorld( pos[0], pos[1], pos[2] );
	
	double slicePos[3];
	MainWindow::GetMainWindowPointer()->GetLayerCollection( "MRI" )->GetSlicePosition( slicePos );
	pos[m_nViewPlane] = slicePos[m_nViewPlane];
}

void RenderView2D::ZoomAtCursor( int nX, int nY, bool ZoomIn )
{
	wxSize sz = GetClientSize();
	double pos[3];
	pos[0] = nX;
	pos[1] = sz.GetHeight() - nY;
	pos[2] = 0;
	m_renderer->ViewportToNormalizedViewport( pos[0], pos[1] );
	m_renderer->NormalizedViewportToView( pos[0], pos[1], pos[2] );
	m_renderer->ViewToWorld( pos[0], pos[1], pos[2] );
	
	// first move the click point to the center of viewport
	double focalPt[3], camPos[3], vproj[3];
	vtkCamera* cam = m_renderer->GetActiveCamera();
	cam->GetFocalPoint( focalPt );
	cam->GetPosition( camPos );
	cam->GetDirectionOfProjection( vproj );
	double camDist = cam->GetDistance();
	
	double dist = MyUtils::GetDistance( pos, focalPt );
	double v[3];
	MyUtils::GetVector( pos, focalPt, v );
	dist *= MyUtils::Dot( vproj, v );
	
	for ( int i = 0; i < 3; i++ )
	{
		focalPt[i] = pos[i] + vproj[i] * dist;
		camPos[i] = focalPt[i] - vproj[i] * camDist;
	}
	
	cam->SetFocalPoint( focalPt );
	cam->SetPosition( camPos );
	
	// then zoom
	cam->Zoom( ZoomIn ? 2 : 0.5 );
	UpdateCursor();
	UpdateAnnotation();
	Render();
}

void RenderView2D::UpdateCursor()
{
	m_cursor2D->Update();
}

void RenderView2D::MoveLeft()
{
	vtkCamera* cam = m_renderer->GetActiveCamera();
	double viewup[3], proj[3], v[3];
	cam->GetViewUp( viewup );
	cam->GetDirectionOfProjection( proj );
	vtkMath::Cross( viewup, proj, v );
	double focal_pt[3], cam_pos[3];
	cam->GetFocalPoint( focal_pt );
	cam->GetPosition( cam_pos );
	double scale = min( min( m_dWorldSize[0], m_dWorldSize[1] ), m_dWorldSize[2] ) / SCALE_FACTOR;
	for ( int i = 0; i < 3; i++ )
	{
		focal_pt[i] -= v[i] * scale;
		cam_pos[i] -= v[i] * scale;
	}
	cam->SetFocalPoint( focal_pt );
	cam->SetPosition( cam_pos );
	
	UpdateAnnotation();
	UpdateCursor();
	NeedRedraw();
}

void RenderView2D::MoveRight()
{
	vtkCamera* cam = m_renderer->GetActiveCamera();
	double viewup[3], proj[3], v[3];
	cam->GetViewUp( viewup );
	cam->GetDirectionOfProjection( proj );
	vtkMath::Cross( viewup, proj, v );
	double focal_pt[3], cam_pos[3];
	cam->GetFocalPoint( focal_pt );
	cam->GetPosition( cam_pos );
	double scale = min( min( m_dWorldSize[0], m_dWorldSize[1] ), m_dWorldSize[2] ) / SCALE_FACTOR;
	for ( int i = 0; i < 3; i++ )
	{
		focal_pt[i] += v[i] * scale;
		cam_pos[i] += v[i] * scale;
	}
	cam->SetFocalPoint( focal_pt );
	cam->SetPosition( cam_pos );
	
	UpdateAnnotation();
	UpdateCursor();
	NeedRedraw();
}

void RenderView2D::MoveUp()
{
	vtkCamera* cam = m_renderer->GetActiveCamera();
	double v[3];
	cam->GetViewUp( v );
	double focal_pt[3], cam_pos[3];
	cam->GetFocalPoint( focal_pt );
	cam->GetPosition( cam_pos );
	double scale = min( min( m_dWorldSize[0], m_dWorldSize[1] ), m_dWorldSize[2] ) / SCALE_FACTOR;
	for ( int i = 0; i < 3; i++ )
	{
		focal_pt[i] -= v[i] * scale;
		cam_pos[i] -= v[i] * scale;
	}
	cam->SetFocalPoint( focal_pt );
	cam->SetPosition( cam_pos );
	
	UpdateAnnotation();
	UpdateCursor();
	NeedRedraw();
}
	
void RenderView2D::MoveDown()
{
	vtkCamera* cam = m_renderer->GetActiveCamera();
	double v[3];
	cam->GetViewUp( v );
	double focal_pt[3], cam_pos[3];
	cam->GetFocalPoint( focal_pt );
	cam->GetPosition( cam_pos );
	double scale = min( min( m_dWorldSize[0], m_dWorldSize[1] ), m_dWorldSize[2] ) / SCALE_FACTOR;
	for ( int i = 0; i < 3; i++ )
	{
		focal_pt[i] += v[i] * scale;
		cam_pos[i] += v[i] * scale;
	}
	cam->SetFocalPoint( focal_pt );
	cam->SetPosition( cam_pos );
	
	UpdateAnnotation();
	UpdateCursor();
	NeedRedraw();
}



void RenderView2D::OnSize( wxSizeEvent& event )
{
	UpdateCursor();
	UpdateAnnotation();
	
	event.Skip();
}

void RenderView2D::PreScreenshot()
{
	LayerCollectionManager* lcm = MainWindow::GetMainWindowPointer()->GetLayerCollectionManager();
	
	m_renderer->RemoveAllViewProps();
	lcm->Append2DProps( m_renderer, m_nViewPlane );
	
	// add coordinate annotation
	m_annotation2D->AppendAnnotations( m_renderer );
//	m_cursor2D->AppendCursor( m_renderer );
	
	// add focus frame
//	m_renderer->AddViewProp( m_actorFocusFrame );
}


void RenderView2D::PostScreenshot()
{
	RefreshAllActors();
}

