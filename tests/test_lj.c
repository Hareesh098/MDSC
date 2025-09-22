#include "mdsc/system.h"
#include "mdsc/force.h"
#include "third_party/unity/unity.h"

static void test_repulsive_force()
{
  system_t s; sys_random_init(&s, 2, 0.001); // big box to avoid PBC issues
  s.p[0].r.x = 0.0; s.p[0].r.y = 0.0; s.p[0].r.z = 0.0;
  s.p[1].r.x = 0.9; s.p[1].r.y = 0.0; s.p[1].r.z = 0.0;
  s.rc = 2.5; s.rc2 = s.rc*s.rc;
  sys_zero_forces(&s);
  force_accum_t acc;
  lj_forces(&s, &acc);
  TEST_ASSERT_TRUE(s.p[0].f.x < 0.0); // force on 0 points to -x (repulsion)
  sys_free(&s);
}

int main(void){ UNITY_BEGIN(); RUN_TEST(test_repulsive_force); return UNITY_END(); }
