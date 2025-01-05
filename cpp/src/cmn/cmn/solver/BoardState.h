#pragma once

namespace cmn {

  enum class CompletionStatus {
    complete = 1,
    incomplete,
    hatan,
  };

  inline const char* toString(CompletionStatus status) {
    switch (status) {
    case CompletionStatus::complete:
      return "complete";
    case CompletionStatus::incomplete:
      return "incomplete";
    case CompletionStatus::hatan:
      return "hatan";
    default:
      return "?";
    }
  }

  template<typename B, typename P>
  struct BoardState {
    B board;
    P topology;
    CompletionStatus status = CompletionStatus::incomplete;
  };

  template<typename B, typename P, typename A>
  struct BoardStateTrial : public BoardState<B, P> {
    A assumption;
  };

} // namespace cmn
