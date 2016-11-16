#include <dirent.h>
#include <string.h>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <string>

#include <gflags/gflags.h>

#include "knearest.h"

using std::cerr;
using std::endl;
using std::string;

DEFINE_string(output, "", "Path to write trained model to");

void TrainDirectory(const string& path, const string& name,
                    KNearest* knearest) {
  DIR* training_directory = opendir(path.c_str());

  dirent* directory_info = nullptr;
  while ((directory_info = readdir(training_directory))) {
    if (directory_info->d_type != DT_REG) {
      continue;
    }
    string file_path = path + "/";
    file_path += directory_info->d_name;
    cv::Mat image = cv::imread(file_path, 0);
    if (image.data == nullptr) {
      cerr << "Failed to load image: " << file_path << endl;
      exit(1);
    }
    knearest->Learn(image, *path.rbegin());
  }
  closedir(training_directory);
}

int main(int argc, char** argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  KNearest knearest;

  for (int i = 1; i < argc; ++i) {
    string path(argv[i]);
    size_t index = path.rfind('/');
    cv::Mat image = cv::imread(path, 0);
    knearest.Learn(image, path[index - 1]);
  }

  knearest.Train();
  knearest.Save(FLAGS_output);

  return 0;
}
