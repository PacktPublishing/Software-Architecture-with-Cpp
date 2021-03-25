#include "fake_customer_review_store.h"
#include "merchants/reviews.h"

// order is important
#define CATCH_CONFIG_MAIN
#include <merchants/visited_merchant_history.h>

#include <catch2/catch.hpp>
#include <catch2/trompeloeil.hpp>
#include <memory>

using trompeloeil::_;

class mock_visited_merchant : public i_visited_merchant {
 public:
  MAKE_MOCK0(get_rating, stars(), override);
  MAKE_MOCK1(post_rating, void(stars s), override);
};

SCENARIO("merchant history keeps store up to date", "[mobile app]") {
  GIVEN("a history with one rated merchant") {
    static constexpr std::size_t CUSTOMER_ID = 7777;
    static constexpr std::size_t MERCHANT_ID = 1234;
    static constexpr const char *REVIEW_TEXT = "Very nice!";
    static constexpr stars RATING = stars{5.f};

    auto fake_review_store_ = fake_customer_review_store{CUSTOMER_ID};
    fake_review_store_.post_review(
        {CUSTOMER_ID, MERCHANT_ID, REVIEW_TEXT, RATING});

    auto history_ = history_of_visited_merchants{};
    const auto merchant_index_ =
        history_.add(std::make_unique<mock_visited_merchant>());

    auto &mocked_merchant = const_cast<mock_visited_merchant &>(
        dynamic_cast<const mock_visited_merchant &>(
            history_.get_merchant(merchant_index_)));

    auto review_ = review{CUSTOMER_ID, MERCHANT_ID, REVIEW_TEXT, RATING};
    ALLOW_CALL(mocked_merchant, post_rating(_))
        .LR_SIDE_EFFECT(review_.rating = _1;
                        fake_review_store_.post_review(review_););
    ALLOW_CALL(mocked_merchant, get_rating()).LR_RETURN(review_.rating);

    WHEN("a user changes rating") {
      constexpr auto new_rating = stars{4};
      static_assert(RATING != new_rating);

      THEN("the review is updated in store") {
        REQUIRE_CALL(mocked_merchant, post_rating(_));
        history_.rate(merchant_index_, stars{new_rating});
      }
    }

    WHEN("a user selects same rating") {
      THEN("the review is not updated in store") {
        FORBID_CALL(mocked_merchant, post_rating(_));
        history_.rate(merchant_index_, stars{RATING});
      }
    }
  }
}
