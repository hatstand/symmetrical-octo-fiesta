#include <dirent.h>
#include <string.h>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <string>

#include "knearest.h"

using std::cerr;
using std::endl;
using std::string;

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
  KNearest knearest;

  for (int i = 2; i < argc; ++i) {
    string path(argv[i]);
    size_t index = path.rfind('/');
    cv::Mat image = cv::imread(path, 0);
    knearest.Learn(image, path[index - 1]);
  }

  knearest.Train();
  knearest.Save(argv[1]);

  return 0;
}
