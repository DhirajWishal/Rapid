// NOTE: Assertions have been autogenerated by utils/update_cc_test_checks.py
// RUN: %clang_cc1 -triple aarch64-none-linux-gnu -target-feature +sve2 -fallow-half-arguments-and-returns -S -O1 -Werror -Wall -emit-llvm -o - %s | FileCheck %s
// RUN: %clang_cc1 -triple aarch64-none-linux-gnu -target-feature +sve2 -fallow-half-arguments-and-returns -S -O1 -Werror -Wall -emit-llvm -o - -x c++ %s | FileCheck %s -check-prefix=CPP-CHECK
// RUN: %clang_cc1 -DSVE_OVERLOADED_FORMS -triple aarch64-none-linux-gnu -target-feature +sve2 -fallow-half-arguments-and-returns -S -O1 -Werror -Wall -emit-llvm -o - %s | FileCheck %s
// RUN: %clang_cc1 -DSVE_OVERLOADED_FORMS -triple aarch64-none-linux-gnu -target-feature +sve2 -fallow-half-arguments-and-returns -S -O1 -Werror -Wall -emit-llvm -o - -x c++ %s | FileCheck %s -check-prefix=CPP-CHECK
// RUN: %clang_cc1 -triple aarch64-none-linux-gnu -target-feature +sve -fallow-half-arguments-and-returns -fsyntax-only -verify -verify-ignore-unexpected=error -verify-ignore-unexpected=note %s

// REQUIRES: aarch64-registered-target

#include <arm_sve.h>

#ifdef SVE_OVERLOADED_FORMS
// A simple used,unused... macro, long enough to represent any SVE builtin.
#define SVE_ACLE_FUNC(A1,A2_UNUSED,A3,A4_UNUSED) A1##A3
#else
#define SVE_ACLE_FUNC(A1,A2,A3,A4) A1##A2##A3##A4
#endif

// CHECK-LABEL: @test_svmls_lane_s16(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[TMP0:%.*]] = call <vscale x 8 x i16> @llvm.aarch64.sve.mls.lane.nxv8i16(<vscale x 8 x i16> [[OP1:%.*]], <vscale x 8 x i16> [[OP2:%.*]], <vscale x 8 x i16> [[OP3:%.*]], i32 0)
// CHECK-NEXT:    ret <vscale x 8 x i16> [[TMP0]]
//
// CPP-CHECK-LABEL: @_Z19test_svmls_lane_s16u11__SVInt16_tu11__SVInt16_tu11__SVInt16_t(
// CPP-CHECK-NEXT:  entry:
// CPP-CHECK-NEXT:    [[TMP0:%.*]] = call <vscale x 8 x i16> @llvm.aarch64.sve.mls.lane.nxv8i16(<vscale x 8 x i16> [[OP1:%.*]], <vscale x 8 x i16> [[OP2:%.*]], <vscale x 8 x i16> [[OP3:%.*]], i32 0)
// CPP-CHECK-NEXT:    ret <vscale x 8 x i16> [[TMP0]]
//
svint16_t test_svmls_lane_s16(svint16_t op1, svint16_t op2, svint16_t op3)
{
  // expected-warning@+1 {{implicit declaration of function 'svmls_lane_s16'}}
  return SVE_ACLE_FUNC(svmls_lane,_s16,,)(op1, op2, op3, 0);
}

// CHECK-LABEL: @test_svmls_lane_s16_1(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[TMP0:%.*]] = call <vscale x 8 x i16> @llvm.aarch64.sve.mls.lane.nxv8i16(<vscale x 8 x i16> [[OP1:%.*]], <vscale x 8 x i16> [[OP2:%.*]], <vscale x 8 x i16> [[OP3:%.*]], i32 7)
// CHECK-NEXT:    ret <vscale x 8 x i16> [[TMP0]]
//
// CPP-CHECK-LABEL: @_Z21test_svmls_lane_s16_1u11__SVInt16_tu11__SVInt16_tu11__SVInt16_t(
// CPP-CHECK-NEXT:  entry:
// CPP-CHECK-NEXT:    [[TMP0:%.*]] = call <vscale x 8 x i16> @llvm.aarch64.sve.mls.lane.nxv8i16(<vscale x 8 x i16> [[OP1:%.*]], <vscale x 8 x i16> [[OP2:%.*]], <vscale x 8 x i16> [[OP3:%.*]], i32 7)
// CPP-CHECK-NEXT:    ret <vscale x 8 x i16> [[TMP0]]
//
svint16_t test_svmls_lane_s16_1(svint16_t op1, svint16_t op2, svint16_t op3)
{
  // expected-warning@+1 {{implicit declaration of function 'svmls_lane_s16'}}
  return SVE_ACLE_FUNC(svmls_lane,_s16,,)(op1, op2, op3, 7);
}

// CHECK-LABEL: @test_svmls_lane_s32(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[TMP0:%.*]] = call <vscale x 4 x i32> @llvm.aarch64.sve.mls.lane.nxv4i32(<vscale x 4 x i32> [[OP1:%.*]], <vscale x 4 x i32> [[OP2:%.*]], <vscale x 4 x i32> [[OP3:%.*]], i32 0)
// CHECK-NEXT:    ret <vscale x 4 x i32> [[TMP0]]
//
// CPP-CHECK-LABEL: @_Z19test_svmls_lane_s32u11__SVInt32_tu11__SVInt32_tu11__SVInt32_t(
// CPP-CHECK-NEXT:  entry:
// CPP-CHECK-NEXT:    [[TMP0:%.*]] = call <vscale x 4 x i32> @llvm.aarch64.sve.mls.lane.nxv4i32(<vscale x 4 x i32> [[OP1:%.*]], <vscale x 4 x i32> [[OP2:%.*]], <vscale x 4 x i32> [[OP3:%.*]], i32 0)
// CPP-CHECK-NEXT:    ret <vscale x 4 x i32> [[TMP0]]
//
svint32_t test_svmls_lane_s32(svint32_t op1, svint32_t op2, svint32_t op3)
{
  // expected-warning@+1 {{implicit declaration of function 'svmls_lane_s32'}}
  return SVE_ACLE_FUNC(svmls_lane,_s32,,)(op1, op2, op3, 0);
}

// CHECK-LABEL: @test_svmls_lane_s32_1(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[TMP0:%.*]] = call <vscale x 4 x i32> @llvm.aarch64.sve.mls.lane.nxv4i32(<vscale x 4 x i32> [[OP1:%.*]], <vscale x 4 x i32> [[OP2:%.*]], <vscale x 4 x i32> [[OP3:%.*]], i32 3)
// CHECK-NEXT:    ret <vscale x 4 x i32> [[TMP0]]
//
// CPP-CHECK-LABEL: @_Z21test_svmls_lane_s32_1u11__SVInt32_tu11__SVInt32_tu11__SVInt32_t(
// CPP-CHECK-NEXT:  entry:
// CPP-CHECK-NEXT:    [[TMP0:%.*]] = call <vscale x 4 x i32> @llvm.aarch64.sve.mls.lane.nxv4i32(<vscale x 4 x i32> [[OP1:%.*]], <vscale x 4 x i32> [[OP2:%.*]], <vscale x 4 x i32> [[OP3:%.*]], i32 3)
// CPP-CHECK-NEXT:    ret <vscale x 4 x i32> [[TMP0]]
//
svint32_t test_svmls_lane_s32_1(svint32_t op1, svint32_t op2, svint32_t op3)
{
  // expected-warning@+1 {{implicit declaration of function 'svmls_lane_s32'}}
  return SVE_ACLE_FUNC(svmls_lane,_s32,,)(op1, op2, op3, 3);
}

// CHECK-LABEL: @test_svmls_lane_s64(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[TMP0:%.*]] = call <vscale x 2 x i64> @llvm.aarch64.sve.mls.lane.nxv2i64(<vscale x 2 x i64> [[OP1:%.*]], <vscale x 2 x i64> [[OP2:%.*]], <vscale x 2 x i64> [[OP3:%.*]], i32 0)
// CHECK-NEXT:    ret <vscale x 2 x i64> [[TMP0]]
//
// CPP-CHECK-LABEL: @_Z19test_svmls_lane_s64u11__SVInt64_tu11__SVInt64_tu11__SVInt64_t(
// CPP-CHECK-NEXT:  entry:
// CPP-CHECK-NEXT:    [[TMP0:%.*]] = call <vscale x 2 x i64> @llvm.aarch64.sve.mls.lane.nxv2i64(<vscale x 2 x i64> [[OP1:%.*]], <vscale x 2 x i64> [[OP2:%.*]], <vscale x 2 x i64> [[OP3:%.*]], i32 0)
// CPP-CHECK-NEXT:    ret <vscale x 2 x i64> [[TMP0]]
//
svint64_t test_svmls_lane_s64(svint64_t op1, svint64_t op2, svint64_t op3)
{
  // expected-warning@+1 {{implicit declaration of function 'svmls_lane_s64'}}
  return SVE_ACLE_FUNC(svmls_lane,_s64,,)(op1, op2, op3, 0);
}

// CHECK-LABEL: @test_svmls_lane_s64_1(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[TMP0:%.*]] = call <vscale x 2 x i64> @llvm.aarch64.sve.mls.lane.nxv2i64(<vscale x 2 x i64> [[OP1:%.*]], <vscale x 2 x i64> [[OP2:%.*]], <vscale x 2 x i64> [[OP3:%.*]], i32 1)
// CHECK-NEXT:    ret <vscale x 2 x i64> [[TMP0]]
//
// CPP-CHECK-LABEL: @_Z21test_svmls_lane_s64_1u11__SVInt64_tu11__SVInt64_tu11__SVInt64_t(
// CPP-CHECK-NEXT:  entry:
// CPP-CHECK-NEXT:    [[TMP0:%.*]] = call <vscale x 2 x i64> @llvm.aarch64.sve.mls.lane.nxv2i64(<vscale x 2 x i64> [[OP1:%.*]], <vscale x 2 x i64> [[OP2:%.*]], <vscale x 2 x i64> [[OP3:%.*]], i32 1)
// CPP-CHECK-NEXT:    ret <vscale x 2 x i64> [[TMP0]]
//
svint64_t test_svmls_lane_s64_1(svint64_t op1, svint64_t op2, svint64_t op3)
{
  // expected-warning@+1 {{implicit declaration of function 'svmls_lane_s64'}}
  return SVE_ACLE_FUNC(svmls_lane,_s64,,)(op1, op2, op3, 1);
}

// CHECK-LABEL: @test_svmls_lane_u16(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[TMP0:%.*]] = call <vscale x 8 x i16> @llvm.aarch64.sve.mls.lane.nxv8i16(<vscale x 8 x i16> [[OP1:%.*]], <vscale x 8 x i16> [[OP2:%.*]], <vscale x 8 x i16> [[OP3:%.*]], i32 0)
// CHECK-NEXT:    ret <vscale x 8 x i16> [[TMP0]]
//
// CPP-CHECK-LABEL: @_Z19test_svmls_lane_u16u12__SVUint16_tu12__SVUint16_tu12__SVUint16_t(
// CPP-CHECK-NEXT:  entry:
// CPP-CHECK-NEXT:    [[TMP0:%.*]] = call <vscale x 8 x i16> @llvm.aarch64.sve.mls.lane.nxv8i16(<vscale x 8 x i16> [[OP1:%.*]], <vscale x 8 x i16> [[OP2:%.*]], <vscale x 8 x i16> [[OP3:%.*]], i32 0)
// CPP-CHECK-NEXT:    ret <vscale x 8 x i16> [[TMP0]]
//
svuint16_t test_svmls_lane_u16(svuint16_t op1, svuint16_t op2, svuint16_t op3)
{
  // expected-warning@+1 {{implicit declaration of function 'svmls_lane_u16'}}
  return SVE_ACLE_FUNC(svmls_lane,_u16,,)(op1, op2, op3, 0);
}

// CHECK-LABEL: @test_svmls_lane_u16_1(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[TMP0:%.*]] = call <vscale x 8 x i16> @llvm.aarch64.sve.mls.lane.nxv8i16(<vscale x 8 x i16> [[OP1:%.*]], <vscale x 8 x i16> [[OP2:%.*]], <vscale x 8 x i16> [[OP3:%.*]], i32 7)
// CHECK-NEXT:    ret <vscale x 8 x i16> [[TMP0]]
//
// CPP-CHECK-LABEL: @_Z21test_svmls_lane_u16_1u12__SVUint16_tu12__SVUint16_tu12__SVUint16_t(
// CPP-CHECK-NEXT:  entry:
// CPP-CHECK-NEXT:    [[TMP0:%.*]] = call <vscale x 8 x i16> @llvm.aarch64.sve.mls.lane.nxv8i16(<vscale x 8 x i16> [[OP1:%.*]], <vscale x 8 x i16> [[OP2:%.*]], <vscale x 8 x i16> [[OP3:%.*]], i32 7)
// CPP-CHECK-NEXT:    ret <vscale x 8 x i16> [[TMP0]]
//
svuint16_t test_svmls_lane_u16_1(svuint16_t op1, svuint16_t op2, svuint16_t op3)
{
  // expected-warning@+1 {{implicit declaration of function 'svmls_lane_u16'}}
  return SVE_ACLE_FUNC(svmls_lane,_u16,,)(op1, op2, op3, 7);
}

// CHECK-LABEL: @test_svmls_lane_u32(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[TMP0:%.*]] = call <vscale x 4 x i32> @llvm.aarch64.sve.mls.lane.nxv4i32(<vscale x 4 x i32> [[OP1:%.*]], <vscale x 4 x i32> [[OP2:%.*]], <vscale x 4 x i32> [[OP3:%.*]], i32 0)
// CHECK-NEXT:    ret <vscale x 4 x i32> [[TMP0]]
//
// CPP-CHECK-LABEL: @_Z19test_svmls_lane_u32u12__SVUint32_tu12__SVUint32_tu12__SVUint32_t(
// CPP-CHECK-NEXT:  entry:
// CPP-CHECK-NEXT:    [[TMP0:%.*]] = call <vscale x 4 x i32> @llvm.aarch64.sve.mls.lane.nxv4i32(<vscale x 4 x i32> [[OP1:%.*]], <vscale x 4 x i32> [[OP2:%.*]], <vscale x 4 x i32> [[OP3:%.*]], i32 0)
// CPP-CHECK-NEXT:    ret <vscale x 4 x i32> [[TMP0]]
//
svuint32_t test_svmls_lane_u32(svuint32_t op1, svuint32_t op2, svuint32_t op3)
{
  // expected-warning@+1 {{implicit declaration of function 'svmls_lane_u32'}}
  return SVE_ACLE_FUNC(svmls_lane,_u32,,)(op1, op2, op3, 0);
}

// CHECK-LABEL: @test_svmls_lane_u32_1(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[TMP0:%.*]] = call <vscale x 4 x i32> @llvm.aarch64.sve.mls.lane.nxv4i32(<vscale x 4 x i32> [[OP1:%.*]], <vscale x 4 x i32> [[OP2:%.*]], <vscale x 4 x i32> [[OP3:%.*]], i32 3)
// CHECK-NEXT:    ret <vscale x 4 x i32> [[TMP0]]
//
// CPP-CHECK-LABEL: @_Z21test_svmls_lane_u32_1u12__SVUint32_tu12__SVUint32_tu12__SVUint32_t(
// CPP-CHECK-NEXT:  entry:
// CPP-CHECK-NEXT:    [[TMP0:%.*]] = call <vscale x 4 x i32> @llvm.aarch64.sve.mls.lane.nxv4i32(<vscale x 4 x i32> [[OP1:%.*]], <vscale x 4 x i32> [[OP2:%.*]], <vscale x 4 x i32> [[OP3:%.*]], i32 3)
// CPP-CHECK-NEXT:    ret <vscale x 4 x i32> [[TMP0]]
//
svuint32_t test_svmls_lane_u32_1(svuint32_t op1, svuint32_t op2, svuint32_t op3)
{
  // expected-warning@+1 {{implicit declaration of function 'svmls_lane_u32'}}
  return SVE_ACLE_FUNC(svmls_lane,_u32,,)(op1, op2, op3, 3);
}

// CHECK-LABEL: @test_svmls_lane_u64(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[TMP0:%.*]] = call <vscale x 2 x i64> @llvm.aarch64.sve.mls.lane.nxv2i64(<vscale x 2 x i64> [[OP1:%.*]], <vscale x 2 x i64> [[OP2:%.*]], <vscale x 2 x i64> [[OP3:%.*]], i32 0)
// CHECK-NEXT:    ret <vscale x 2 x i64> [[TMP0]]
//
// CPP-CHECK-LABEL: @_Z19test_svmls_lane_u64u12__SVUint64_tu12__SVUint64_tu12__SVUint64_t(
// CPP-CHECK-NEXT:  entry:
// CPP-CHECK-NEXT:    [[TMP0:%.*]] = call <vscale x 2 x i64> @llvm.aarch64.sve.mls.lane.nxv2i64(<vscale x 2 x i64> [[OP1:%.*]], <vscale x 2 x i64> [[OP2:%.*]], <vscale x 2 x i64> [[OP3:%.*]], i32 0)
// CPP-CHECK-NEXT:    ret <vscale x 2 x i64> [[TMP0]]
//
svuint64_t test_svmls_lane_u64(svuint64_t op1, svuint64_t op2, svuint64_t op3)
{
  // expected-warning@+1 {{implicit declaration of function 'svmls_lane_u64'}}
  return SVE_ACLE_FUNC(svmls_lane,_u64,,)(op1, op2, op3, 0);
}

// CHECK-LABEL: @test_svmls_lane_u64_1(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[TMP0:%.*]] = call <vscale x 2 x i64> @llvm.aarch64.sve.mls.lane.nxv2i64(<vscale x 2 x i64> [[OP1:%.*]], <vscale x 2 x i64> [[OP2:%.*]], <vscale x 2 x i64> [[OP3:%.*]], i32 1)
// CHECK-NEXT:    ret <vscale x 2 x i64> [[TMP0]]
//
// CPP-CHECK-LABEL: @_Z21test_svmls_lane_u64_1u12__SVUint64_tu12__SVUint64_tu12__SVUint64_t(
// CPP-CHECK-NEXT:  entry:
// CPP-CHECK-NEXT:    [[TMP0:%.*]] = call <vscale x 2 x i64> @llvm.aarch64.sve.mls.lane.nxv2i64(<vscale x 2 x i64> [[OP1:%.*]], <vscale x 2 x i64> [[OP2:%.*]], <vscale x 2 x i64> [[OP3:%.*]], i32 1)
// CPP-CHECK-NEXT:    ret <vscale x 2 x i64> [[TMP0]]
//
svuint64_t test_svmls_lane_u64_1(svuint64_t op1, svuint64_t op2, svuint64_t op3)
{
  // expected-warning@+1 {{implicit declaration of function 'svmls_lane_u64'}}
  return SVE_ACLE_FUNC(svmls_lane,_u64,,)(op1, op2, op3, 1);
}
