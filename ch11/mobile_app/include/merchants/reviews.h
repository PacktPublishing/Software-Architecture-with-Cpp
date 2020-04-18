#pragma once

#include <cstdint>
#include <optional>
#include <stdexcept>
#include <string>

class stars {
 public:
  explicit constexpr stars(float stars = 0.f) : stars_{stars} {
    check_range(stars);
  }

  constexpr float value() { return stars_; }

  constexpr bool operator==(const stars &rhs) const {
    return stars_ == rhs.stars_;
  }
  constexpr bool operator!=(const stars &rhs) const { return !(rhs == *this); }

 private:
  static constexpr void check_range(float stars) {
    if (stars > 5 || stars < 0)
      throw std::invalid_argument{
          "Only rates between 0 and 5 stars are allowed"};
  }

  float stars_;
};

struct review {
  using customer_id_t = std::uint64_t;
  using merchant_id_t = std::uint64_t;

  customer_id_t customer;
  merchant_id_t merchant;
  std::string review;
  stars rating;
};

class i_customer_review_store {
 public:
  using CustomerId = review::customer_id_t;
  using MerchantId = review::merchant_id_t;

  virtual ~i_customer_review_store() = default;

  virtual std::optional<review> get_review_for_merchant(
      MerchantId merchant_id) = 0;

  virtual void post_review(review review) = 0;
};
