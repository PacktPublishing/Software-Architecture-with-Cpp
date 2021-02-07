#include "merchants/visited_merchant_history.h"

std::size_t history_of_visited_merchants::add(
    std::unique_ptr<i_visited_merchant> merchant) {
  merchants_.push_back(std::move(merchant));
  return merchants_.size() - 1;
}

void history_of_visited_merchants::rate(std::size_t merchant_index,
                                        stars new_rating) {
  auto &merchant = *merchants_[merchant_index];
  if (merchant.get_rating() != new_rating) {
    merchant.post_rating(new_rating);
  }
}

const i_visited_merchant &history_of_visited_merchants::get_merchant(
    std::size_t merchant_index) {
  return *merchants_[merchant_index];
}
