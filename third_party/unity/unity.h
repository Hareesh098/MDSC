#ifndef UNITY_H
#define UNITY_H
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define TEST_ASSERT_TRUE(x) if(!(x)){ printf("ASSERT TRUE failed at %s:%d\n",__FILE__,__LINE__); exit(1); }
#define TEST_ASSERT_DOUBLE_WITHIN(delta, expected, actual) do{ double d=fabs((expected)-(actual)); if(d>(delta)){ printf("ASSERT WITHIN failed exp=%g act=%g at %s:%d\n",(expected),(actual),__FILE__,__LINE__); exit(1);} }while(0)
#define UNITY_BEGIN() 0
#define UNITY_END() (printf("All tests passed.\n"),0)
#define RUN_TEST(fn) do{ fn(); }while(0)
#endif
