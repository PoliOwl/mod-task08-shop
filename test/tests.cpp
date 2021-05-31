// Copyright 2021 GHA Test Team
#include <gtest/gtest.h>
#include "../include/task.h"

TEST(ShopTests, createShop) {
    ASSERT_NO_THROW(shop mall(10, 10, 10));
}
