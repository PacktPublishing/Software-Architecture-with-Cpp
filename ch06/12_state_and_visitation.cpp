#include <iostream>
#include <stdexcept>
#include <variant>

namespace state {

struct Depleted {};

struct Available {
  int count;
};

struct Discontinued {};
}  // namespace state

using State =
    std::variant<state::Depleted, state::Available, state::Discontinued>;

namespace event {

struct DeliveryArrived {
  int count;
};

struct Purchased {
  int count;
};

struct Discontinued {};

}  // namespace event

State on_event(state::Available available, event::DeliveryArrived delivered) {
  available.count += delivered.count;
  return available;
}

State on_event(state::Available available, event::Purchased purchased) {
  available.count -= purchased.count;
  if (available.count > 0) return available;
  return state::Depleted{};
}

template <typename S>
State on_event(S, event::Discontinued) {
  return state::Discontinued{};
}

State on_event(state::Depleted depleted, event::DeliveryArrived delivered) {
  return state::Available{delivered.count};
}

template <class... Ts>
struct overload : Ts... {
  using Ts::operator()...;
};
template <class... Ts>
overload(Ts...)->overload<Ts...>;

class ItemStateMachine {
 public:
  template <typename Event>
  void process_event(Event &&event) {
    state_ = std::visit(overload{
        [&](const auto &state) requires std::is_same_v<
            decltype(on_event(state, std::forward<Event>(event))), State> {
          return on_event(state, std::forward<Event>(event));
  }
  , [](const auto &unsupported_state) -> State {
    throw std::logic_error{"Unsupported state transition"};
  }
      },
      state_);
      }

      std::string report_current_state() {
        return std::visit(
            overload{[](const state::Available &state) -> std::string {
                       return std::to_string(state.count) + " items available";
                     },
                     [](const state::Depleted) -> std::string {
                       return "Item is temporarily out of stock";
                     },
                     [](const state::Discontinued) -> std::string {
                       return "Item has been discontinued";
                     }},
            state_);
      }

     private:
      State state_;
      }
      ;

      int main() {
        auto fsm = ItemStateMachine{};
        std::cout << fsm.report_current_state() << '\n';
        fsm.process_event(event::DeliveryArrived{3});
        std::cout << fsm.report_current_state() << '\n';
        fsm.process_event(event::Purchased{2});
        std::cout << fsm.report_current_state() << '\n';
        fsm.process_event(event::DeliveryArrived{2});
        std::cout << fsm.report_current_state() << '\n';
        fsm.process_event(event::Purchased{3});
        std::cout << fsm.report_current_state() << '\n';
        fsm.process_event(event::Discontinued{});
        std::cout << fsm.report_current_state() << '\n';
        // fsm.process_event(event::DeliveryArrived{1});
      }
