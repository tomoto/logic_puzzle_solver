#include "nur/data/Board.h"
#include "nur/serializer/Loader.h"
#include "nur/serializer/Formatter.h"
#include "nur/solver/rule/ShimaCompletionRule.h"
#include "nur/solver/rule/ShimaBorderRule.h"
#include "nur/solver/rule/SPOverwrapRule.h"
#include "nur/solver/rule/ShimaPotentialReachabilityRule.h"

#include "nur/solver/topology/Topology.h"
#include "nur/solver/topology/KabeTopologicalRule.h"

#include <gtest/gtest.h>

using namespace nur;

TEST(nur_Rules, shimaCompletionRule) {
  auto b = Loader::load({
    "0 3 0",
    "0 0 2",
    "0 0 0",
    });

  {
    ShimaCompletionRule r;
    EXPECT_TRUE(r.apply(b));

    EXPECT_EQ(Formatter::formatData(b),
      "State:\n"
      "1 1 0\n"
      "1 0 2\n"
      "0 0 2\n"
      "Shima 1@(1,0):\n"
      "2 3 0\n"
      "1 0 0\n"
      "0 0 0\n"
      "Shima 2@(2,1):\n"
      "0 0\n"
      "0 2\n"
      "0 1\n"
    );

    EXPECT_FALSE(r.apply(b));
    EXPECT_TRUE(b.shima(1).complete());
    EXPECT_TRUE(b.shima(2).complete());
  }

  {
    ShimaBorderRule r;
    EXPECT_TRUE(r.apply(b));

    EXPECT_EQ(Formatter::formatData(b),
      "State:\n"
      " 1  1 -3\n"
      " 1 -1  2\n"
      "-1 -1  2\n"
      "Shima 1@(1,0):\n"
      "2 3 0\n"
      "1 0 0\n"
      "0 0 0\n"
      "Shima 2@(2,1):\n"
      "0 0\n"
      "0 2\n"
      "0 1\n"
    );

    EXPECT_FALSE(r.apply(b));
  }
}

// TEST(nur_Rules, justReadingRealProblem) {
//   auto b = Loader::load({
//     "0 5 0 0 0 0 3 0 0 0 0 0 0 0 0 0 0",
//     "0 0 1 0 0 0 0 1 0 0 0 0 0 0 0 0 7",
//     "0 0 0 2 0 0 0 0 3 0 0 1 0 0 0 1 0",
//     "0 0 0 0 0 0 0 0 0 0 1 0 0 0 5 0 0",
//     "0 0 0 0 0 0 0 0 0 3 0 0 0 0 0 0 0",
//     "0 0 0 0 0 4 0 0 0 0 0 0 2 0 0 0 0",
//     "0 5 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0",
//     "0 0 1 0 0 0 0 9 0 0 0 3 0 0 7 0 0",
//     "0 0 0 4 0 0 0 0 1 0 0 0 0 0 0 0 0",
//     "0 0 0 0 0 0 0 0 0 5 0 0 0 0 0 0 0",
//     "3 0 0 0 0 0 3 0 0 0 0 0 0 0 0 0 2",
//     "0 1 0 0 0 0 0 1 0 0 0 0 0 0 0 1 0",
//     "0 0 5 0 0 0 0 0 2 0 0 0 0 0 5 0 0",
//     "0 0 0 0 0 0 0 0 0 0 4 0 0 0 0 0 0",
//     "0 0 0 0 0 5 0 0 0 0 0 1 0 0 0 0 0",
//     "0 0 0 0 1 0 0 0 0 0 0 0 3 0 0 0 0",
//     "4 0 0 3 0 0 0 0 4 0 0 0 0 0 2 0 4",
//     });

//   EXPECT_TRUE(ShimaInitialRule().apply(b));
//   EXPECT_TRUE(ShimaCompletionRule().apply(b));
//   EXPECT_TRUE(ShimaBorderRule().apply(b));
//   EXPECT_TRUE(SPOverwrapRule().apply(b));
//   EXPECT_TRUE(ShimaBorderRule().apply(b));
//   EXPECT_TRUE(SPOverwrapRule().apply(b));
//   EXPECT_TRUE(ShimaBorderRule().apply(b));
//   EXPECT_TRUE(SPOverwrapRule().apply(b));
//   EXPECT_TRUE(ShimaPotentialReachabilityRule().apply(b));
//   EXPECT_TRUE(ShimaBorderRule().apply(b));

//   auto topology = Topology::analyze(b);
//   EXPECT_TRUE(KabeTopologyRule().apply(b, topology));
//   EXPECT_TRUE(SPOverwrapRule().apply(b));

//   FAIL() << Formatter::formatPretty(b) << Formatter::formatData(b) << std::endl;
// }

