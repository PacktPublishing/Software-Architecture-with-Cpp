module;

#include <chrono>
#include <iomanip>
#include <optional>
#include <string>
#include <vector>

export module store;

export import merchant;

using namespace std::chrono;

export struct Item {
  std::string name;
  std::optional<std::string> photo_url;
  std::string description;
  std::optional<float> price;
  time_point<system_clock> date_added{};
  bool featured{};
};

export std::ostream &operator<<(std::ostream &os, const Item &item) {
  auto stringify_optional = [](const auto &optional) {
    using optional_value_type =
        typename std::remove_cvref_t<decltype(optional)>::value_type;
    if constexpr (std::is_same_v<optional_value_type, std::string>) {
      return optional ? *optional : "missing";
    } else {
      return optional ? std::to_string(*optional) : "missing";
    }
  };

  auto time_added = system_clock::to_time_t(item.date_added);

  os << "name: " << item.name
     << ", photo_url: " << stringify_optional(item.photo_url)
     << ", description: " << item.description
     << ", price: " << std::setprecision(2) << stringify_optional(item.price)
     << ", date_added: " << std::put_time(std::localtime(&time_added), "%c %Z")
     << ", featured: " << item.featured;
  return os;
}

export enum class Category {
  Food,
  Antiques,
  Books,
  Music,
  Photography,
  Handicraft,
  Artist,
};

export struct Store {
  const Merchant *owner;  // gsl::not_null had issues with modules
  std::vector<Item> items;
  std::vector<Category> categories;
};
