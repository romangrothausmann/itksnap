/*=========================================================================

JsrcToRGBAImageFilter combines LabelToRGBAFilter and itkUnaryFunctorImageFilter
to allow label mapping as common for ITKSnap or an "infinit LUT" with itkScalarToRGBPixelFunctor
Note: JsrcToRGBAImageFilter is templated while LabelToRGBAFilter is not

=========================================================================*/
#ifndef __JsrcToRGBAImageFilter_h_
#define __JsrcToRGBAImageFilter_h_

#include "SNAPCommon.h"

#include <itkRGBAPixel.h>
#include <itkNumericTraitsRGBAPixel.h>

/**
 * \class JsrcToRGBAImageFilter
 * \brief Simple filter that combines LabelToRGBAFilter and itkUnaryFunctorImageFilter
 */
class JsrcToRGBAImageFilter: 
  public itk::ImageToImageFilter<
itk::Image<LabelType, 2> , itk::Image<itk::RGBAPixel<unsigned char>,2> > //needs templating on InputPixelType
{
public:
  
  /** Pixel Type of the input image */
  typedef itk::Image<InputPixelType, 2>                  InputImageType;
  typedef itk::SmartPointer<InputImageType>           InputImagePointer;

  /** Pixel Type of the output image */
  typedef itk::RGBAPixel<unsigned char>                 OutputPixelType;
  typedef itk::Image<OutputPixelType, 2>                OutputImageType;
  typedef itk::SmartPointer<OutputImageType>         OutputImagePointer;

  /** Standard class typedefs. */
  typedef JsrcToRGBAImageFilter                                        Self;
  typedef itk::ImageToImageFilter<InputImageType,OutputImageType>  Superclass;
  typedef itk::SmartPointer<Self>                               Pointer;
  typedef itk::SmartPointer<const Self>                    ConstPointer;  
  
  /** Method for creation through the object factory. */
  itkNewMacro(Self)
    
  /** Image dimension. */
  itkStaticConstMacro(ImageDimension, unsigned int,
                      InputImageType::ImageDimension);

  /** Set color table macro */
  void SetColorTable(ColorLabelTable *table)
  {
    m_ColorTable = table;
    this->SetNthInput(1, table);
  }
  
  /** Get color table */
  ColorLabelTable *GetColorTable()
  {
    return m_ColorTable;
  }

  /** Set UseFunctor */
  void SetUseFunctor(bool useFunctor)
  {
    m_useFunctor = useFunctor;
   }
  
  /** Get UseFunctor */
  bool *GetUseFunctor()
  {
    return m_useFunctor;
  }

protected:

  void PrintSelf(std::ostream& os, itk::Indent indent) const
    { os << indent << ""; }

  ::MappingFunctor
::operator()(PixelType in)
{
  itk::RGBPixel<DisplayPixelType::ComponentType> RGBp;
  DisplayPixelType RGBAp;
  itk::Functor::ScalarToRGBPixelFunctor<JSRType> mFunctor;

  RGBp= mFunctor(in);
  RGBAp[0]= RGBp[0];
  RGBAp[1]= RGBp[1];
  RGBAp[2]= RGBp[2];
  RGBAp[3]= (in == 0) ? 0 : itk::NumericTraits<DisplayPixelType::ComponentType>::max(); //make label 0 fully transparent

  return  RGBAp;
}

  //in constructor:
      {
      typedef itk::UnaryFunctorImageFilter<InputSliceType, DisplaySliceType, MappingFunctor> FunctorImageFilterType;
      FunctorImageFilterType::Pointer FunctorImageFilter = FunctorImageFilterType::New();
      //FunctorImageFilter->SetFunctor(m_Functor);
      
      typedef LabelToRGBAFilter RGBAFilterType;
      RGBAFilterType::Pointer LabelToRGBAFilter = RGBAFilterType::New();
      }

  /** Generate Data */
  void GenerateData( void )
    {
    // Here's the input and output
    InputImageType::ConstPointer inputPtr = this->GetInput();
    OutputImageType::Pointer outputPtr = this->GetOutput();

    // Get the number of pixels in the input
    size_t n = inputPtr->GetBufferedRegion().GetNumberOfPixels();

    // Allocate output if needed
    if(outputPtr->GetBufferedRegion().GetNumberOfPixels() != n)
      {
      outputPtr->SetBufferedRegion(inputPtr->GetBufferedRegion());
      outputPtr->Allocate();
      }


    
    if(m_useFunctor)
        outputPtr = FunctorImageFilter->GetOutput();
    else
        outputPtr = LabelToRGBAFilter->GetOutput();

    //outputPtr->DisconnectPipeline(); //likely not needed, if do not forget Update() on filters before!
    }

private:
  ColorLabelTable *m_ColorTable;
  bool m_useFunctor;
};

#endif
