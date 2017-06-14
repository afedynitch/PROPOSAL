#include "gtest/gtest.h"
#include "PROPOSAL/Medium.h"
#include <iostream>

using namespace PROPOSAL;

TEST(Comparison , Comparison_equal ) {
    Medium A;
    Medium B;
    EXPECT_TRUE(A==B);
    Medium* C = new Medium(MediumType::AntaresWater,0.3);
    Medium* D = new Medium(MediumType::AntaresWater,0.3);
    EXPECT_TRUE(*C==*D);
    Medium* E = new Medium(MediumType::Water,1.);
    EXPECT_TRUE(A==*E);

}

TEST(Comparison , Comparison_not_equal ) {
    Medium A;
    Medium B(MediumType::AntaresWater,0.3);
    EXPECT_TRUE(A!=B);
    Medium* C = new Medium(MediumType::AntaresWater,0.3);
    Medium* D = new Medium(MediumType::Ice,0.3);
    EXPECT_TRUE(*C!=*D);
    Medium* E = new Medium(MediumType::Water,0.3);
    Medium* F = new Medium(MediumType::Water,1);
    EXPECT_TRUE(*E!=*F);

}

TEST(Assignment , Copyconstructor ) {
    Medium A;
    Medium B =A;

    EXPECT_TRUE(A==B);

}

TEST(Assignment , Copyconstructor2 ) {
    Medium A(MediumType::Uranium,1.3);
    Medium B(A);

    EXPECT_TRUE(A==B);

}

TEST(Assignment , Operator ) {
    Medium A;
    Medium B(MediumType::Ice,0.4);

    EXPECT_TRUE(A!=B);

    B=A;

    EXPECT_TRUE(A==B);
}

TEST(Assignment , Swap ) {
    Medium A;
    Medium B;
    EXPECT_TRUE(A==B);
    Medium* C = new Medium(MediumType::AntaresWater,0.3);
    Medium* D = new Medium(MediumType::AntaresWater,0.3);
    EXPECT_TRUE(*C==*D);
    Medium* E = new Medium(MediumType::Water,1.);
    EXPECT_TRUE(A==*E);

    A.swap(*C);
    EXPECT_TRUE(A==*D);
    EXPECT_TRUE(B==*C);
    A.swap(*C);
    C->swap(*E);
    EXPECT_TRUE(A==*C);


}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}