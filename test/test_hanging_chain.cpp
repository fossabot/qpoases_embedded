/*
 *  This file is part of qpOASES.
 *
 *  qpOASES -- An Implementation of the Online Active Set Strategy.
 *  Copyright (C) 2020 Milan Vukov.
 *
 *  qpOASES is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  qpOASES is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with qpOASES; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301
 *  USA
 *
 */
#include <vector>

#include <gtest/gtest.h>

#include <qpoases_embedded/MessageHandling.hpp>
#include <qpoases_embedded/QProblem.hpp>
#include <qpoases_embedded/Utils.hpp>

#include "./test_helpers.h"

#include "qpoases_embedded/hanging_chain_test_data.h"

namespace qpoases_embedded {

static constexpr real_t kEqualXTolerance = 1.5e-13;  // for primal variables
static constexpr real_t kEqualYTolerance = 1.6e-9;   // for dual variables

class TestHangingChain : public ::testing::TestWithParam<QpTestData> {};

TEST_P(TestHangingChain, ProcessTestData) {
  const auto& test_data = GetParam();
  ASSERT_GT(test_data.num_variables, 0);
  ASSERT_GE(test_data.num_constraints, 0);
  int nWSR = 100;

  QProblem qp(test_data.num_variables, test_data.num_constraints);
  *qp.getMutableH() = test_data.h;
  *qp.getMutableG() = test_data.g;
  *qp.getMutableA() = test_data.a;
  *qp.getMutableLb() = test_data.lb;
  *qp.getMutableUb() = test_data.ub;
  *qp.getMutableLbA() = test_data.lba;
  *qp.getMutableUbA() = test_data.uba;
  const auto success = qp.init(nWSR, printIteration);
  EXPECT_EQ(SUCCESSFUL_RETURN, success) << getErrorString(success);
  EXPECT_STL_VECTORS_NEAR(test_data.x_opt, qp.getX(), kEqualXTolerance);
  EXPECT_STL_VECTORS_NEAR(test_data.y_opt, qp.getY(), kEqualYTolerance);

  if (test_data.num_constraints == 0) {
    nWSR = 100;
    QProblemB qp_b(test_data.num_variables);
    *qp_b.getMutableH() = test_data.h;
    *qp_b.getMutableG() = test_data.g;
    *qp_b.getMutableLb() = test_data.lb;
    *qp_b.getMutableUb() = test_data.ub;
    const auto success = qp_b.init(nWSR, printIterationB);
    EXPECT_EQ(SUCCESSFUL_RETURN, success) << getErrorString(success);
    EXPECT_STL_VECTORS_NEAR(test_data.x_opt, qp_b.getX(), kEqualXTolerance);
    EXPECT_STL_VECTORS_NEAR(test_data.y_opt, qp_b.getY(), kEqualYTolerance);

    EXPECT_STL_VECTORS_EQ(qp.getX(), qp_b.getX());
    EXPECT_STL_VECTORS_NEAR(qp.getY(), qp_b.getY(), 1.5e-13);
  }
}

INSTANTIATE_TEST_SUITE_P(HangingChainTests, TestHangingChain,
                         ::testing::ValuesIn(qp_test_data_vectors));

}  // namespace qpoases_embedded
