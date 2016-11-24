#include "recogniser.h"

#include <stdlib.h>

#include <vector>

#include "gtest/gtest.h"
#include "knearest.h"
#include "opencv2/imgcodecs/imgcodecs.hpp"

using std::string;
using std::vector;

namespace {

string GetDataPath(const std::string& path) {
  string test_src_dir = getenv("TEST_SRCDIR");
  string test_workspace = getenv("TEST_WORKSPACE");
  string ret = test_src_dir + "/" + test_workspace + "/" + path;
  return ret;
}

class RecogniserTest : public ::testing::Test {
 protected:
  cv::Mat DecodeImage(const string& path) { return cv::imread(path); }
};

TEST_F(RecogniserTest, RecognisesGrid) {
  KNearest nearest;
  ASSERT_TRUE(nearest.Load(GetDataPath("data/model")));
  Recogniser recogniser(nearest);
  cv::Mat image = DecodeImage(GetDataPath("testdata/words2.png"));
  ASSERT_FALSE(image.data == nullptr);
  EXPECT_EQ(1440, image.size().width);
  EXPECT_EQ(2560, image.size().height);
  vector<char> grid = recogniser.RecogniseGrid(image);
}

}  // namespace

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
