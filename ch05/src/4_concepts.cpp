#include <algorithm>
#include <chrono>
#include <gsl/pointers>
#include <iomanip>
#include <iostream>
#include <optional>
#include <ostream>
#include <ranges>
#include <vector>

using namespace std::chrono;
using namespace std::literals::chrono_literals;
using namespace std::ranges;

using CustomerId = int;

CustomerId get_current_customer_id() { return 42; }

struct Merchant {
  int id;
};

struct Item {
  std::string name;
  std::optional<std::string> photo_url;
  std::string description;
  std::optional<float> price;
  time_point<system_clock> date_added{};
  bool featured{};
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

enum class Category {
  Food,
  Antiques,
  Books,
  Music,
  Photography,
  Handicraft,
  Artist,
};

struct Store {
  gsl::not_null<const Merchant *> owner;
  std::vector<Item> items;
  std::vector<Category> categories;
};

using Stores = std::vector<gsl::not_null<const Store *>>;

Stores get_favorite_stores_for(const CustomerId &customer_id) {
  static const auto merchants = std::vector<Merchant>{{17}, {29}};
  static const auto stores = std::vector<Store>{
      {.owner = &merchants[0],
       .items =
           {
               {.name = "Honey",
                .photo_url = {},
                .description = "Straight outta Compton's apiary",
                .price = 9.99f,
                .date_added = system_clock::now(),
                .featured = false},
               {.name = "Oscypek",
                .photo_url = {},
                .description = "Tasty smoked cheese from the Tatra mountains",
                .price = 1.23f,
                .date_added = system_clock::now() - 1h,
                .featured = true},
           },
       .categories = {Category::Food}},
      {.owner = &merchants[1],
       .items = {{
           .name = "Handmade painted ceramic bowls",
           .photo_url = "http://example.com/beautiful_bowl.png",
           .description =
               "Hand-crafted and hand-decorated bowls made of fired clay",
           .price = {},
           .date_added = system_clock::now() - 12min,
           .featured = true,
       }},
       .categories = {Category::Artist, Category::Handicraft}}};
  static auto favorite_stores_by_customer =
      std::unordered_map<CustomerId, Stores>{{42, {&stores[0], &stores[1]}}};
  return favorite_stores_by_customer[customer_id];
}

using Items = std::vector<gsl::not_null<const Item *>>;

auto get_featured_items_for_store(const Store &store) {
  return store.items | views::filter(&Item::featured) |
         views::transform(
             [](const auto &item) { return gsl::not_null(&item); });
}

range auto get_all_featured_items(const Stores &stores) {
  auto all_featured = stores | views::transform([](auto elem) {
                        return get_featured_items_for_store(*elem);
                      }) |
                      views::join;
  auto as_items = Items{};
  as_items.reserve(distance(all_featured));
  copy(all_featured, std::back_inserter(as_items));
  return as_items;
}

template <typename Range, typename Comp, typename Proj>
concept sortable_range =
    random_access_range<Range> &&std::sortable<iterator_t<Range>, Comp, Proj>;

void order_items_by_date_added(
    sortable_range<greater, decltype(&Item::date_added)> auto &items) {
  sort(items, greater{}, &Item::date_added);
}

template <input_range Container>
requires std::is_same_v<typename Container::value_type,
                        gsl::not_null<const Item *>> void
render_item_gallery(const Container &items) {
  copy(items,
       std::ostream_iterator<typename Container::value_type>(std::cout, "\n"));
}

int main() {
  auto fav_stores = get_favorite_stores_for(get_current_customer_id());

  auto selected_items = get_all_featured_items(fav_stores);

  order_items_by_date_added(selected_items);

  render_item_gallery(selected_items);
}
