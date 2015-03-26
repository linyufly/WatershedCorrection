// Author: Mingcheng Chen (linyufly@gmail.com)

#include "watershed_corrector.h"

#include <algorithm>
#include <vector>

#include <vtkDataArray.h>
#include <vtkPointData.h>
#include <vtkSmartPointer.h>
#include <vtkStructuredPoints.h>

namespace {

const int kConnectivity = 6;

const int kDirections[6][3] = {
    {1, 0, 0}, {-1, 0, 0},
    {0, 1, 0}, {0, -1, 0},
    {0, 0, 1}, {0, 0, -1}};

struct GridPoint {
  int x_, y_, z_;
  double scalar_;

  GridPoint() {
  }

  GridPoint(int x, int y, int z, double scalar)
      : x_(x), y_(y), z_(z), scalar_(scalar) {
  }

  bool operator < (const GridPoint &other) const {
    return scalar_ < other.scalar_;
  }
};

int get_code(int index[], int dimensions[]) {
  return (index[2] * dimensions[1] + index[1])
         * dimensions[0] + index[0];
}

bool outside(int index[], int dimensions[]) {
  return index[0] < 0 || index[1] < 0 || index[2] < 0
         || index[0] >= dimensions[0]
         || index[1] >= dimensions[1]
         || index[2] >= dimensions[2];
}

}

void WatershedCorrector::correct(
    vtkStructuredPoints *scalar_field,
    vtkStructuredPoints *label_field) {
  int dimensions[3];
  scalar_field->GetDimensions(dimensions);
  int nx = dimensions[0];
  int ny = dimensions[1];
  int nz = dimensions[2];

  std::vector<GridPoint> sorted_points;

  for (int x = 0; x < nx; x++) {
    for (int y = 0; y < ny; y++) {
      for (int z = 0; z < nz; z++) {
        int curr_index[] = {x, y, z};
        int curr_code = get_code(curr_index, dimensions);
        sorted_points.push_back(
            GridPoint(
                x, y, z, scalar_field->GetPointData()->GetTuple1(curr_code)));
      }
    }
  }

  std:sort(sorted_points.begin(), sorted_points.end());

  for (int p = 0; p < sorted_points.size(); p++) {
    GridPoint curr_point = sorted_points[p];
    int x = curr_point.x_;
    int y = curr_point.y_;
    int z = curr_point.z_;
    double curr_scalar = curr_point.scalar_;

    double lowest = curr_scalar;

    std::set<double> lowest_labels;

    for (int d = 0; d < kConnectivity; d++) {
      int next_x = x + kDirection[d][0];
      int next_y = y + kDirection[d][1];
      int next_z = z + kDirection[d][2];

      int next_index[] = {next_x, next_y, next_z};
      if (outside(next_index, dimensions)) {
        continue;
      }

      int next_code = get_code(next_index, dimensions);
      double next_scalar = scalar_field->GetPointData()->GetTuple1(next_code);
      if (next_scalar < lowest) {
        lowest = next_scalar;
        lowest_labels.clear();
      }

      if (next_scalar == lowest) {
        lowest_labels.insert(
            label_field->GetPointData()->GetTuple1(next_code));
      }
    }

    if (!lowest_labels.size()) {
      continue;
    }

    int curr_index[] = {x, y, z};
    int curr_code = get_code(curr_index, dimensions);
    double curr_label = label_field->GetPointData()->GetTuple1(curr_code);
    if (lowest_labels.find(curr_label) == lowest_labels.end()) {
      curr_label = *lowest_labels.begin();
      label_field->GetPointData()->SetTuple1(curr_code, curr_label);
    }
  }
}
