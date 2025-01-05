#include "sli/solver/topology/TrackAnalysisSubject.h"
#include "sli/serializer/Loader.h"
#include "sli/serializer/Formatter.h"

#include <gtest/gtest.h>

using namespace sli;

TEST(sli_Track, analyze) {
  auto b = Loader::loadFull({
    "+-+ +",
    "  | |",
    "+ +-+",
    "|    ",
    "+-+ +",
    });

  auto s = TrackAnalysisSubject(b);
  auto t = cmn::TrackAnalyzer::analyze(s);
  EXPECT_EQ(t.totalLeg, 6);
  EXPECT_EQ(t.tracks.size(), 2);
}
