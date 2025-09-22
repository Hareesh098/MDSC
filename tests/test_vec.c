#include "mdsc/vec.h"
#include "third_party/unity/unity.h"

static void test_add()
{
  vec3 a=v3(1,2,3), b=v3(4,5,6);
  vec3 c=v_add(a,b);
  TEST_ASSERT_DOUBLE_WITHIN(1e-12, 5.0, c.x);
  TEST_ASSERT_DOUBLE_WITHIN(1e-12, 7.0, c.y);
  TEST_ASSERT_DOUBLE_WITHIN(1e-12, 9.0, c.z);
}

int main(void){ UNITY_BEGIN(); RUN_TEST(test_add); return UNITY_END(); }
