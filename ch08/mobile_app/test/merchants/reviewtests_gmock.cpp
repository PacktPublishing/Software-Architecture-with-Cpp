#include <gmock/gmock.h>
#include <merchants/visited_merchant_history.h>

#include "fake_customer_review_store.h"
#include "merchants/reviews.h"

namespace {

class mock_visited_merchant : public i_visited_merchant {
 public:
  explicit mock_visited_merchant(fake_customer_review_store &store,
                                 merchant_id_t id)
      : review_store_{store},
        review_{store.get_review_for_merchant(id).value()} {
    ON_CALL(*this, post_rating).WillByDefault([this](stars s) {
      review_.rating = s;
      review_store_.post_review(review_);
    });
    ON_CALL(*this, get_rating).WillByDefault([this] { return review_.rating; });
  }

  MOCK_METHOD(stars, get_rating, (), (override));
  MOCK_METHOD(void, post_rating, (stars s), (override));

 private:
  fake_customer_review_store &review_store_;
  review review_;
};

}  // namespace

class history_with_one_rated_merchant : public ::testing::Test {
 public:
  static constexpr std::size_t CUSTOMER_ID = 7777;
  static constexpr std::size_t MERCHANT_ID = 1234;
  static constexpr const char *REVIEW_TEXT = "Very nice!";
  static constexpr stars RATING = stars{5.f};

 protected:
  void SetUp() final {
    fake_review_store_.post_review(
        {CUSTOMER_ID, MERCHANT_ID, REVIEW_TEXT, RATING});

    // nice mock will not warn on "uninteresting" call to get_rating
    auto mocked_merchant =
        std::make_unique<::testing::NiceMock<mock_visited_merchant>>(
            fake_review_store_, MERCHANT_ID);

    merchant_index_ = history_.add(std::move(mocked_merchant));
  }

  fake_customer_review_store fake_review_store_{CUSTOMER_ID};
  history_of_visited_merchants history_{};
  std::size_t merchant_index_{};
};

TEST_F(history_with_one_rated_merchant,
       when_user_changes_rating_then_the_review_is_updated_in_store) {
  const auto &mocked_merchant = dynamic_cast<const mock_visited_merchant &>(
      history_.get_merchant(merchant_index_));
  EXPECT_CALL(mocked_merchant, post_rating);

  constexpr auto new_rating = stars{4};
  static_assert(RATING != new_rating);
  history_.rate(merchant_index_, stars{new_rating});
}

TEST_F(history_with_one_rated_merchant,
       when_user_selects_same_rating_then_the_review_is_not_updated_in_store) {
  const auto &mocked_merchant = dynamic_cast<const mock_visited_merchant &>(
      history_.get_merchant(merchant_index_));
  EXPECT_CALL(mocked_merchant, post_rating).Times(0);

  history_.rate(merchant_index_, stars{RATING});
}
