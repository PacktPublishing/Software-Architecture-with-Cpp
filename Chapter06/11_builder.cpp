#include <chrono>
#include <iomanip>
#include <iostream>
#include <optional>
#include <string>

using namespace std::chrono;

struct Item {
  std::string name;
  std::optional<std::string> photo_url;
  std::string description;
  std::optional<float> price;
  time_point<system_clock> date_added{};
  bool featured{};
};

template <typename ConcreteBuilder>
class GenericItemBuilder {
 public:
  explicit GenericItemBuilder(std::string name)
      : item_{.name = std::move(name)} {}

  ConcreteBuilder &&with_description(std::string description) {
    item_.description = std::move(description);
    return static_cast<ConcreteBuilder &&>(*this);
  }

  ConcreteBuilder &&with_photo_url(std::string url) {
    item_.photo_url = std::move(url);
    return static_cast<ConcreteBuilder &&>(*this);
  }

  ConcreteBuilder &&with_price(float price) {
    item_.price = price;
    return static_cast<ConcreteBuilder &&>(*this);
  }

  ConcreteBuilder &&marked_as_featured() {
    item_.featured = true;
    return static_cast<ConcreteBuilder &&>(*this);
  }

  Item build() && {
    item_.date_added = system_clock::now();
    return std::move(item_);
  }

 protected:
  Item item_;
};

class ItemBuilder final : public GenericItemBuilder<ItemBuilder> {
  using GenericItemBuilder<ItemBuilder>::GenericItemBuilder;
};

namespace {
Item fetch_item(std::string_view url) {
  if (url.ends_with("/linen_blouse")) {
    return {.name = "Linen blouse",
            .photo_url = std::string{url} + "/image.png",
            .description = "Flowy and airy, quite unordinary",
            .price = 72};
  }
  return {};
}
}  // namespace

class FetchingItemBuilder final
    : public GenericItemBuilder<FetchingItemBuilder> {
 public:
  explicit FetchingItemBuilder(std::string name)
      : GenericItemBuilder(std::move(name)) {}

  FetchingItemBuilder &&using_data_from(std::string_view url) && {
    item_ = fetch_item(url);
    return std::move(*this);
  }
};

std::ostream &operator<<(std::ostream &os, const Item &item) {
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

int main() {
  auto directly_loaded_item = ItemBuilder{"Pot"}
                                  .with_description("A decent one")
                                  .with_price(100)
                                  .build();
  std::cout << directly_loaded_item << '\n';

  auto fetched_item =
      FetchingItemBuilder{"Linen blouse"}
          .using_data_from("https://example.com/items/linen_blouse")
          .marked_as_featured()
          .build();
  std::cout << fetched_item << '\n';
}
