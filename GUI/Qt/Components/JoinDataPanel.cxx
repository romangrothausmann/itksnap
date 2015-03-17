/*
part of Click'n'Join mode, which was contributed by Roman Grothausmann
*/

#include "JoinDataPanel.h"
#include "ui_JoinDataPanel.h"

#include "SNAPQtCommon.h" //findParentWidget
#include "GlobalUIModel.h" //GlobalUIModel
#include "JoinModel.h"
#include "GlobalState.h" //CROSSHAIRS_MODE
#include "IRISApplication.h"
#include "GenericImageData.h" //GetCurrentImageData()->GetImageRegion();
#include "QtWidgetActivator.h" //activateOnFlag
#include "DisplayLayoutModel.h" //access tiled/stacked view mode
#include "ImageIODelegates.h"
#include "ImageIOWizard.h"
#include "ImageIOWizardModel.h"


JoinDataPanel::JoinDataPanel(QWidget *parent) :
    SNAPComponent(parent),
    ui(new Ui::JoinDataPanel){
    ui->setupUi(this);
    }

JoinDataPanel::~JoinDataPanel(){
    delete ui;
    }

void JoinDataPanel::SetModel(GlobalUIModel *model){
    // Store the model
    m_Model = model;

    activateOnNotFlag(ui->btnStartCnJ, m_Model, UIF_JOIN_MODE);
    activateOnNotFlag(ui->inJoinType, m_Model, UIF_JOIN_MODE);
    activateOnFlag(ui->instrText, m_Model, UIF_JOIN_MODE);
    activateOnFlag(ui->btnFinishCnJ, m_Model, UIF_JOIN_MODE);
    activateOnFlag(ui->btnCancel, m_Model, UIF_JOIN_MODE);
    activateOnFlag(ui->btnCopySeg, m_Model, UIF_JOIN_MODE);
    activateOnFlag(ui->btnClearSeg, m_Model, UIF_JOIN_MODE);
    }

void JoinDataPanel::on_btnStartCnJ_clicked(){

    int sel = ui->inJoinType->currentIndex();
    switch(sel){
    case 0:{

	IRISApplication *driver = m_Model->GetDriver();

	//reset ROI from the main image
	GlobalState::RegionType roi =
	    driver->GetCurrentImageData()->GetImageRegion();
      
	// Can't be empty!
	assert(roi.GetNumberOfPixels());
      
	// Update
	driver->GetGlobalState()->SetSegmentationROI(roi);

	//// Initialize the image data
	driver->InitializeJOINImageData(
	    driver->GetGlobalState()->GetSegmentationROISettings(),
	    m_Model->GetProgressCommand());

	driver->CopySegementationToJsrc(
	    driver->GetGlobalState()->GetSegmentationROISettings(),
	    m_Model->GetProgressCommand());
	driver->SetCurrentImageDataToJOIN();

	// set tiled layout to ease understanding the interaction mode
	m_Model->GetDisplayLayoutModel()->GetSliceViewLayerLayoutModel()->SetValue(LAYOUT_TILED);
	} break;
    case 1:{
	////panel will be hidden in GWSJOIN_MODE
    m_Model->GetGlobalState()->SetToolbarMode(GLOBALWS_ROI_MODE);
    //ui->stack->setCurrentWidget(ui->pageSnakeTool);
    //ui->grpInspector->setTitle("GWS ROI Inspector");
	} break;
    case 2:{
	// Create a model for IO
	SmartPtr<LoadSegmentationImageDelegate> delegate = LoadSegmentationImageDelegate::New();
	delegate->Initialize(m_Model->GetDriver());
	SmartPtr<ImageIOWizardModel> model = ImageIOWizardModel::New();
	model->InitializeForLoad(m_Model, delegate,
	    "JsrImage", "Join Source Image");
	
	// Execute the IO wizard
	ImageIOWizard wiz(this);
	wiz.SetModel(model);
	wiz.exec();

	//todo check for cancel in wizard and return from JOIN_MODE

	IRISApplication *driver = m_Model->GetDriver();

	//reset ROI from the main image
	GlobalState::RegionType roi =
	    driver->GetCurrentImageData()->GetImageRegion();
      
	// Can't be empty!
	assert(roi.GetNumberOfPixels());
      
	// Update
	driver->GetGlobalState()->SetSegmentationROI(roi);

	//// Initialize the image data
	driver->InitializeJOINImageData(
	    driver->GetGlobalState()->GetSegmentationROISettings(),
	    m_Model->GetProgressCommand());

	driver->CopySegementationToJsrc(
	    driver->GetGlobalState()->GetSegmentationROISettings(),
	    m_Model->GetProgressCommand());
	driver->SetCurrentImageDataToJOIN();

	// set tiled layout to ease understanding the interaction mode
	m_Model->GetDisplayLayoutModel()->GetSliceViewLayerLayoutModel()->SetValue(LAYOUT_TILED);
	} break;
    default:
	////Switch to crosshairs mode
	m_Model->GetGlobalState()->SetToolbarMode(CROSSHAIRS_MODE);
	std::cerr << "Index not handled (yet)." << std::endl;
	break;
	}//switch
    }

void JoinDataPanel::on_btnFinishCnJ_clicked(){
    IRISApplication *driver = m_Model->GetDriver();
    GlobalState *gs = driver->GetGlobalState();

    // Update IRIS with JOIN images
    driver->UpdateIRISWithJOINImageData(NULL);

    // Set an undo point
    driver->StoreUndoPoint("Automatic Segmentation");

    // Return to IRIS mode
    driver->SetCurrentImageDataToIRIS();
    driver->ReleaseJOINImageData();

    ui->btnStartCnJ->setEnabled(true);
    ui->btnCancel->setEnabled(false);
    ui->btnFinishCnJ->setEnabled(false);
    ui->inJoinType->setEnabled(true);
    m_Model->GetGlobalState()->SetToolbarMode(CROSSHAIRS_MODE); //disables JoinInteraction
    }

void JoinDataPanel::on_btnCancel_clicked(){
    IRISApplication *driver = m_Model->GetDriver();
    GlobalState *gs = driver->GetGlobalState();

    // Return to IRIS mode
    driver->SetCurrentImageDataToIRIS();
    driver->ReleaseJOINImageData();

    ui->btnStartCnJ->setEnabled(true);
    ui->btnCancel->setEnabled(false);
    ui->btnFinishCnJ->setEnabled(false);
    ui->inJoinType->setEnabled(true);
    m_Model->GetGlobalState()->SetToolbarMode(CROSSHAIRS_MODE); //disables JoinInteraction
    }


void JoinDataPanel::on_btnCopySeg_clicked(){
    IRISApplication *driver = m_Model->GetDriver();
    driver->CopySegementationToJdst(
	driver->GetGlobalState()->GetSegmentationROISettings(),
	m_Model->GetProgressCommand());

    driver->InvokeEvent(SegmentationChangeEvent());

    }

void JoinDataPanel::on_btnClearSeg_clicked(){
    IRISApplication *driver = m_Model->GetDriver();
    driver->ClearJdst();

    driver->InvokeEvent(SegmentationChangeEvent());
    }