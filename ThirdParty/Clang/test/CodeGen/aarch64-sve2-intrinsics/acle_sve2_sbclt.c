// NOTE: Assertions have been autogenerated by utils/update_cc_test_checks.py
// REQUIRES: aarch64-registered-target

// RUN: %clang_cc1 -triple aarch64-none-linux-gnu -target-feature +sve2 -fallow-half-arguments-and-returns -S -O1 -Werror -Wall -emit-llvm -o - %s | FileCheck %s
// RUN: %clang_cc1 -triple aarch64-none-linux-gnu -target-feature +sve2 -fallow-half-arguments-and-returns -S -O1 -Werror -Wall -emit-llvm -o - -x c++ %s | FileCheck %s -check-prefix=CPP-CHECK
// RUN: %clang_cc1 -DSVE_OVERLOADED_FORMS -triple aarch64-none-linux-gnu -target-feature +sve2 -fallow-half-arguments-and-returns -S -O1 -Werror -Wall -emit-llvm -o - %s | FileCheck %s
// RUN: %clang_cc1 -DSVE_OVERLOADED_FORMS -triple aarch64-none-linux-gnu -target-feature +sve2 -fallow-half-arguments-and-returns -S -O1 -Werror -Wall -emit-llvm -o - -x c++ %s | FileCheck %s -check-prefix=CPP-CHECK
// RUN: %clang_cc1 -triple aarch64-none-linux-gnu -target-feature +sve -fallow-half-arguments-and-returns -fsyntax-only -verify -verify-ignore-unexpected=error %s
// RUN: %clang_cc1 -DSVE_OVERLOADED_FORMS -triple aarch64-none-linux-gnu -target-feature +sve -fallow-half-arguments-and-returns -fsyntax-only -verify=overload -verify-ignore-unexpected=error %s

#include <arm_sve.h>

#ifdef SVE_OVERLOADED_FORMS
// A simple used,unused... macro, long enough to represent any SVE builtin.
#define SVE_ACLE_FUNC(A1,A2_UNUSED,A3,A4_UNUSED) A1##A3
#else
#define SVE_ACLE_FUNC(A1,A2,A3,A4) A1##A2##A3##A4
#endif

// CHECK-LABEL: @test_svsbclt_u32(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[TMP0:%.*]] = call <vscale x 4 x i32> @llvm.aarch64.sve.sbclt.nxv4i32(<vscale x 4 x i32> [[OP1:%.*]], <vscale x 4 x i32> [[OP2:%.*]], <vscale x 4 x i32> [[OP3:%.*]])
// CHECK-NEXT:    ret <vscale x 4 x i32> [[TMP0]]
//
// CPP-CHECK-LABEL: @_Z16test_svsbclt_u32u12__SVUint32_tu12__SVUint32_tu12__SVUint32_t(
// CPP-CHECK-NEXT:  entry:
// CPP-CHECK-NEXT:    [[TMP0:%.*]] = call <vscale x 4 x i32> @llvm.aarch64.sve.sbclt.nxv4i32(<vscale x 4 x i32> [[OP1:%.*]], <vscale x 4 x i32> [[OP2:%.*]], <vscale x 4 x i32> [[OP3:%.*]])
// CPP-CHECK-NEXT:    ret <vscale x 4 x i32> [[TMP0]]
//
svuint32_t test_svsbclt_u32(svuint32_t op1, svuint32_t op2, svuint32_t op3)
{
  // overload-warning@+2 {{implicit declaration of function 'svsbclt'}}
  // expected-warning@+1 {{implicit declaration of function 'svsbclt_u32'}}
  return SVE_ACLE_FUNC(svsbclt,_u32,,)(op1, op2, op3);
}

// CHECK-LABEL: @test_svsbclt_u64(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[TMP0:%.*]] = call <vscale x 2 x i64> @llvm.aarch64.sve.sbclt.nxv2i64(<vscale x 2 x i64> [[OP1:%.*]], <vscale x 2 x i64> [[OP2:%.*]], <vscale x 2 x i64> [[OP3:%.*]])
// CHECK-NEXT:    ret <vscale x 2 x i64> [[TMP0]]
//
// CPP-CHECK-LABEL: @_Z16test_svsbclt_u64u12__SVUint64_tu12__SVUint64_tu12__SVUint64_t(
// CPP-CHECK-NEXT:  entry:
// CPP-CHECK-NEXT:    [[TMP0:%.*]] = call <vscale x 2 x i64> @llvm.aarch64.sve.sbclt.nxv2i64(<vscale x 2 x i64> [[OP1:%.*]], <vscale x 2 x i64> [[OP2:%.*]], <vscale x 2 x i64> [[OP3:%.*]])
// CPP-CHECK-NEXT:    ret <vscale x 2 x i64> [[TMP0]]
//
svuint64_t test_svsbclt_u64(svuint64_t op1, svuint64_t op2, svuint64_t op3)
{
  // overload-warning@+2 {{implicit declaration of function 'svsbclt'}}
  // expected-warning@+1 {{implicit declaration of function 'svsbclt_u64'}}
  return SVE_ACLE_FUNC(svsbclt,_u64,,)(op1, op2, op3);
}

// CHECK-LABEL: @test_svsbclt_n_u32(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[DOTSPLATINSERT:%.*]] = insertelement <vscale x 4 x i32> poison, i32 [[OP3:%.*]], i64 0
// CHECK-NEXT:    [[TMP0:%.*]] = shufflevector <vscale x 4 x i32> [[DOTSPLATINSERT]], <vscale x 4 x i32> poison, <vscale x 4 x i32> zeroinitializer
// CHECK-NEXT:    [[TMP1:%.*]] = call <vscale x 4 x i32> @llvm.aarch64.sve.sbclt.nxv4i32(<vscale x 4 x i32> [[OP1:%.*]], <vscale x 4 x i32> [[OP2:%.*]], <vscale x 4 x i32> [[TMP0]])
// CHECK-NEXT:    ret <vscale x 4 x i32> [[TMP1]]
//
// CPP-CHECK-LABEL: @_Z18test_svsbclt_n_u32u12__SVUint32_tu12__SVUint32_tj(
// CPP-CHECK-NEXT:  entry:
// CPP-CHECK-NEXT:    [[DOTSPLATINSERT:%.*]] = insertelement <vscale x 4 x i32> poison, i32 [[OP3:%.*]], i64 0
// CPP-CHECK-NEXT:    [[TMP0:%.*]] = shufflevector <vscale x 4 x i32> [[DOTSPLATINSERT]], <vscale x 4 x i32> poison, <vscale x 4 x i32> zeroinitializer
// CPP-CHECK-NEXT:    [[TMP1:%.*]] = call <vscale x 4 x i32> @llvm.aarch64.sve.sbclt.nxv4i32(<vscale x 4 x i32> [[OP1:%.*]], <vscale x 4 x i32> [[OP2:%.*]], <vscale x 4 x i32> [[TMP0]])
// CPP-CHECK-NEXT:    ret <vscale x 4 x i32> [[TMP1]]
//
svuint32_t test_svsbclt_n_u32(svuint32_t op1, svuint32_t op2, uint32_t op3)
{
  // overload-warning@+2 {{implicit declaration of function 'svsbclt'}}
  // expected-warning@+1 {{implicit declaration of function 'svsbclt_n_u32'}}
  return SVE_ACLE_FUNC(svsbclt,_n_u32,,)(op1, op2, op3);
}

// CHECK-LABEL: @test_svsbclt_n_u64(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[DOTSPLATINSERT:%.*]] = insertelement <vscale x 2 x i64> poison, i64 [[OP3:%.*]], i64 0
// CHECK-NEXT:    [[TMP0:%.*]] = shufflevector <vscale x 2 x i64> [[DOTSPLATINSERT]], <vscale x 2 x i64> poison, <vscale x 2 x i32> zeroinitializer
// CHECK-NEXT:    [[TMP1:%.*]] = call <vscale x 2 x i64> @llvm.aarch64.sve.sbclt.nxv2i64(<vscale x 2 x i64> [[OP1:%.*]], <vscale x 2 x i64> [[OP2:%.*]], <vscale x 2 x i64> [[TMP0]])
// CHECK-NEXT:    ret <vscale x 2 x i64> [[TMP1]]
//
// CPP-CHECK-LABEL: @_Z18test_svsbclt_n_u64u12__SVUint64_tu12__SVUint64_tm(
// CPP-CHECK-NEXT:  entry:
// CPP-CHECK-NEXT:    [[DOTSPLATINSERT:%.*]] = insertelement <vscale x 2 x i64> poison, i64 [[OP3:%.*]], i64 0
// CPP-CHECK-NEXT:    [[TMP0:%.*]] = shufflevector <vscale x 2 x i64> [[DOTSPLATINSERT]], <vscale x 2 x i64> poison, <vscale x 2 x i32> zeroinitializer
// CPP-CHECK-NEXT:    [[TMP1:%.*]] = call <vscale x 2 x i64> @llvm.aarch64.sve.sbclt.nxv2i64(<vscale x 2 x i64> [[OP1:%.*]], <vscale x 2 x i64> [[OP2:%.*]], <vscale x 2 x i64> [[TMP0]])
// CPP-CHECK-NEXT:    ret <vscale x 2 x i64> [[TMP1]]
//
svuint64_t test_svsbclt_n_u64(svuint64_t op1, svuint64_t op2, uint64_t op3)
{
  // overload-warning@+2 {{implicit declaration of function 'svsbclt'}}
  // expected-warning@+1 {{implicit declaration of function 'svsbclt_n_u64'}}
  return SVE_ACLE_FUNC(svsbclt,_n_u64,,)(op1, op2, op3);
}
