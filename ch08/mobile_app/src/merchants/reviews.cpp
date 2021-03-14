#include "merchants/reviews.h"

#include <gsl/gsl>
#include <ranges>

template <typename T>
constexpr bool is_range_of_reviews_v =
    std::is_same_v<std::ranges::range_value_t<T>, review>;

class CustomerReviewStore : public i_customer_review_store {
 public:
  CustomerReviewStore() = default;
  explicit CustomerReviewStore(const std::ranges::range auto &initial_reviews) {
    static_assert(is_range_of_reviews_v<decltype(initial_reviews)>,
                  "Must pass in a collection of reviews");
    std::ranges::copy(begin(initial_reviews), end(initial_reviews),
                      begin(reviews_));
  }

  std::optional<review> get_review_for_merchant(MerchantId merchant_id) final {
    throw std::logic_error{"not implemented"};
  }

  void post_review(review review) final {
    Expects(review.merchant);
    Expects(review.customer);
    Ensures(!reviews_.empty());

    reviews_.push_back(std::move(review));
  }

 private:
  std::vector<review> reviews_;
};

// void fails_to_compile() {
//  CustomerReviewStore(std::vector<int>{})
//      .post_review(review{1, 2, "3", stars{4.5f}});
//  // error: static assertion failed: Must pass in a collection of reviews
//}
