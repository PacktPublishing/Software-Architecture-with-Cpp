#include "merchants/reviews.h"

class CustomerReviewStore : public i_customer_review_store {
 public:
  std::optional<review> get_review_for_merchant(MerchantId merchant_id) final {
    throw std::logic_error{"not implemented"};
  }

  void post_review(review review) final {
    throw std::logic_error{"not implemented"};
  }
};
