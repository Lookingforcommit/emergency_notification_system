#include "hello.hpp"

#include <userver/utest/utest.hpp>

UTEST(SayHelloTo, Basic) {
  using emergency_notification_system::SayHelloTo;
  using emergency_notification_system::UserType;

  EXPECT_EQ(SayHelloTo("Developer", UserType::kFirstTime),
            "Hello, Developer!\n");
  EXPECT_EQ(SayHelloTo({}, UserType::kFirstTime), "Hello, unknown user!\n");

  EXPECT_EQ(SayHelloTo("Developer", UserType::kKnown),
            "Hi again, Developer!\n");
}
