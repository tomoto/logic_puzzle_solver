#include "sli/serializer/Loader.h"
#include "sli/serializer/Formatter.h"
#include "sli/solver/Solver.h"
#include "utl/StrUtils.h"
#include "utl/Ansi.h"

#include <iostream>
#include <string>
#include <vector>
#include <chrono>

using namespace utl::StrUtils;
using namespace std::chrono;

struct Options {
  int speed = 100;
  enum class Format {
    PRETTY,
    PLAIN,
  } format = Format::PRETTY;
  bool result_only = false;

  bool pretty() const {
    return format == Format::PRETTY;
  }
};

auto parseArg(int argc, char** argv) {
  Options options;
  for (int i = 1; i < argc; ++i) {
    std::string a = argv[i];

    // No-argument options
    if (a == "-r" || a == "--result-only") {
      options.result_only = true;
    }

    // One-argument options
    if (i < argc - 1) {
      if (a == "-s" || a == "--speed") {
        options.speed = std::stoi(argv[++i]);
      } else if (a == "-f" || a == "--format") {
        std::string f = argv[++i];
        if (f == "plain") {
          options.format = Options::Format::PLAIN;
        }
      }
    }
  }

  // post-process
  if (options.result_only) {
    options.speed = 0;
  }

  return options;
}

auto load() {
  std::vector<std::string> lines;
  std::string line;
  while (std::getline(std::cin, line)) {
    trim(line);
    if (line.empty() || line.starts_with('#')) {
      continue;
    }
    lines.push_back(line);
  }

  return sli::Loader::loadNumbers(lines);
}

int main(int argc, char** argv) {
  auto options = parseArg(argc, argv);

  auto board = load();
  sli::Solver solver;
  int steps = 0;
  microseconds displayTime(0);

  auto display = [&](
    sli::Board& b,
    int depth,
    bool overwrite,
    bool dynamic
    ) {
      if (overwrite && !options.result_only) {
        std::cout << utl::Ansi::up(b.physicalHeight - 1);
      }
      sli::Formatter::formatFull(std::cout, b, {
        .pretty = options.pretty(),
        .highlightDirty = dynamic
        });
      std::cout << "steps: " << steps << " depth: " << depth << '\n';
    };

  auto reportProgress = [&](sli::Solver::BoardState& state, int depth) mutable {
    steps++;
    if (options.speed > 0 && steps % options.speed == 0) {
      auto start = high_resolution_clock::now();
      display(state.board, depth, true, true);
      auto end = high_resolution_clock::now();
      displayTime += duration_cast<microseconds>(end - start);
    }};

  if (!options.result_only) {
    display(board, 0, false, false);
  }

  auto start = high_resolution_clock::now();
  auto state = solver.solve(board, reportProgress);
  auto end = high_resolution_clock::now();
  auto elapsedTime = duration_cast<milliseconds>(end - start);
  auto calculationTime = duration_cast<milliseconds>(elapsedTime - displayTime);

  display(state.board, 0, true, false);
  std::cout << toString(state.status) << '\n';
  std::cout << "Elapsed time: " << elapsedTime.count() << " ms "
    << "(calculation only: " << calculationTime.count() << " ms)\n";

  return 0;
}
