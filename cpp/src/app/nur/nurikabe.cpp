#include "nur/serializer/Loader.h"
#include "nur/serializer/Formatter.h"
#include "nur/solver/Solver.h"
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
};

auto parseArg(int argc, char** argv) {
  Options result;
  for (int i = 1; i < argc - 1; ++i) {
    std::string a = argv[i];
    if (a == "-s" || a == "--speed") {
      result.speed = std::stoi(argv[++i]);
    }
  }
  return result;
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

  return nur::Loader::load(lines);
}

int main(int argc, char** argv) {
  auto options = parseArg(argc, argv);

  auto board = load();
  nur::Solver solver;
  int steps = 0;
  microseconds displayTime(0);

  auto display = [&](
    nur::Board& b,
    int depth,
    bool overwrite,
    bool dynamic
    ) {
      if (overwrite) {
        std::cout << utl::Ansi::up(b.height + 1);
      }
      std::cout << nur::Formatter::formatPretty(b);
      std::cout << "steps: " << steps << " depth: " << depth << '\n';
    };

  auto reportProgress = [&](nur::Solver::BoardState& state, int depth) mutable {
    steps++;
    if (options.speed > 0 && steps % options.speed == 0) {
      auto start = high_resolution_clock::now();
      display(state.board, depth, true, true);
      auto end = high_resolution_clock::now();
      displayTime += duration_cast<microseconds>(end - start);
    }};

  display(board, 0, false, false);

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
