#include <gtest/gtest.h>

#include "tatami_stats/utils.hpp"

TEST(Utilities, Groups) {
    {
        std::vector<int> groups { 0, 0, 1, 1, 2, 2 };
        EXPECT_EQ(3, tatami_stats::total_groups(groups.data(), groups.size()));
        EXPECT_EQ(0, tatami_stats::total_groups(groups.data(), 0));
    }

    {
        std::vector<int> groups { 5, 3, 2 };
        EXPECT_EQ(6, tatami_stats::total_groups(groups.data(), groups.size()));
    }

    {
        std::vector<int> groups { 0, 0, 0, 1, 1, 2 };
        std::vector<int> expected { 3, 2, 1 };
        auto observed = tatami_stats::tabulate_groups<int, int>(groups.data(), groups.size());
        EXPECT_EQ(expected, observed);
    }

    {
        std::vector<int> groups { 5 };
        std::vector<int> expected { 0, 0, 0, 0, 0, 1 };
        auto observed = tatami_stats::tabulate_groups<int, int>(groups.data(), groups.size());
        EXPECT_EQ(expected, observed);
    }
}

TEST(Utilities, OutputBuffer) {
    {
        std::vector<int> foo(10);
        tatami_stats::LocalOutputBuffer<int> buffer(0, 5, 3, foo.data(), 1);
        EXPECT_EQ(foo[4], 0); // directly uses the buffer.
        EXPECT_EQ(foo[5], 1);
        EXPECT_EQ(foo[6], 1);
        EXPECT_EQ(foo[7], 1);
        EXPECT_EQ(foo[8], 0);

        buffer.data()[0] = 3;
        EXPECT_EQ(foo[5], 3);
        const auto& cbuffer = buffer; // check the const overload.
        EXPECT_EQ(cbuffer.data()[0], 3);

        buffer.transfer();
        EXPECT_EQ(foo[4], 0); // directly uses the buffer.
        EXPECT_EQ(foo[5], 3);
        EXPECT_EQ(foo[6], 1);
        EXPECT_EQ(foo[7], 1);
        EXPECT_EQ(foo[8], 0);
    }

    {
        std::vector<int> foo(10);
        tatami_stats::LocalOutputBuffer<int> buffer(1, 5, 3, foo.data(), 2);

        buffer.data()[0] = 3;
        const auto& cbuffer = buffer; // checking the const overload.
        EXPECT_EQ(cbuffer.data()[0], 3);
        EXPECT_EQ(foo, std::vector<int>(10)); // buffer is not used yet...

        buffer.transfer();
        EXPECT_EQ(foo[4], 0);
        EXPECT_EQ(foo[5], 3);
        EXPECT_EQ(foo[6], 2);
        EXPECT_EQ(foo[7], 2);
        EXPECT_EQ(foo[8], 0);
    }
}
