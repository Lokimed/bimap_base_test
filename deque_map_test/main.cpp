#include <iostream>

#include "gtest/gtest.h"

#include "deque_map.h"

#include <map>
#include <unordered_map>

constexpr uint32_t max_size = 1000;
static_assert(max_size % 2 == 0, "bad max size");

namespace alias {
template<typename T, typename V> using w_map = std::map<T, V>;
template<typename T, typename V> using w_multimap = std::multimap<T, V>;
template<typename T, typename V> using w_unordered_map = std::unordered_map<T, V>;
template<typename T, typename V> using w_unordered_multimap = std::unordered_multimap<T, V>;

template<typename T, typename V> using map = container::wrapper::map<w_map<T, V>>;
template<typename T, typename V> using multimap = container::wrapper::multimap<w_multimap<T, V>>;
template<typename T, typename V> using unordered_map = container::wrapper::map<w_unordered_map<T, V>>;
template<typename T, typename V> using unordered_multimap = container::wrapper::multimap<w_unordered_multimap<T, V>>;
}

using DMap  = container::deque_map<uint32_t, std::string, alias::map>;
using DMMap = container::deque_map<uint32_t, std::string, alias::multimap>;
using DUMap   = container::deque_map<uint32_t, std::string, alias::unordered_map>;
using DUMMap  = container::deque_map<uint32_t, std::string, alias::unordered_multimap>;

#ifdef SET_TEST_CREATOR
#error SET_TEST_CREATOR already defined
#endif
#define SET_TEST_CREATOR(test_name, foo_name)\
TEST(deque_map, foo_name##_DMap)\
{\
    foo_name <DMap>();\
}\
TEST(deque_map, foo_name##_DMMap)\
{\
    foo_name <DMMap>();\
}\
TEST(deque_map, foo_name##_DUMap)\
{\
    foo_name <DUMap>();\
}\
TEST(deque_map, foo_name##_DUMMap)\
{\
    foo_name <DUMMap>();\
}

template<typename T>
void insertContainer()
{
    T container(max_size);
    EXPECT_TRUE(container.isEmpty());

    for(auto ind = 0u; ind < max_size * 2; ++ind) {
        container.insert(ind, std::to_string(ind));
    }

    EXPECT_EQ(container.size(), max_size);
}
SET_TEST_CREATOR(add_values, insertContainer)

template<typename T>
void emplceContainer()
{
    T container(max_size);
    EXPECT_TRUE(container.isEmpty());

    for(auto ind = 0u; ind < max_size * 2; ++ind) {
        auto val = std::to_string(ind);
        container.emplace(std::move(ind), std::move(val));
    }

    EXPECT_EQ(container.size(), max_size);
}
SET_TEST_CREATOR(add_values, emplceContainer)

template<typename T>
void overfill()
{
    std::vector<uint32_t> data;
    data.reserve(max_size);

    T container(max_size);
    EXPECT_TRUE(container.isEmpty());

    //max_size --- (max_size * 2) - 1
    for(auto ind = max_size; ind < max_size * 2; ++ind) {
        container.insert(ind, std::to_string(ind));
        data.emplace_back(ind);
    }
    EXPECT_EQ(container.size(), max_size);

    for(const auto& val : data) {
        const auto range = container.keyGet(val);
        EXPECT_TRUE(range.first != container.cend());
    }

    //0 --- max_size - 1
    for(auto ind = 0u; ind < max_size; ++ind) {
        container.insert(ind, std::to_string(ind));
        EXPECT_EQ(container.size(), max_size);
    }

    EXPECT_EQ(data.size(), max_size);
    for(const auto& val : data) {
        const auto range = container.keyGet(val);
        EXPECT_TRUE(range.first == container.cend());
    }
    EXPECT_EQ(data.size(), max_size);
}
SET_TEST_CREATOR(overfill, overfill)

template<typename T>
void pop()
{
    std::vector<std::string> values;
    values.reserve(max_size);

    std::vector<uint32_t> keys;
    keys.reserve(max_size);

    T container(max_size);
    EXPECT_TRUE(container.isEmpty());

    for(auto ind = 0u; ind < max_size; ++ind) {
        container.insert(ind, std::to_string(ind));
    }
    EXPECT_EQ(container.size(), max_size);

    for(auto ind = max_size; ind < max_size * 2; ++ind) {
        values.emplace_back(std::to_string(ind));
        keys.emplace_back(ind);

        container.insert(ind, std::to_string(ind));
        EXPECT_EQ(container.size(), max_size);
    }

    // max_size --- (max_size * 2) - 1
    for(const auto& k : keys) {
        const auto range = container.keyGet(k);
        EXPECT_EQ(range.first->first, k);
    }

    EXPECT_EQ(container.size(), max_size);
    size_t ind = 0;
    while(true) {
        try {
            const auto value = container.pop();
            const auto check_value = values[ind++];
            EXPECT_EQ(check_value, value);
        } catch(const std::out_of_range&) {
            break;
        }
    }
}
SET_TEST_CREATOR(pop, pop)

template<typename T>
void erase()
{
    T container(max_size);
    EXPECT_TRUE(container.isEmpty());

    std::vector<std::string> val_stay;
    val_stay.reserve(max_size);

    std::vector<uint32_t> keys_delete;
    keys_delete.reserve(max_size);

    for(auto ind = 0u; ind < max_size; ++ind) {
        container.insert(ind, std::to_string(ind));
        EXPECT_EQ(container.size(), ind + 1);

        if(ind % 2 == 0) {
            val_stay.emplace_back(std::to_string(ind));
            continue;
        }
        keys_delete.emplace_back(ind);
    }
    EXPECT_EQ(container.size(), max_size);

    for(const auto& k : keys_delete) {
        const auto range = container.keyGet(k);
        container.erase(range.first);
    }
    EXPECT_EQ(container.size(), max_size / 2);

    for(const auto& v : val_stay) {
        const auto value = container.pop();
        EXPECT_EQ(v, value);
    }
}
SET_TEST_CREATOR(erase, erase)

template<typename T>
void multi_test()
{
    std::vector<std::string> values;
    values.reserve(max_size);

    T container(max_size*2);
    EXPECT_TRUE(container.isEmpty());

    for(auto ind = 0u; ind < max_size; ++ind) {
        container.insert(ind, std::to_string(ind));
        values.emplace_back(std::to_string(ind));
    }
    EXPECT_EQ(container.size(), max_size);

    for(auto ind = 0u; ind < max_size; ++ind) {
        container.insert(ind, std::to_string(ind));
        values.emplace_back(std::to_string(ind));
    }
    EXPECT_EQ(container.size(), max_size * 2);

    for(const auto& v : values) {
        const auto value = container.pop();
        EXPECT_EQ(v, value);
    }
    EXPECT_EQ(container.size(), 0);
    EXPECT_TRUE(container.isEmpty());
}
TEST(deque_map, multi_test_DMMap)
{
    multi_test<DMMap>();
}
TEST(deque_map, multi_test_DUMMap)
{
    multi_test<DUMMap>();
}

template<typename T>
void signgle_test()
{
    std::vector<std::string> values;
    values.reserve(max_size);

    T container(max_size*2);
    EXPECT_TRUE(container.isEmpty());

    for(auto ind = 0u; ind < max_size; ++ind) {
        container.insert(ind, std::to_string(ind));
        values.emplace_back(std::to_string(ind));
    }
    EXPECT_EQ(container.size(), max_size);

    for(auto ind = 0u; ind < max_size; ++ind) {
        container.insert(ind, std::to_string(ind));
    }
    EXPECT_EQ(container.size(), max_size);

    for(const auto& v : values) {
        const auto value = container.pop();
        EXPECT_EQ(v, value);
    }
    EXPECT_EQ(container.size(), 0);
    EXPECT_TRUE(container.isEmpty());
}
TEST(deque_map, signgle_test_DMap)
{
    signgle_test<DMap>();
}
TEST(deque_map, signgle_test_DUMap)
{
    signgle_test<DUMap>();
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
