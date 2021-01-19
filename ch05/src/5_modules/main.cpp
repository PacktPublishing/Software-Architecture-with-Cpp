#include <algorithm>
#include <chrono>
#include <iostream>
#include <optional>
#include <ostream>
#include <ranges>
#include <vector>

// import merchant; // not necessary -- exported by store
import customer;
import store;

using namespace std::chrono;
using namespace std::literals::chrono_literals;
using namespace std::ranges;

using Stores = std::vector<const Store *>;

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

using Items = std::vector<const Item *>;

auto get_featured_items_for_store(const Store &store) {
  return store.items | views::filter(&Item::featured) |
         views::transform([](const auto &item) { return &item; });
}

range auto get_all_featured_items(const Stores &stores) -> Items {
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
requires std::is_same_v<typename Container::value_type, const Item *> void
render_item_gallery(const Container &items) {
  auto printable_items =
      items |
      views::transform(
          [](auto *item) -> std::add_lvalue_reference_t<decltype(*item)> {
            return *item;
          });
  copy(printable_items, std::ostream_iterator<Item>(std::cout, "\n"));
}

int main() {
  auto fav_stores = get_favorite_stores_for(get_current_customer_id());

  auto selected_items = get_all_featured_items(fav_stores);

  order_items_by_date_added(selected_items);

  render_item_gallery(selected_items);
}
