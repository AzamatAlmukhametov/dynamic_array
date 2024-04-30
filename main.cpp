#include <vector>
#include <exception>

#include <gtest/gtest.h>

#include "DynamicArray.hpp"
#include "utils.hpp"

const size_t size = 1'000;

TEST(DynamicArrayTest, DefaultConstructor) {
    DynamicArray<int> da;
    ASSERT_EQ(0, da.size());
    ASSERT_EQ(nullptr, da.data());
    EXPECT_EQ(0, da.capacity());
}

TEST(DynamicArrayTest, SizeConstructor) {
    DynamicArray<int> da(size);
    ASSERT_EQ(size, da.size());
    ASSERT_LE(size, da.capacity());

    for (size_t p = 0; p < da.size(); ++p) {
        ASSERT_NE(nullptr, da.data() + p);
    }
}

TEST(DynamicArrayTest, SizeConstructorThrow) {  
    static int count = 0;
    struct DefaultConstructorThrow {
        DefaultConstructorThrow() {
            ++count;
            if (count == 100) {
                throw std::runtime_error("DefaultConstructorThrows");
            }
        }
    };

    EXPECT_ANY_THROW(DynamicArray<DefaultConstructorThrow> da(size));
}

TEST(DynamicArrayTest, CopyConstructor) {
    DynamicArray<int> da;
    initializeWithRandNumbers(da, size, 0, size);

    DynamicArray<int> daCopy(da);
    ASSERT_EQ(da.size(), daCopy.size());

    for (size_t p = 0; p < da.size(); ++p) {
        ASSERT_EQ(*(da.data() + p), *(daCopy.data() + p));
    }
}

TEST(DynamicArrayTest, CopyConstructorThrow) {
    static int count = 0;
    struct CopyConstructorThrow {
        CopyConstructorThrow() noexcept {}
        CopyConstructorThrow(const CopyConstructorThrow& obj) {
            ++count;
            if (count == 100) {
                throw std::runtime_error("DefaultConstructorThrows");
            }
        }
    };

    DynamicArray<CopyConstructorThrow> da(size);
    EXPECT_ANY_THROW(DynamicArray<CopyConstructorThrow> daCopy(da));
}

TEST(DynamicArrayTest, MoveConstructor) {
    DynamicArray<int> da;
    initializeWithRandNumbers(da, size, 0, size);

    DynamicArray<int> daCopy(da);
    DynamicArray<int> daMoved(std::move(da));

    ASSERT_EQ(daCopy.size(), daMoved.size());

    for (size_t p = 0; p < daCopy.size(); ++p) {
        ASSERT_EQ(*(daCopy.data() + p), *(daMoved.data() + p));
    }
}

TEST(DynamicArrayTest, InitializerList) {
    const int arr[10] = {1,2,3,4,5,6,7,8,9,10};
    DynamicArray<int> da{1,2,3,4,5,6,7,8,9,10};
    for (size_t p = 0; p < da.size(); ++p) {
        ASSERT_EQ(arr[p], *(da.data() + p));
    }
}

TEST(DynamicArrayTest, InitializerListThrow) {
    static int count = 0;
    struct DefaultConstructorThrow {
        DefaultConstructorThrow(int arg) {
            ++count;
            if (count == 5) {
                throw std::runtime_error("DefaultConstructorThrows");
            }
        }
    };

    EXPECT_ANY_THROW(( [] {
        DynamicArray<DefaultConstructorThrow> da{1,2,3,4,5,6,7,8,9,10};
    }() 
    ));
}

TEST(DynamicArrayTest, EmplaceBack) {
    struct Obj {
        Obj(int num, std::string&& str) : n(num), s(str) {}
        int n;
        std::string s;
    };

    DynamicArray<Obj> da;
    for (size_t i = 0; i < size; ++i) {
        da.emplace_back(i, std::to_string(i));
    }

    ASSERT_EQ(size, da.size());
    for (size_t p = 0; p < da.size(); ++p) {
        ASSERT_EQ(p, (da.data() + p)->n);
        ASSERT_EQ(std::to_string(p), (da.data() + p)->s);
    }
}

TEST(DynamicArrayTest, PushBackPopBack) {
    struct Obj {
        Obj(size_t num, std::string&& str) : n(num), s(str) {}
        size_t n;
        std::string s;
    };

    DynamicArray<Obj> da;
    for (size_t i = 0; i < size; ++i) {
        da.push_back(Obj(i, std::to_string(i)));
    }

    ASSERT_EQ(size, da.size());
    for (size_t p = 0; p < da.size(); ++p) {
        ASSERT_EQ(p, (da.data() + p)->n);
        ASSERT_EQ(std::to_string(p), (da.data() + p)->s);
    }

    for (size_t p = 0; p < size; ++p) {
        ASSERT_EQ(size - p - 1, (da.data() + size - p - 1)->n);
        ASSERT_EQ(std::to_string(size - p - 1), (da.data() + size - p - 1)->s);
        da.pop_back();
    }
    ASSERT_TRUE(da.empty());
}

TEST(DynamicArrayTest, Insert) {
    DynamicArray<int> da;
    initializeWithRandNumbers(da, size, 0, size);

    std::vector<int> sample(da.size());
    for (size_t i = 0; i < da.size(); ++i) {
        sample[i] = da[i];
    }

    size_t inserts = size / 10;
    for (size_t i = 0; i < inserts; ++i) {
        int pos = (da.size() + i) % da.size();
        sample.insert(sample.begin() + pos, i);
        da.insert(da.begin() + pos, i);
    }

    ASSERT_EQ(sample.size(), da.size());
    for (size_t p = 0; p < da.size(); ++p) {
        ASSERT_EQ(*(sample.data() + p), *(da.data() + p));
    }
}

TEST(DynamicArrayTest, Erase) {
    DynamicArray<int> da;
    initializeWithRandNumbers(da, size, 0, size);

    std::vector<int> sample(da.size());
    for (size_t i = 0; i < da.size(); ++i) {
        sample[i] = da[i];
    }

    size_t erases = size / 10;
    for (size_t i = 0; i < erases; ++i) {
        int pos = (da.size() + i) % da.size();
        sample.insert(sample.begin() + pos, i);
        da.insert(da.begin() + pos, i);
    }

    ASSERT_EQ(sample.size(), da.size());
    for (size_t p = 0; p < da.size(); ++p) {
        ASSERT_EQ(*(sample.data() + p), *(da.data() + p));
    }
}

TEST(DynamicArrayTest, ResizeNarrow) {
    DynamicArray<int> da;
    initializeWithRandNumbers(da, size, 0, size);

    std::vector<int> sample(da.size());
    for (size_t i = 0; i < da.size(); ++i) {
        sample[i] = da[i];
    }

    const size_t newSize = size / 10;
    da.resize(newSize);

    ASSERT_EQ(newSize, da.size());
    for (size_t p = 0; p < da.size(); ++p) {
        ASSERT_EQ(*(sample.data() + p), *(da.data() + p));
    }
}

TEST(DynamicArrayTest, ResizeExpand) {
    const size_t initialSize = size / 10;

    std::vector<int> sample(initialSize);
    DynamicArray<int> da(initialSize);

    da.resize(size);
    sample.resize(size);

    ASSERT_EQ(size, da.size());
    for (size_t p = 0; p < da.size(); ++p) {
        ASSERT_EQ(*(sample.data() + p), *(da.data() + p));
    }
}

TEST(DynamicArrayTest, Reserve) {
    const size_t initialSize = size / 10;
    DynamicArray<int> da(initialSize);
    da.reserve(size);
    ASSERT_EQ(initialSize, da.size());
    ASSERT_EQ(size, da.capacity());
}

TEST(DynamicArrayTest, Clear) {
    DynamicArray<int> da(size);
    da.clear();
    ASSERT_EQ(0, da.size());
}

TEST(DynamicArrayTest, Swap) {
    DynamicArray<int> da1, originDa1;
    initializeWithRandNumbers(da1, size, 0, size);
    originDa1 = da1;

    DynamicArray<int> da2, originDa2;
    initializeWithRandNumbers(da2, size, 0, size);
    originDa2 = da2;

    std::swap(da1, da2);

    ASSERT_EQ(da1.size(), originDa2.size());
    for (size_t p = 0; p < da1.size(); ++p) {
        ASSERT_EQ(*(da1.data() + p), *(originDa2.data() + p));
    }

    ASSERT_EQ(da2.size(), originDa1.size());
    for (size_t p = 0; p < da2.size(); ++p) {
        ASSERT_EQ(*(da2.data() + p), *(originDa1.data() + p));
    }
}

TEST(DynamicArrayTest, SpaceShuttleOperator) {
    DynamicArray<int> da {0,1,2,3,4,5,6,7,8,9};
    DynamicArray<int> da1{0,1,2,3,4,5,6,7,8,9};
    DynamicArray<int> da2{0,1,2,3,4,5,6,7,8};
    DynamicArray<int> da3{0,1,2,3,4,5,6,7,8,10};

    ASSERT_EQ(da, da1);
    ASSERT_NE(da, da2);
    ASSERT_LT(da2, da1);
    ASSERT_LT(da1, da3);
    ASSERT_LE(da, da1);
    ASSERT_LE(da1, da3);
    ASSERT_GT(da1, da2);
    ASSERT_GT(da3, da1);
    ASSERT_GE(da1, da);
    ASSERT_GE(da3, da1);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}