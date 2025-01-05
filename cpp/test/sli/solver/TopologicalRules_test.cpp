#include "sli/solver/topology/AvoidLoopTopologicalRule.h"
#include "sli/solver/topology/TrackAnalysisSubject.h"
#include "cmn/solver/topology/TrackAnalyzer.h"
#include "sli/serializer/Loader.h"
#include "sli/serializer/Formatter.h"
#include "utl/Utils.h"

#include <gtest/gtest.h>
#include <sstream>

using namespace sli;

TEST(sli_TopologicalRules, basic) {
  auto b = Loader::loadFull({
    "+ + +-+",
    "x   | |",
    "+ + + +",
    "  |   |",
    "+-+x+-+",
    });

  {
    AvoidLoopTopologicalRule r;
    auto t = cmn::TrackAnalyzer::analyze(TrackAnalysisSubject(b));
    EXPECT_EQ(r.apply(b, t), true);
    EXPECT_EQ(Formatter::formatFull(b),
      "+ + +-+\n"
      "x   | |\n"
      "+x+ + +\n"
      "x | x |\n"
      "+-+x+-+\n"
    );
  }
}