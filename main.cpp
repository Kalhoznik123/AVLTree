#include <iostream>
#include "AVLTree.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>





TEST(AVLTree,SizeEqual){

    //arrange
    AVLTree<int> tree;

    //act
    tree.Insert(1);
    tree.Insert(5);
    tree.Insert(6);
    tree.Insert(7);
    tree.Insert(3);
    tree.Insert(100);

    //assert
    ASSERT_EQ(tree.Size(),6);
    //ASSERT_FALSE(ok3);
}

TEST(AVLTree,SizeNotEqual){

    //arrange
    AVLTree<int> tree;

    //act
    tree.Insert(1);
    tree.Insert(5);
    tree.Insert(6);
    tree.Insert(7);
    tree.Insert(3);
    tree.Insert(100);

    //assert
    ASSERT_NE(tree.Size(),4);
    //ASSERT_FALSE(ok3);
}

TEST(AVLTree,EmptyFasle){

    //arrange
    AVLTree<int> tree;

    //act
    tree.Insert(1);
    tree.Insert(5);
    tree.Insert(6);
    tree.Insert(7);
    tree.Insert(3);
    tree.Insert(100);

    //assert
    ASSERT_FALSE(tree.Empty());
    //ASSERT_FALSE(ok3);
}

TEST(AVLTree,EmptyTrue){

    //arrange
    AVLTree<int> tree;

    //act
    //assert
    ASSERT_TRUE(tree.Empty());
    //ASSERT_FALSE(ok3);
}

TEST(AVLTree,Swap){

}

int main(int argc, char* *argv){

    ::testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
