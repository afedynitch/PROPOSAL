#include "gtest/gtest.h"
#include "PROPOSAL/Particle.h"
#include <iostream>

TEST(Comparison , Comparison_equal ) {
    Particle A;
    Particle B;
    EXPECT_TRUE(A==B);
    Particle* C = new Particle("tau",1.,1.,1,20,20,1e5,10);
    Particle* D = new Particle("tau",1.,1.,1,20,20,1e5,10);
    C->SetEnergy(1e6);
    D->SetEnergy(1e6);
    EXPECT_TRUE(*C==*D);
    Particle* E = new Particle("mu",0,0,0,0,0,0,0);
    EXPECT_TRUE(A==*E);

}

TEST(Comparison , Comparison_not_equal ) {
    Particle A;
    Particle B("tau",1.,1.,1,20,20,1e5,10);
    EXPECT_TRUE(A!=B);
    Particle* C = new Particle("tau",1.,1.,1,20,20,1e5,10);
    Particle* D = new Particle("tau",1.,1.,1,20,20,1e5,10);
    D->SetEnergy(1e6);
    EXPECT_TRUE(*C!=*D);

}

TEST(Assignment , Copyconstructor ) {
    Particle A;
    Particle B =A;

    EXPECT_TRUE(A==B);

}

TEST(Assignment , Copyconstructor2 ) {
    Particle A("tau",1.,1.,1,20,20,1e5,10);
    Particle B(A);

    EXPECT_TRUE(A==B);

}

TEST(Assignment , Operator ) {
    Particle A;
    Particle B("tau",1.,1.,1,20,20,1e5,10);

    EXPECT_TRUE(A!=B);

    B=A;

    EXPECT_TRUE(A==B);

    A.SetEnergy(1e12);

    EXPECT_TRUE(A!=B);

    B=A;

    EXPECT_TRUE(A==B);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}