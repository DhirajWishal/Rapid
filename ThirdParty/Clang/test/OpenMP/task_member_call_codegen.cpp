// NOTE: Assertions have been autogenerated by utils/update_cc_test_checks.py UTC_ARGS: --function-signature --include-generated-funcs --replace-value-regex "__omp_offloading_[0-9a-z]+_[0-9a-z]+" "reduction_size[.].+[.]" "pl_cond[.].+[.|,]" --prefix-filecheck-ir-name _
// RUN: %clang_cc1 -verify -triple x86_64-apple-darwin10 -fopenmp -x c++ -emit-llvm %s -o - | FileCheck %s --check-prefix=CHECK1
// RUN: %clang_cc1 -fopenmp -x c++ -triple x86_64-apple-darwin10 -emit-pch -o %t %s
// RUN: %clang_cc1 -fopenmp -x c++ -triple x86_64-apple-darwin10 -include-pch %t -verify %s -emit-llvm -o - | FileCheck %s --check-prefix=CHECK2
// RUN: %clang_cc1 -verify -triple x86_64-apple-darwin10 -fopenmp -fopenmp-enable-irbuilder -x c++ -emit-llvm %s -o - | FileCheck %s --check-prefix=CHECK3
// RUN: %clang_cc1 -fopenmp -fopenmp-enable-irbuilder -x c++ -triple x86_64-apple-darwin10 -emit-pch -o %t %s
// RUN: %clang_cc1 -fopenmp -fopenmp-enable-irbuilder -x c++ -triple x86_64-apple-darwin10 -include-pch %t -verify %s -emit-llvm -o - | FileCheck %s --check-prefix=CHECK4

// RUN: %clang_cc1 -verify -triple x86_64-apple-darwin10 -fopenmp-simd -x c++ -emit-llvm %s -o - | FileCheck %s --implicit-check-not="{{__kmpc|__tgt}}"
// RUN: %clang_cc1 -fopenmp-simd -x c++ -triple x86_64-apple-darwin10 -emit-pch -o %t %s
// RUN: %clang_cc1 -fopenmp-simd -x c++ -triple x86_64-apple-darwin10 -include-pch %t -verify %s -emit-llvm -o - | FileCheck %s --implicit-check-not="{{__kmpc|__tgt}}"
// expected-no-diagnostics

#ifndef HEADER
#define HEADER

class a {
public:
  void b();
};
void c() {
  a d;
#pragma omp task
  d.b();
}
#endif
// CHECK1-LABEL: define {{[^@]+}}@_Z1cv
// CHECK1-SAME: () #[[ATTR0:[0-9]+]] {
// CHECK1-NEXT:  entry:
// CHECK1-NEXT:    [[D:%.*]] = alloca [[CLASS_A:%.*]], align 1
// CHECK1-NEXT:    [[AGG_CAPTURED:%.*]] = alloca [[STRUCT_ANON:%.*]], align 1
// CHECK1-NEXT:    [[TMP0:%.*]] = call i32 @__kmpc_global_thread_num(%struct.ident_t* @[[GLOB1:[0-9]+]])
// CHECK1-NEXT:    [[TMP1:%.*]] = call i8* @__kmpc_omp_task_alloc(%struct.ident_t* @[[GLOB1]], i32 [[TMP0]], i32 1, i64 48, i64 1, i32 (i32, i8*)* bitcast (i32 (i32, %struct.kmp_task_t_with_privates*)* @.omp_task_entry. to i32 (i32, i8*)*))
// CHECK1-NEXT:    [[TMP2:%.*]] = bitcast i8* [[TMP1]] to %struct.kmp_task_t_with_privates*
// CHECK1-NEXT:    [[TMP3:%.*]] = getelementptr inbounds [[STRUCT_KMP_TASK_T_WITH_PRIVATES:%.*]], %struct.kmp_task_t_with_privates* [[TMP2]], i32 0, i32 0
// CHECK1-NEXT:    [[TMP4:%.*]] = getelementptr inbounds [[STRUCT_KMP_TASK_T_WITH_PRIVATES]], %struct.kmp_task_t_with_privates* [[TMP2]], i32 0, i32 1
// CHECK1-NEXT:    [[TMP5:%.*]] = getelementptr inbounds [[STRUCT__KMP_PRIVATES_T:%.*]], %struct..kmp_privates.t* [[TMP4]], i32 0, i32 0
// CHECK1-NEXT:    [[TMP6:%.*]] = call i32 @__kmpc_omp_task(%struct.ident_t* @[[GLOB1]], i32 [[TMP0]], i8* [[TMP1]])
// CHECK1-NEXT:    ret void
//
//
// CHECK1-LABEL: define {{[^@]+}}@.omp_task_privates_map.
// CHECK1-SAME: (%struct..kmp_privates.t* noalias noundef [[TMP0:%.*]], %class.a** noalias noundef [[TMP1:%.*]]) #[[ATTR2:[0-9]+]] {
// CHECK1-NEXT:  entry:
// CHECK1-NEXT:    [[DOTADDR:%.*]] = alloca %struct..kmp_privates.t*, align 8
// CHECK1-NEXT:    [[DOTADDR1:%.*]] = alloca %class.a**, align 8
// CHECK1-NEXT:    store %struct..kmp_privates.t* [[TMP0]], %struct..kmp_privates.t** [[DOTADDR]], align 8
// CHECK1-NEXT:    store %class.a** [[TMP1]], %class.a*** [[DOTADDR1]], align 8
// CHECK1-NEXT:    [[TMP2:%.*]] = load %struct..kmp_privates.t*, %struct..kmp_privates.t** [[DOTADDR]], align 8
// CHECK1-NEXT:    [[TMP3:%.*]] = getelementptr inbounds [[STRUCT__KMP_PRIVATES_T:%.*]], %struct..kmp_privates.t* [[TMP2]], i32 0, i32 0
// CHECK1-NEXT:    [[TMP4:%.*]] = load %class.a**, %class.a*** [[DOTADDR1]], align 8
// CHECK1-NEXT:    store %class.a* [[TMP3]], %class.a** [[TMP4]], align 8
// CHECK1-NEXT:    ret void
//
//
// CHECK1-LABEL: define {{[^@]+}}@.omp_task_entry.
// CHECK1-SAME: (i32 noundef [[TMP0:%.*]], %struct.kmp_task_t_with_privates* noalias noundef [[TMP1:%.*]]) #[[ATTR3:[0-9]+]] {
// CHECK1-NEXT:  entry:
// CHECK1-NEXT:    [[DOTGLOBAL_TID__ADDR_I:%.*]] = alloca i32, align 4
// CHECK1-NEXT:    [[DOTPART_ID__ADDR_I:%.*]] = alloca i32*, align 8
// CHECK1-NEXT:    [[DOTPRIVATES__ADDR_I:%.*]] = alloca i8*, align 8
// CHECK1-NEXT:    [[DOTCOPY_FN__ADDR_I:%.*]] = alloca void (i8*, ...)*, align 8
// CHECK1-NEXT:    [[DOTTASK_T__ADDR_I:%.*]] = alloca i8*, align 8
// CHECK1-NEXT:    [[__CONTEXT_ADDR_I:%.*]] = alloca %struct.anon*, align 8
// CHECK1-NEXT:    [[DOTFIRSTPRIV_PTR_ADDR_I:%.*]] = alloca %class.a*, align 8
// CHECK1-NEXT:    [[DOTADDR:%.*]] = alloca i32, align 4
// CHECK1-NEXT:    [[DOTADDR1:%.*]] = alloca %struct.kmp_task_t_with_privates*, align 8
// CHECK1-NEXT:    store i32 [[TMP0]], i32* [[DOTADDR]], align 4
// CHECK1-NEXT:    store %struct.kmp_task_t_with_privates* [[TMP1]], %struct.kmp_task_t_with_privates** [[DOTADDR1]], align 8
// CHECK1-NEXT:    [[TMP2:%.*]] = load i32, i32* [[DOTADDR]], align 4
// CHECK1-NEXT:    [[TMP3:%.*]] = load %struct.kmp_task_t_with_privates*, %struct.kmp_task_t_with_privates** [[DOTADDR1]], align 8
// CHECK1-NEXT:    [[TMP4:%.*]] = getelementptr inbounds [[STRUCT_KMP_TASK_T_WITH_PRIVATES:%.*]], %struct.kmp_task_t_with_privates* [[TMP3]], i32 0, i32 0
// CHECK1-NEXT:    [[TMP5:%.*]] = getelementptr inbounds [[STRUCT_KMP_TASK_T:%.*]], %struct.kmp_task_t* [[TMP4]], i32 0, i32 2
// CHECK1-NEXT:    [[TMP6:%.*]] = getelementptr inbounds [[STRUCT_KMP_TASK_T]], %struct.kmp_task_t* [[TMP4]], i32 0, i32 0
// CHECK1-NEXT:    [[TMP7:%.*]] = load i8*, i8** [[TMP6]], align 8
// CHECK1-NEXT:    [[TMP8:%.*]] = bitcast i8* [[TMP7]] to %struct.anon*
// CHECK1-NEXT:    [[TMP9:%.*]] = getelementptr inbounds [[STRUCT_KMP_TASK_T_WITH_PRIVATES]], %struct.kmp_task_t_with_privates* [[TMP3]], i32 0, i32 1
// CHECK1-NEXT:    [[TMP10:%.*]] = bitcast %struct..kmp_privates.t* [[TMP9]] to i8*
// CHECK1-NEXT:    [[TMP11:%.*]] = bitcast %struct.kmp_task_t_with_privates* [[TMP3]] to i8*
// CHECK1-NEXT:    call void @llvm.experimental.noalias.scope.decl(metadata [[META3:![0-9]+]])
// CHECK1-NEXT:    call void @llvm.experimental.noalias.scope.decl(metadata [[META6:![0-9]+]])
// CHECK1-NEXT:    call void @llvm.experimental.noalias.scope.decl(metadata [[META8:![0-9]+]])
// CHECK1-NEXT:    call void @llvm.experimental.noalias.scope.decl(metadata [[META10:![0-9]+]])
// CHECK1-NEXT:    store i32 [[TMP2]], i32* [[DOTGLOBAL_TID__ADDR_I]], align 4, !noalias !12
// CHECK1-NEXT:    store i32* [[TMP5]], i32** [[DOTPART_ID__ADDR_I]], align 8, !noalias !12
// CHECK1-NEXT:    store i8* [[TMP10]], i8** [[DOTPRIVATES__ADDR_I]], align 8, !noalias !12
// CHECK1-NEXT:    store void (i8*, ...)* bitcast (void (%struct..kmp_privates.t*, %class.a**)* @.omp_task_privates_map. to void (i8*, ...)*), void (i8*, ...)** [[DOTCOPY_FN__ADDR_I]], align 8, !noalias !12
// CHECK1-NEXT:    store i8* [[TMP11]], i8** [[DOTTASK_T__ADDR_I]], align 8, !noalias !12
// CHECK1-NEXT:    store %struct.anon* [[TMP8]], %struct.anon** [[__CONTEXT_ADDR_I]], align 8, !noalias !12
// CHECK1-NEXT:    [[TMP12:%.*]] = load %struct.anon*, %struct.anon** [[__CONTEXT_ADDR_I]], align 8, !noalias !12
// CHECK1-NEXT:    [[TMP13:%.*]] = load void (i8*, ...)*, void (i8*, ...)** [[DOTCOPY_FN__ADDR_I]], align 8, !noalias !12
// CHECK1-NEXT:    [[TMP14:%.*]] = load i8*, i8** [[DOTPRIVATES__ADDR_I]], align 8, !noalias !12
// CHECK1-NEXT:    [[TMP15:%.*]] = bitcast void (i8*, ...)* [[TMP13]] to void (i8*, %class.a**)*
// CHECK1-NEXT:    call void [[TMP15]](i8* [[TMP14]], %class.a** [[DOTFIRSTPRIV_PTR_ADDR_I]]) #[[ATTR4:[0-9]+]]
// CHECK1-NEXT:    [[TMP16:%.*]] = load %class.a*, %class.a** [[DOTFIRSTPRIV_PTR_ADDR_I]], align 8, !noalias !12
// CHECK1-NEXT:    call void @_ZN1a1bEv(%class.a* noundef nonnull align 1 dereferenceable(1) [[TMP16]]) #[[ATTR4]]
// CHECK1-NEXT:    ret i32 0
//
//
// CHECK2-LABEL: define {{[^@]+}}@_Z1cv
// CHECK2-SAME: () #[[ATTR0:[0-9]+]] {
// CHECK2-NEXT:  entry:
// CHECK2-NEXT:    [[D:%.*]] = alloca [[CLASS_A:%.*]], align 1
// CHECK2-NEXT:    [[AGG_CAPTURED:%.*]] = alloca [[STRUCT_ANON:%.*]], align 1
// CHECK2-NEXT:    [[TMP0:%.*]] = call i32 @__kmpc_global_thread_num(%struct.ident_t* @[[GLOB1:[0-9]+]])
// CHECK2-NEXT:    [[TMP1:%.*]] = call i8* @__kmpc_omp_task_alloc(%struct.ident_t* @[[GLOB1]], i32 [[TMP0]], i32 1, i64 48, i64 1, i32 (i32, i8*)* bitcast (i32 (i32, %struct.kmp_task_t_with_privates*)* @.omp_task_entry. to i32 (i32, i8*)*))
// CHECK2-NEXT:    [[TMP2:%.*]] = bitcast i8* [[TMP1]] to %struct.kmp_task_t_with_privates*
// CHECK2-NEXT:    [[TMP3:%.*]] = getelementptr inbounds [[STRUCT_KMP_TASK_T_WITH_PRIVATES:%.*]], %struct.kmp_task_t_with_privates* [[TMP2]], i32 0, i32 0
// CHECK2-NEXT:    [[TMP4:%.*]] = getelementptr inbounds [[STRUCT_KMP_TASK_T_WITH_PRIVATES]], %struct.kmp_task_t_with_privates* [[TMP2]], i32 0, i32 1
// CHECK2-NEXT:    [[TMP5:%.*]] = getelementptr inbounds [[STRUCT__KMP_PRIVATES_T:%.*]], %struct..kmp_privates.t* [[TMP4]], i32 0, i32 0
// CHECK2-NEXT:    [[TMP6:%.*]] = call i32 @__kmpc_omp_task(%struct.ident_t* @[[GLOB1]], i32 [[TMP0]], i8* [[TMP1]])
// CHECK2-NEXT:    ret void
//
//
// CHECK2-LABEL: define {{[^@]+}}@.omp_task_privates_map.
// CHECK2-SAME: (%struct..kmp_privates.t* noalias noundef [[TMP0:%.*]], %class.a** noalias noundef [[TMP1:%.*]]) #[[ATTR2:[0-9]+]] {
// CHECK2-NEXT:  entry:
// CHECK2-NEXT:    [[DOTADDR:%.*]] = alloca %struct..kmp_privates.t*, align 8
// CHECK2-NEXT:    [[DOTADDR1:%.*]] = alloca %class.a**, align 8
// CHECK2-NEXT:    store %struct..kmp_privates.t* [[TMP0]], %struct..kmp_privates.t** [[DOTADDR]], align 8
// CHECK2-NEXT:    store %class.a** [[TMP1]], %class.a*** [[DOTADDR1]], align 8
// CHECK2-NEXT:    [[TMP2:%.*]] = load %struct..kmp_privates.t*, %struct..kmp_privates.t** [[DOTADDR]], align 8
// CHECK2-NEXT:    [[TMP3:%.*]] = getelementptr inbounds [[STRUCT__KMP_PRIVATES_T:%.*]], %struct..kmp_privates.t* [[TMP2]], i32 0, i32 0
// CHECK2-NEXT:    [[TMP4:%.*]] = load %class.a**, %class.a*** [[DOTADDR1]], align 8
// CHECK2-NEXT:    store %class.a* [[TMP3]], %class.a** [[TMP4]], align 8
// CHECK2-NEXT:    ret void
//
//
// CHECK2-LABEL: define {{[^@]+}}@.omp_task_entry.
// CHECK2-SAME: (i32 noundef [[TMP0:%.*]], %struct.kmp_task_t_with_privates* noalias noundef [[TMP1:%.*]]) #[[ATTR3:[0-9]+]] {
// CHECK2-NEXT:  entry:
// CHECK2-NEXT:    [[DOTGLOBAL_TID__ADDR_I:%.*]] = alloca i32, align 4
// CHECK2-NEXT:    [[DOTPART_ID__ADDR_I:%.*]] = alloca i32*, align 8
// CHECK2-NEXT:    [[DOTPRIVATES__ADDR_I:%.*]] = alloca i8*, align 8
// CHECK2-NEXT:    [[DOTCOPY_FN__ADDR_I:%.*]] = alloca void (i8*, ...)*, align 8
// CHECK2-NEXT:    [[DOTTASK_T__ADDR_I:%.*]] = alloca i8*, align 8
// CHECK2-NEXT:    [[__CONTEXT_ADDR_I:%.*]] = alloca %struct.anon*, align 8
// CHECK2-NEXT:    [[DOTFIRSTPRIV_PTR_ADDR_I:%.*]] = alloca %class.a*, align 8
// CHECK2-NEXT:    [[DOTADDR:%.*]] = alloca i32, align 4
// CHECK2-NEXT:    [[DOTADDR1:%.*]] = alloca %struct.kmp_task_t_with_privates*, align 8
// CHECK2-NEXT:    store i32 [[TMP0]], i32* [[DOTADDR]], align 4
// CHECK2-NEXT:    store %struct.kmp_task_t_with_privates* [[TMP1]], %struct.kmp_task_t_with_privates** [[DOTADDR1]], align 8
// CHECK2-NEXT:    [[TMP2:%.*]] = load i32, i32* [[DOTADDR]], align 4
// CHECK2-NEXT:    [[TMP3:%.*]] = load %struct.kmp_task_t_with_privates*, %struct.kmp_task_t_with_privates** [[DOTADDR1]], align 8
// CHECK2-NEXT:    [[TMP4:%.*]] = getelementptr inbounds [[STRUCT_KMP_TASK_T_WITH_PRIVATES:%.*]], %struct.kmp_task_t_with_privates* [[TMP3]], i32 0, i32 0
// CHECK2-NEXT:    [[TMP5:%.*]] = getelementptr inbounds [[STRUCT_KMP_TASK_T:%.*]], %struct.kmp_task_t* [[TMP4]], i32 0, i32 2
// CHECK2-NEXT:    [[TMP6:%.*]] = getelementptr inbounds [[STRUCT_KMP_TASK_T]], %struct.kmp_task_t* [[TMP4]], i32 0, i32 0
// CHECK2-NEXT:    [[TMP7:%.*]] = load i8*, i8** [[TMP6]], align 8
// CHECK2-NEXT:    [[TMP8:%.*]] = bitcast i8* [[TMP7]] to %struct.anon*
// CHECK2-NEXT:    [[TMP9:%.*]] = getelementptr inbounds [[STRUCT_KMP_TASK_T_WITH_PRIVATES]], %struct.kmp_task_t_with_privates* [[TMP3]], i32 0, i32 1
// CHECK2-NEXT:    [[TMP10:%.*]] = bitcast %struct..kmp_privates.t* [[TMP9]] to i8*
// CHECK2-NEXT:    [[TMP11:%.*]] = bitcast %struct.kmp_task_t_with_privates* [[TMP3]] to i8*
// CHECK2-NEXT:    call void @llvm.experimental.noalias.scope.decl(metadata [[META3:![0-9]+]])
// CHECK2-NEXT:    call void @llvm.experimental.noalias.scope.decl(metadata [[META6:![0-9]+]])
// CHECK2-NEXT:    call void @llvm.experimental.noalias.scope.decl(metadata [[META8:![0-9]+]])
// CHECK2-NEXT:    call void @llvm.experimental.noalias.scope.decl(metadata [[META10:![0-9]+]])
// CHECK2-NEXT:    store i32 [[TMP2]], i32* [[DOTGLOBAL_TID__ADDR_I]], align 4, !noalias !12
// CHECK2-NEXT:    store i32* [[TMP5]], i32** [[DOTPART_ID__ADDR_I]], align 8, !noalias !12
// CHECK2-NEXT:    store i8* [[TMP10]], i8** [[DOTPRIVATES__ADDR_I]], align 8, !noalias !12
// CHECK2-NEXT:    store void (i8*, ...)* bitcast (void (%struct..kmp_privates.t*, %class.a**)* @.omp_task_privates_map. to void (i8*, ...)*), void (i8*, ...)** [[DOTCOPY_FN__ADDR_I]], align 8, !noalias !12
// CHECK2-NEXT:    store i8* [[TMP11]], i8** [[DOTTASK_T__ADDR_I]], align 8, !noalias !12
// CHECK2-NEXT:    store %struct.anon* [[TMP8]], %struct.anon** [[__CONTEXT_ADDR_I]], align 8, !noalias !12
// CHECK2-NEXT:    [[TMP12:%.*]] = load %struct.anon*, %struct.anon** [[__CONTEXT_ADDR_I]], align 8, !noalias !12
// CHECK2-NEXT:    [[TMP13:%.*]] = load void (i8*, ...)*, void (i8*, ...)** [[DOTCOPY_FN__ADDR_I]], align 8, !noalias !12
// CHECK2-NEXT:    [[TMP14:%.*]] = load i8*, i8** [[DOTPRIVATES__ADDR_I]], align 8, !noalias !12
// CHECK2-NEXT:    [[TMP15:%.*]] = bitcast void (i8*, ...)* [[TMP13]] to void (i8*, %class.a**)*
// CHECK2-NEXT:    call void [[TMP15]](i8* [[TMP14]], %class.a** [[DOTFIRSTPRIV_PTR_ADDR_I]]) #[[ATTR4:[0-9]+]]
// CHECK2-NEXT:    [[TMP16:%.*]] = load %class.a*, %class.a** [[DOTFIRSTPRIV_PTR_ADDR_I]], align 8, !noalias !12
// CHECK2-NEXT:    call void @_ZN1a1bEv(%class.a* noundef nonnull align 1 dereferenceable(1) [[TMP16]]) #[[ATTR4]]
// CHECK2-NEXT:    ret i32 0
//
//
// CHECK3-LABEL: define {{[^@]+}}@_Z1cv
// CHECK3-SAME: () #[[ATTR0:[0-9]+]] {
// CHECK3-NEXT:  entry:
// CHECK3-NEXT:    [[D:%.*]] = alloca [[CLASS_A:%.*]], align 1
// CHECK3-NEXT:    [[AGG_CAPTURED:%.*]] = alloca [[STRUCT_ANON:%.*]], align 1
// CHECK3-NEXT:    [[OMP_GLOBAL_THREAD_NUM:%.*]] = call i32 @__kmpc_global_thread_num(%struct.ident_t* @[[GLOB3:[0-9]+]])
// CHECK3-NEXT:    [[TMP0:%.*]] = call i8* @__kmpc_omp_task_alloc(%struct.ident_t* @[[GLOB1:[0-9]+]], i32 [[OMP_GLOBAL_THREAD_NUM]], i32 1, i64 48, i64 1, i32 (i32, i8*)* bitcast (i32 (i32, %struct.kmp_task_t_with_privates*)* @.omp_task_entry. to i32 (i32, i8*)*))
// CHECK3-NEXT:    [[TMP1:%.*]] = bitcast i8* [[TMP0]] to %struct.kmp_task_t_with_privates*
// CHECK3-NEXT:    [[TMP2:%.*]] = getelementptr inbounds [[STRUCT_KMP_TASK_T_WITH_PRIVATES:%.*]], %struct.kmp_task_t_with_privates* [[TMP1]], i32 0, i32 0
// CHECK3-NEXT:    [[TMP3:%.*]] = getelementptr inbounds [[STRUCT_KMP_TASK_T_WITH_PRIVATES]], %struct.kmp_task_t_with_privates* [[TMP1]], i32 0, i32 1
// CHECK3-NEXT:    [[TMP4:%.*]] = getelementptr inbounds [[STRUCT__KMP_PRIVATES_T:%.*]], %struct..kmp_privates.t* [[TMP3]], i32 0, i32 0
// CHECK3-NEXT:    [[OMP_GLOBAL_THREAD_NUM1:%.*]] = call i32 @__kmpc_global_thread_num(%struct.ident_t* @[[GLOB3]])
// CHECK3-NEXT:    [[TMP5:%.*]] = call i32 @__kmpc_omp_task(%struct.ident_t* @[[GLOB1]], i32 [[OMP_GLOBAL_THREAD_NUM1]], i8* [[TMP0]])
// CHECK3-NEXT:    ret void
//
//
// CHECK3-LABEL: define {{[^@]+}}@.omp_task_privates_map.
// CHECK3-SAME: (%struct..kmp_privates.t* noalias noundef [[TMP0:%.*]], %class.a** noalias noundef [[TMP1:%.*]]) #[[ATTR2:[0-9]+]] {
// CHECK3-NEXT:  entry:
// CHECK3-NEXT:    [[DOTADDR:%.*]] = alloca %struct..kmp_privates.t*, align 8
// CHECK3-NEXT:    [[DOTADDR1:%.*]] = alloca %class.a**, align 8
// CHECK3-NEXT:    store %struct..kmp_privates.t* [[TMP0]], %struct..kmp_privates.t** [[DOTADDR]], align 8
// CHECK3-NEXT:    store %class.a** [[TMP1]], %class.a*** [[DOTADDR1]], align 8
// CHECK3-NEXT:    [[TMP2:%.*]] = load %struct..kmp_privates.t*, %struct..kmp_privates.t** [[DOTADDR]], align 8
// CHECK3-NEXT:    [[TMP3:%.*]] = getelementptr inbounds [[STRUCT__KMP_PRIVATES_T:%.*]], %struct..kmp_privates.t* [[TMP2]], i32 0, i32 0
// CHECK3-NEXT:    [[TMP4:%.*]] = load %class.a**, %class.a*** [[DOTADDR1]], align 8
// CHECK3-NEXT:    store %class.a* [[TMP3]], %class.a** [[TMP4]], align 8
// CHECK3-NEXT:    ret void
//
//
// CHECK3-LABEL: define {{[^@]+}}@.omp_task_entry.
// CHECK3-SAME: (i32 noundef [[TMP0:%.*]], %struct.kmp_task_t_with_privates* noalias noundef [[TMP1:%.*]]) #[[ATTR3:[0-9]+]] {
// CHECK3-NEXT:  entry:
// CHECK3-NEXT:    [[DOTGLOBAL_TID__ADDR_I:%.*]] = alloca i32, align 4
// CHECK3-NEXT:    [[DOTPART_ID__ADDR_I:%.*]] = alloca i32*, align 8
// CHECK3-NEXT:    [[DOTPRIVATES__ADDR_I:%.*]] = alloca i8*, align 8
// CHECK3-NEXT:    [[DOTCOPY_FN__ADDR_I:%.*]] = alloca void (i8*, ...)*, align 8
// CHECK3-NEXT:    [[DOTTASK_T__ADDR_I:%.*]] = alloca i8*, align 8
// CHECK3-NEXT:    [[__CONTEXT_ADDR_I:%.*]] = alloca %struct.anon*, align 8
// CHECK3-NEXT:    [[DOTFIRSTPRIV_PTR_ADDR_I:%.*]] = alloca %class.a*, align 8
// CHECK3-NEXT:    [[DOTADDR:%.*]] = alloca i32, align 4
// CHECK3-NEXT:    [[DOTADDR1:%.*]] = alloca %struct.kmp_task_t_with_privates*, align 8
// CHECK3-NEXT:    store i32 [[TMP0]], i32* [[DOTADDR]], align 4
// CHECK3-NEXT:    store %struct.kmp_task_t_with_privates* [[TMP1]], %struct.kmp_task_t_with_privates** [[DOTADDR1]], align 8
// CHECK3-NEXT:    [[TMP2:%.*]] = load i32, i32* [[DOTADDR]], align 4
// CHECK3-NEXT:    [[TMP3:%.*]] = load %struct.kmp_task_t_with_privates*, %struct.kmp_task_t_with_privates** [[DOTADDR1]], align 8
// CHECK3-NEXT:    [[TMP4:%.*]] = getelementptr inbounds [[STRUCT_KMP_TASK_T_WITH_PRIVATES:%.*]], %struct.kmp_task_t_with_privates* [[TMP3]], i32 0, i32 0
// CHECK3-NEXT:    [[TMP5:%.*]] = getelementptr inbounds [[STRUCT_KMP_TASK_T:%.*]], %struct.kmp_task_t* [[TMP4]], i32 0, i32 2
// CHECK3-NEXT:    [[TMP6:%.*]] = getelementptr inbounds [[STRUCT_KMP_TASK_T]], %struct.kmp_task_t* [[TMP4]], i32 0, i32 0
// CHECK3-NEXT:    [[TMP7:%.*]] = load i8*, i8** [[TMP6]], align 8
// CHECK3-NEXT:    [[TMP8:%.*]] = bitcast i8* [[TMP7]] to %struct.anon*
// CHECK3-NEXT:    [[TMP9:%.*]] = getelementptr inbounds [[STRUCT_KMP_TASK_T_WITH_PRIVATES]], %struct.kmp_task_t_with_privates* [[TMP3]], i32 0, i32 1
// CHECK3-NEXT:    [[TMP10:%.*]] = bitcast %struct..kmp_privates.t* [[TMP9]] to i8*
// CHECK3-NEXT:    [[TMP11:%.*]] = bitcast %struct.kmp_task_t_with_privates* [[TMP3]] to i8*
// CHECK3-NEXT:    call void @llvm.experimental.noalias.scope.decl(metadata [[META3:![0-9]+]])
// CHECK3-NEXT:    call void @llvm.experimental.noalias.scope.decl(metadata [[META6:![0-9]+]])
// CHECK3-NEXT:    call void @llvm.experimental.noalias.scope.decl(metadata [[META8:![0-9]+]])
// CHECK3-NEXT:    call void @llvm.experimental.noalias.scope.decl(metadata [[META10:![0-9]+]])
// CHECK3-NEXT:    store i32 [[TMP2]], i32* [[DOTGLOBAL_TID__ADDR_I]], align 4, !noalias !12
// CHECK3-NEXT:    store i32* [[TMP5]], i32** [[DOTPART_ID__ADDR_I]], align 8, !noalias !12
// CHECK3-NEXT:    store i8* [[TMP10]], i8** [[DOTPRIVATES__ADDR_I]], align 8, !noalias !12
// CHECK3-NEXT:    store void (i8*, ...)* bitcast (void (%struct..kmp_privates.t*, %class.a**)* @.omp_task_privates_map. to void (i8*, ...)*), void (i8*, ...)** [[DOTCOPY_FN__ADDR_I]], align 8, !noalias !12
// CHECK3-NEXT:    store i8* [[TMP11]], i8** [[DOTTASK_T__ADDR_I]], align 8, !noalias !12
// CHECK3-NEXT:    store %struct.anon* [[TMP8]], %struct.anon** [[__CONTEXT_ADDR_I]], align 8, !noalias !12
// CHECK3-NEXT:    [[TMP12:%.*]] = load %struct.anon*, %struct.anon** [[__CONTEXT_ADDR_I]], align 8, !noalias !12
// CHECK3-NEXT:    [[TMP13:%.*]] = load void (i8*, ...)*, void (i8*, ...)** [[DOTCOPY_FN__ADDR_I]], align 8, !noalias !12
// CHECK3-NEXT:    [[TMP14:%.*]] = load i8*, i8** [[DOTPRIVATES__ADDR_I]], align 8, !noalias !12
// CHECK3-NEXT:    [[TMP15:%.*]] = bitcast void (i8*, ...)* [[TMP13]] to void (i8*, %class.a**)*
// CHECK3-NEXT:    call void [[TMP15]](i8* [[TMP14]], %class.a** [[DOTFIRSTPRIV_PTR_ADDR_I]]) #[[ATTR4:[0-9]+]]
// CHECK3-NEXT:    [[TMP16:%.*]] = load %class.a*, %class.a** [[DOTFIRSTPRIV_PTR_ADDR_I]], align 8, !noalias !12
// CHECK3-NEXT:    call void @_ZN1a1bEv(%class.a* noundef nonnull align 1 dereferenceable(1) [[TMP16]]) #[[ATTR4]]
// CHECK3-NEXT:    ret i32 0
//
//
// CHECK4-LABEL: define {{[^@]+}}@_Z1cv
// CHECK4-SAME: () #[[ATTR0:[0-9]+]] {
// CHECK4-NEXT:  entry:
// CHECK4-NEXT:    [[D:%.*]] = alloca [[CLASS_A:%.*]], align 1
// CHECK4-NEXT:    [[AGG_CAPTURED:%.*]] = alloca [[STRUCT_ANON:%.*]], align 1
// CHECK4-NEXT:    [[OMP_GLOBAL_THREAD_NUM:%.*]] = call i32 @__kmpc_global_thread_num(%struct.ident_t* @[[GLOB3:[0-9]+]])
// CHECK4-NEXT:    [[TMP0:%.*]] = call i8* @__kmpc_omp_task_alloc(%struct.ident_t* @[[GLOB1:[0-9]+]], i32 [[OMP_GLOBAL_THREAD_NUM]], i32 1, i64 48, i64 1, i32 (i32, i8*)* bitcast (i32 (i32, %struct.kmp_task_t_with_privates*)* @.omp_task_entry. to i32 (i32, i8*)*))
// CHECK4-NEXT:    [[TMP1:%.*]] = bitcast i8* [[TMP0]] to %struct.kmp_task_t_with_privates*
// CHECK4-NEXT:    [[TMP2:%.*]] = getelementptr inbounds [[STRUCT_KMP_TASK_T_WITH_PRIVATES:%.*]], %struct.kmp_task_t_with_privates* [[TMP1]], i32 0, i32 0
// CHECK4-NEXT:    [[TMP3:%.*]] = getelementptr inbounds [[STRUCT_KMP_TASK_T_WITH_PRIVATES]], %struct.kmp_task_t_with_privates* [[TMP1]], i32 0, i32 1
// CHECK4-NEXT:    [[TMP4:%.*]] = getelementptr inbounds [[STRUCT__KMP_PRIVATES_T:%.*]], %struct..kmp_privates.t* [[TMP3]], i32 0, i32 0
// CHECK4-NEXT:    [[OMP_GLOBAL_THREAD_NUM1:%.*]] = call i32 @__kmpc_global_thread_num(%struct.ident_t* @[[GLOB3]])
// CHECK4-NEXT:    [[TMP5:%.*]] = call i32 @__kmpc_omp_task(%struct.ident_t* @[[GLOB1]], i32 [[OMP_GLOBAL_THREAD_NUM1]], i8* [[TMP0]])
// CHECK4-NEXT:    ret void
//
//
// CHECK4-LABEL: define {{[^@]+}}@.omp_task_privates_map.
// CHECK4-SAME: (%struct..kmp_privates.t* noalias noundef [[TMP0:%.*]], %class.a** noalias noundef [[TMP1:%.*]]) #[[ATTR2:[0-9]+]] {
// CHECK4-NEXT:  entry:
// CHECK4-NEXT:    [[DOTADDR:%.*]] = alloca %struct..kmp_privates.t*, align 8
// CHECK4-NEXT:    [[DOTADDR1:%.*]] = alloca %class.a**, align 8
// CHECK4-NEXT:    store %struct..kmp_privates.t* [[TMP0]], %struct..kmp_privates.t** [[DOTADDR]], align 8
// CHECK4-NEXT:    store %class.a** [[TMP1]], %class.a*** [[DOTADDR1]], align 8
// CHECK4-NEXT:    [[TMP2:%.*]] = load %struct..kmp_privates.t*, %struct..kmp_privates.t** [[DOTADDR]], align 8
// CHECK4-NEXT:    [[TMP3:%.*]] = getelementptr inbounds [[STRUCT__KMP_PRIVATES_T:%.*]], %struct..kmp_privates.t* [[TMP2]], i32 0, i32 0
// CHECK4-NEXT:    [[TMP4:%.*]] = load %class.a**, %class.a*** [[DOTADDR1]], align 8
// CHECK4-NEXT:    store %class.a* [[TMP3]], %class.a** [[TMP4]], align 8
// CHECK4-NEXT:    ret void
//
//
// CHECK4-LABEL: define {{[^@]+}}@.omp_task_entry.
// CHECK4-SAME: (i32 noundef [[TMP0:%.*]], %struct.kmp_task_t_with_privates* noalias noundef [[TMP1:%.*]]) #[[ATTR3:[0-9]+]] {
// CHECK4-NEXT:  entry:
// CHECK4-NEXT:    [[DOTGLOBAL_TID__ADDR_I:%.*]] = alloca i32, align 4
// CHECK4-NEXT:    [[DOTPART_ID__ADDR_I:%.*]] = alloca i32*, align 8
// CHECK4-NEXT:    [[DOTPRIVATES__ADDR_I:%.*]] = alloca i8*, align 8
// CHECK4-NEXT:    [[DOTCOPY_FN__ADDR_I:%.*]] = alloca void (i8*, ...)*, align 8
// CHECK4-NEXT:    [[DOTTASK_T__ADDR_I:%.*]] = alloca i8*, align 8
// CHECK4-NEXT:    [[__CONTEXT_ADDR_I:%.*]] = alloca %struct.anon*, align 8
// CHECK4-NEXT:    [[DOTFIRSTPRIV_PTR_ADDR_I:%.*]] = alloca %class.a*, align 8
// CHECK4-NEXT:    [[DOTADDR:%.*]] = alloca i32, align 4
// CHECK4-NEXT:    [[DOTADDR1:%.*]] = alloca %struct.kmp_task_t_with_privates*, align 8
// CHECK4-NEXT:    store i32 [[TMP0]], i32* [[DOTADDR]], align 4
// CHECK4-NEXT:    store %struct.kmp_task_t_with_privates* [[TMP1]], %struct.kmp_task_t_with_privates** [[DOTADDR1]], align 8
// CHECK4-NEXT:    [[TMP2:%.*]] = load i32, i32* [[DOTADDR]], align 4
// CHECK4-NEXT:    [[TMP3:%.*]] = load %struct.kmp_task_t_with_privates*, %struct.kmp_task_t_with_privates** [[DOTADDR1]], align 8
// CHECK4-NEXT:    [[TMP4:%.*]] = getelementptr inbounds [[STRUCT_KMP_TASK_T_WITH_PRIVATES:%.*]], %struct.kmp_task_t_with_privates* [[TMP3]], i32 0, i32 0
// CHECK4-NEXT:    [[TMP5:%.*]] = getelementptr inbounds [[STRUCT_KMP_TASK_T:%.*]], %struct.kmp_task_t* [[TMP4]], i32 0, i32 2
// CHECK4-NEXT:    [[TMP6:%.*]] = getelementptr inbounds [[STRUCT_KMP_TASK_T]], %struct.kmp_task_t* [[TMP4]], i32 0, i32 0
// CHECK4-NEXT:    [[TMP7:%.*]] = load i8*, i8** [[TMP6]], align 8
// CHECK4-NEXT:    [[TMP8:%.*]] = bitcast i8* [[TMP7]] to %struct.anon*
// CHECK4-NEXT:    [[TMP9:%.*]] = getelementptr inbounds [[STRUCT_KMP_TASK_T_WITH_PRIVATES]], %struct.kmp_task_t_with_privates* [[TMP3]], i32 0, i32 1
// CHECK4-NEXT:    [[TMP10:%.*]] = bitcast %struct..kmp_privates.t* [[TMP9]] to i8*
// CHECK4-NEXT:    [[TMP11:%.*]] = bitcast %struct.kmp_task_t_with_privates* [[TMP3]] to i8*
// CHECK4-NEXT:    call void @llvm.experimental.noalias.scope.decl(metadata [[META3:![0-9]+]])
// CHECK4-NEXT:    call void @llvm.experimental.noalias.scope.decl(metadata [[META6:![0-9]+]])
// CHECK4-NEXT:    call void @llvm.experimental.noalias.scope.decl(metadata [[META8:![0-9]+]])
// CHECK4-NEXT:    call void @llvm.experimental.noalias.scope.decl(metadata [[META10:![0-9]+]])
// CHECK4-NEXT:    store i32 [[TMP2]], i32* [[DOTGLOBAL_TID__ADDR_I]], align 4, !noalias !12
// CHECK4-NEXT:    store i32* [[TMP5]], i32** [[DOTPART_ID__ADDR_I]], align 8, !noalias !12
// CHECK4-NEXT:    store i8* [[TMP10]], i8** [[DOTPRIVATES__ADDR_I]], align 8, !noalias !12
// CHECK4-NEXT:    store void (i8*, ...)* bitcast (void (%struct..kmp_privates.t*, %class.a**)* @.omp_task_privates_map. to void (i8*, ...)*), void (i8*, ...)** [[DOTCOPY_FN__ADDR_I]], align 8, !noalias !12
// CHECK4-NEXT:    store i8* [[TMP11]], i8** [[DOTTASK_T__ADDR_I]], align 8, !noalias !12
// CHECK4-NEXT:    store %struct.anon* [[TMP8]], %struct.anon** [[__CONTEXT_ADDR_I]], align 8, !noalias !12
// CHECK4-NEXT:    [[TMP12:%.*]] = load %struct.anon*, %struct.anon** [[__CONTEXT_ADDR_I]], align 8, !noalias !12
// CHECK4-NEXT:    [[TMP13:%.*]] = load void (i8*, ...)*, void (i8*, ...)** [[DOTCOPY_FN__ADDR_I]], align 8, !noalias !12
// CHECK4-NEXT:    [[TMP14:%.*]] = load i8*, i8** [[DOTPRIVATES__ADDR_I]], align 8, !noalias !12
// CHECK4-NEXT:    [[TMP15:%.*]] = bitcast void (i8*, ...)* [[TMP13]] to void (i8*, %class.a**)*
// CHECK4-NEXT:    call void [[TMP15]](i8* [[TMP14]], %class.a** [[DOTFIRSTPRIV_PTR_ADDR_I]]) #[[ATTR4:[0-9]+]]
// CHECK4-NEXT:    [[TMP16:%.*]] = load %class.a*, %class.a** [[DOTFIRSTPRIV_PTR_ADDR_I]], align 8, !noalias !12
// CHECK4-NEXT:    call void @_ZN1a1bEv(%class.a* noundef nonnull align 1 dereferenceable(1) [[TMP16]]) #[[ATTR4]]
// CHECK4-NEXT:    ret i32 0
//
