#pragma once

#include <memory>
#include <vector>

#include "reviews.h"

// represents a merchant item on a list of visited merchants
class i_visited_merchant {
 public:
  using merchant_id_t = review::merchant_id_t;

  virtual ~i_visited_merchant() = default;

  virtual stars get_rating() = 0;
  virtual void post_rating(stars s) = 0;
};

class visited_merchant : public i_visited_merchant {
 public:
  explicit visited_merchant(review::merchant_id_t id,
                            i_customer_review_store &review_store)
      : review_store_{review_store},
        review_{review_store_.get_review_for_merchant(id).value()} {}

  stars get_rating() override { return review_.rating; }
  void post_rating(stars s) override {
    review_.rating = s;
    review_store_.post_review(review_);
  }

 private:
  i_customer_review_store &review_store_;
  review review_;
};

class history_of_visited_merchants {
 public:
  std::size_t add(std::unique_ptr<i_visited_merchant> merchant);
  void rate(std::size_t merchant_index, stars new_rating);
  const i_visited_merchant &get_merchant(std::size_t merchant_index);

 private:
  std::vector<std::unique_ptr<i_visited_merchant>> merchants_;
};
