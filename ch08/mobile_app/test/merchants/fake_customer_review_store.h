#pragma once

#include <optional>
#include <unordered_map>

#include "merchants/reviews.h"

class fake_customer_review_store : public i_customer_review_store {
 public:
  explicit fake_customer_review_store(review::customer_id_t customer_id)
      : customer_id_(customer_id) {}

  std::optional<review> get_review_for_merchant(
      review::merchant_id_t merchant_id) final {
    if (auto it = reviews_.find(merchant_id); it != std::end(reviews_)) {
      return it->second;
    }
    return {};
  }

  void post_review(review r) final {
    if (r.customer != customer_id_) {
      throw std::invalid_argument{
          "Trying to post a review under a different customer ID"};
    }
    reviews_[r.merchant] = std::move(r);
  }

 private:
  review::customer_id_t customer_id_;
  std::unordered_map<review::merchant_id_t, review> reviews_;
};
