// Author: Mingcheng Chen (linyufly@gmail.com)

#include "watershed_corrector.h"

#include <vtkSmartPointer.h>
#include <vtkStructuredPoints.h>
#include <vtkStructuredPointsReader.h>
#include <vtkStructuredPointsWriter.h>

namespace {

const char *kScalarFieldFile = "/home/linyufly/GitLab/RidgeExtraction/convective_half_ftle.vtk";
const char *kLabelFieldFile = "/home/linyufly/GitLab/RidgeExtraction/convective_half_label_dig.vtk";
const char *kOutputFile = "corrected_label.vtk";

}

void correct_test() {
  printf("correct_test {\n");

  vtkSmartPointer<vtkStructuredPointsReader> reader =
      vtkSmartPointer<vtkStructuredPointsReader>::New();
  reader->SetFileName(kScalarFieldFile);
  reader->Update();

  vtkSmartPointer<vtkStructuredPoints> scalar_field =
      vtkSmartPointer<vtkStructuredPoints>::New();
  scalar_field->DeepCopy(reader->GetOutput());

  reader->SetFileName(kLabelFieldFile);
  reader->Update();

  vtkSmartPointer<vtkStructuredPoints> label_field =
      vtkSmartPointer<vtkStructuredPoints>::New();
  label_field->DeepCopy(reader->GetOutput());

  WatershedCorrector::correct(scalar_field, label_field);

  vtkSmartPointer<vtkStructuredPointWriter> writer =
      vtkSmartPointer<vtkStructuredPointsWriter>::New();
  writer->SetInputData(label_field);
  writer->SetFileName(kOutputFile);
  writer->Write();

  printf("} correct_test\n");
}

int main() {
  correct_test();

  return 0;
}

