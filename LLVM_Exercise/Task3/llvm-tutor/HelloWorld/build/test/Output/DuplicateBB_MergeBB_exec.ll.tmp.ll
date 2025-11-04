; ModuleID = '<stdin>'
source_filename = "/Users/lazyeater/bril/adv_compiler/LLVM_Exercise/Task3/llvm-tutor/test/../inputs/input_for_mba.c"
target datalayout = "e-m:o-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-n32:64-S128-Fn32"
target triple = "arm64-apple-macosx15.0.0"

; Function Attrs: noinline nounwind optnone ssp uwtable(sync)
define signext i8 @foo(i8 noundef signext %0, i8 noundef signext %1, i8 noundef signext %2, i8 noundef signext %3) #0 {
lt-if-then-else-0:
  %4 = icmp eq i8 %1, 0
  br i1 %4, label %lt-clone-1-0, label %lt-clone-2-0

lt-clone-1-0:                                     ; preds = %lt-if-then-else-0
  %5 = alloca i8, align 1
  %6 = alloca i8, align 1
  %7 = alloca i8, align 1
  %8 = alloca i8, align 1
  %9 = alloca i8, align 1
  %10 = alloca i8, align 1
  store i8 %0, ptr %5, align 1
  store i8 %1, ptr %6, align 1
  store i8 %2, ptr %7, align 1
  store i8 %3, ptr %8, align 1
  %11 = load i8, ptr %7, align 1
  %12 = sext i8 %11 to i32
  %13 = load i8, ptr %8, align 1
  %14 = sext i8 %13 to i32
  %15 = add nsw i32 %12, %14
  %16 = trunc i32 %15 to i8
  store i8 %16, ptr %9, align 1
  %17 = load i8, ptr %5, align 1
  %18 = sext i8 %17 to i32
  %19 = load i8, ptr %6, align 1
  %20 = sext i8 %19 to i32
  %21 = add nsw i32 %18, %20
  %22 = trunc i32 %21 to i8
  store i8 %22, ptr %10, align 1
  %23 = load i8, ptr %9, align 1
  %24 = sext i8 %23 to i32
  %25 = load i8, ptr %10, align 1
  %26 = sext i8 %25 to i32
  %27 = add nsw i32 %24, %26
  %28 = trunc i32 %27 to i8
  br label %lt-tail-0

lt-clone-2-0:                                     ; preds = %lt-if-then-else-0
  %29 = alloca i8, align 1
  %30 = alloca i8, align 1
  %31 = alloca i8, align 1
  %32 = alloca i8, align 1
  %33 = alloca i8, align 1
  %34 = alloca i8, align 1
  store i8 %0, ptr %29, align 1
  store i8 %1, ptr %30, align 1
  store i8 %2, ptr %31, align 1
  store i8 %3, ptr %32, align 1
  %35 = load i8, ptr %31, align 1
  %36 = sext i8 %35 to i32
  %37 = load i8, ptr %32, align 1
  %38 = sext i8 %37 to i32
  %39 = add nsw i32 %36, %38
  %40 = trunc i32 %39 to i8
  store i8 %40, ptr %33, align 1
  %41 = load i8, ptr %29, align 1
  %42 = sext i8 %41 to i32
  %43 = load i8, ptr %30, align 1
  %44 = sext i8 %43 to i32
  %45 = add nsw i32 %42, %44
  %46 = trunc i32 %45 to i8
  store i8 %46, ptr %34, align 1
  %47 = load i8, ptr %33, align 1
  %48 = sext i8 %47 to i32
  %49 = load i8, ptr %34, align 1
  %50 = sext i8 %49 to i32
  %51 = add nsw i32 %48, %50
  %52 = trunc i32 %51 to i8
  br label %lt-tail-0

lt-tail-0:                                        ; preds = %lt-clone-2-0, %lt-clone-1-0
  %53 = phi ptr [ %5, %lt-clone-1-0 ], [ %29, %lt-clone-2-0 ]
  %54 = phi ptr [ %6, %lt-clone-1-0 ], [ %30, %lt-clone-2-0 ]
  %55 = phi ptr [ %7, %lt-clone-1-0 ], [ %31, %lt-clone-2-0 ]
  %56 = phi ptr [ %8, %lt-clone-1-0 ], [ %32, %lt-clone-2-0 ]
  %57 = phi ptr [ %9, %lt-clone-1-0 ], [ %33, %lt-clone-2-0 ]
  %58 = phi ptr [ %10, %lt-clone-1-0 ], [ %34, %lt-clone-2-0 ]
  %59 = phi i8 [ %11, %lt-clone-1-0 ], [ %35, %lt-clone-2-0 ]
  %60 = phi i32 [ %12, %lt-clone-1-0 ], [ %36, %lt-clone-2-0 ]
  %61 = phi i8 [ %13, %lt-clone-1-0 ], [ %37, %lt-clone-2-0 ]
  %62 = phi i32 [ %14, %lt-clone-1-0 ], [ %38, %lt-clone-2-0 ]
  %63 = phi i32 [ %15, %lt-clone-1-0 ], [ %39, %lt-clone-2-0 ]
  %64 = phi i8 [ %16, %lt-clone-1-0 ], [ %40, %lt-clone-2-0 ]
  %65 = phi i8 [ %17, %lt-clone-1-0 ], [ %41, %lt-clone-2-0 ]
  %66 = phi i32 [ %18, %lt-clone-1-0 ], [ %42, %lt-clone-2-0 ]
  %67 = phi i8 [ %19, %lt-clone-1-0 ], [ %43, %lt-clone-2-0 ]
  %68 = phi i32 [ %20, %lt-clone-1-0 ], [ %44, %lt-clone-2-0 ]
  %69 = phi i32 [ %21, %lt-clone-1-0 ], [ %45, %lt-clone-2-0 ]
  %70 = phi i8 [ %22, %lt-clone-1-0 ], [ %46, %lt-clone-2-0 ]
  %71 = phi i8 [ %23, %lt-clone-1-0 ], [ %47, %lt-clone-2-0 ]
  %72 = phi i32 [ %24, %lt-clone-1-0 ], [ %48, %lt-clone-2-0 ]
  %73 = phi i8 [ %25, %lt-clone-1-0 ], [ %49, %lt-clone-2-0 ]
  %74 = phi i32 [ %26, %lt-clone-1-0 ], [ %50, %lt-clone-2-0 ]
  %75 = phi i32 [ %27, %lt-clone-1-0 ], [ %51, %lt-clone-2-0 ]
  %76 = phi i8 [ %28, %lt-clone-1-0 ], [ %52, %lt-clone-2-0 ]
  ret i8 %76
}

; Function Attrs: noinline nounwind optnone ssp uwtable(sync)
define i32 @main(i32 noundef %0, ptr noundef %1) #0 {
lt-if-then-else-1:
  %2 = icmp eq i32 %0, 0
  br i1 %2, label %lt-clone-1-1, label %lt-clone-2-1

lt-clone-1-1:                                     ; preds = %lt-if-then-else-1
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca ptr, align 8
  %6 = alloca i32, align 4
  %7 = alloca i32, align 4
  %8 = alloca i32, align 4
  %9 = alloca i32, align 4
  %10 = alloca i8, align 1
  store i32 0, ptr %3, align 4
  store i32 %0, ptr %4, align 4
  store ptr %1, ptr %5, align 8
  %11 = load ptr, ptr %5, align 8
  %12 = getelementptr inbounds ptr, ptr %11, i64 1
  %13 = load ptr, ptr %12, align 8
  %14 = call i32 @atoi(ptr noundef %13)
  store i32 %14, ptr %6, align 4
  %15 = load ptr, ptr %5, align 8
  %16 = getelementptr inbounds ptr, ptr %15, i64 2
  %17 = load ptr, ptr %16, align 8
  %18 = call i32 @atoi(ptr noundef %17)
  store i32 %18, ptr %7, align 4
  %19 = load ptr, ptr %5, align 8
  %20 = getelementptr inbounds ptr, ptr %19, i64 3
  %21 = load ptr, ptr %20, align 8
  %22 = call i32 @atoi(ptr noundef %21)
  store i32 %22, ptr %8, align 4
  %23 = load ptr, ptr %5, align 8
  %24 = getelementptr inbounds ptr, ptr %23, i64 4
  %25 = load ptr, ptr %24, align 8
  %26 = call i32 @atoi(ptr noundef %25)
  store i32 %26, ptr %9, align 4
  %27 = load i32, ptr %6, align 4
  %28 = trunc i32 %27 to i8
  %29 = load i32, ptr %7, align 4
  %30 = trunc i32 %29 to i8
  %31 = load i32, ptr %8, align 4
  %32 = trunc i32 %31 to i8
  %33 = load i32, ptr %9, align 4
  %34 = trunc i32 %33 to i8
  %35 = call signext i8 @foo(i8 noundef signext %28, i8 noundef signext %30, i8 noundef signext %32, i8 noundef signext %34)
  store i8 %35, ptr %10, align 1
  %36 = load i8, ptr %10, align 1
  %37 = sext i8 %36 to i32
  br label %lt-tail-1

lt-clone-2-1:                                     ; preds = %lt-if-then-else-1
  %38 = alloca i32, align 4
  %39 = alloca i32, align 4
  %40 = alloca ptr, align 8
  %41 = alloca i32, align 4
  %42 = alloca i32, align 4
  %43 = alloca i32, align 4
  %44 = alloca i32, align 4
  %45 = alloca i8, align 1
  store i32 0, ptr %38, align 4
  store i32 %0, ptr %39, align 4
  store ptr %1, ptr %40, align 8
  %46 = load ptr, ptr %40, align 8
  %47 = getelementptr inbounds ptr, ptr %46, i64 1
  %48 = load ptr, ptr %47, align 8
  %49 = call i32 @atoi(ptr noundef %48)
  store i32 %49, ptr %41, align 4
  %50 = load ptr, ptr %40, align 8
  %51 = getelementptr inbounds ptr, ptr %50, i64 2
  %52 = load ptr, ptr %51, align 8
  %53 = call i32 @atoi(ptr noundef %52)
  store i32 %53, ptr %42, align 4
  %54 = load ptr, ptr %40, align 8
  %55 = getelementptr inbounds ptr, ptr %54, i64 3
  %56 = load ptr, ptr %55, align 8
  %57 = call i32 @atoi(ptr noundef %56)
  store i32 %57, ptr %43, align 4
  %58 = load ptr, ptr %40, align 8
  %59 = getelementptr inbounds ptr, ptr %58, i64 4
  %60 = load ptr, ptr %59, align 8
  %61 = call i32 @atoi(ptr noundef %60)
  store i32 %61, ptr %44, align 4
  %62 = load i32, ptr %41, align 4
  %63 = trunc i32 %62 to i8
  %64 = load i32, ptr %42, align 4
  %65 = trunc i32 %64 to i8
  %66 = load i32, ptr %43, align 4
  %67 = trunc i32 %66 to i8
  %68 = load i32, ptr %44, align 4
  %69 = trunc i32 %68 to i8
  %70 = call signext i8 @foo(i8 noundef signext %63, i8 noundef signext %65, i8 noundef signext %67, i8 noundef signext %69)
  store i8 %70, ptr %45, align 1
  %71 = load i8, ptr %45, align 1
  %72 = sext i8 %71 to i32
  br label %lt-tail-1

lt-tail-1:                                        ; preds = %lt-clone-2-1, %lt-clone-1-1
  %73 = phi ptr [ %3, %lt-clone-1-1 ], [ %38, %lt-clone-2-1 ]
  %74 = phi ptr [ %4, %lt-clone-1-1 ], [ %39, %lt-clone-2-1 ]
  %75 = phi ptr [ %5, %lt-clone-1-1 ], [ %40, %lt-clone-2-1 ]
  %76 = phi ptr [ %6, %lt-clone-1-1 ], [ %41, %lt-clone-2-1 ]
  %77 = phi ptr [ %7, %lt-clone-1-1 ], [ %42, %lt-clone-2-1 ]
  %78 = phi ptr [ %8, %lt-clone-1-1 ], [ %43, %lt-clone-2-1 ]
  %79 = phi ptr [ %9, %lt-clone-1-1 ], [ %44, %lt-clone-2-1 ]
  %80 = phi ptr [ %10, %lt-clone-1-1 ], [ %45, %lt-clone-2-1 ]
  %81 = phi ptr [ %11, %lt-clone-1-1 ], [ %46, %lt-clone-2-1 ]
  %82 = phi ptr [ %12, %lt-clone-1-1 ], [ %47, %lt-clone-2-1 ]
  %83 = phi ptr [ %13, %lt-clone-1-1 ], [ %48, %lt-clone-2-1 ]
  %84 = phi i32 [ %14, %lt-clone-1-1 ], [ %49, %lt-clone-2-1 ]
  %85 = phi ptr [ %15, %lt-clone-1-1 ], [ %50, %lt-clone-2-1 ]
  %86 = phi ptr [ %16, %lt-clone-1-1 ], [ %51, %lt-clone-2-1 ]
  %87 = phi ptr [ %17, %lt-clone-1-1 ], [ %52, %lt-clone-2-1 ]
  %88 = phi i32 [ %18, %lt-clone-1-1 ], [ %53, %lt-clone-2-1 ]
  %89 = phi ptr [ %19, %lt-clone-1-1 ], [ %54, %lt-clone-2-1 ]
  %90 = phi ptr [ %20, %lt-clone-1-1 ], [ %55, %lt-clone-2-1 ]
  %91 = phi ptr [ %21, %lt-clone-1-1 ], [ %56, %lt-clone-2-1 ]
  %92 = phi i32 [ %22, %lt-clone-1-1 ], [ %57, %lt-clone-2-1 ]
  %93 = phi ptr [ %23, %lt-clone-1-1 ], [ %58, %lt-clone-2-1 ]
  %94 = phi ptr [ %24, %lt-clone-1-1 ], [ %59, %lt-clone-2-1 ]
  %95 = phi ptr [ %25, %lt-clone-1-1 ], [ %60, %lt-clone-2-1 ]
  %96 = phi i32 [ %26, %lt-clone-1-1 ], [ %61, %lt-clone-2-1 ]
  %97 = phi i32 [ %27, %lt-clone-1-1 ], [ %62, %lt-clone-2-1 ]
  %98 = phi i8 [ %28, %lt-clone-1-1 ], [ %63, %lt-clone-2-1 ]
  %99 = phi i32 [ %29, %lt-clone-1-1 ], [ %64, %lt-clone-2-1 ]
  %100 = phi i8 [ %30, %lt-clone-1-1 ], [ %65, %lt-clone-2-1 ]
  %101 = phi i32 [ %31, %lt-clone-1-1 ], [ %66, %lt-clone-2-1 ]
  %102 = phi i8 [ %32, %lt-clone-1-1 ], [ %67, %lt-clone-2-1 ]
  %103 = phi i32 [ %33, %lt-clone-1-1 ], [ %68, %lt-clone-2-1 ]
  %104 = phi i8 [ %34, %lt-clone-1-1 ], [ %69, %lt-clone-2-1 ]
  %105 = phi i8 [ %35, %lt-clone-1-1 ], [ %70, %lt-clone-2-1 ]
  %106 = phi i8 [ %36, %lt-clone-1-1 ], [ %71, %lt-clone-2-1 ]
  %107 = phi i32 [ %37, %lt-clone-1-1 ], [ %72, %lt-clone-2-1 ]
  ret i32 %107
}

declare i32 @atoi(ptr noundef) #1

attributes #0 = { noinline nounwind optnone ssp uwtable(sync) "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+altnzcv,+ccdp,+ccidx,+ccpp,+complxnum,+crc,+dit,+dotprod,+flagm,+fp-armv8,+fp16fml,+fptoint,+fullfp16,+jsconv,+lse,+neon,+pauth,+perfmon,+predres,+ras,+rcpc,+rdm,+sb,+sha2,+sha3,+specrestrict,+ssbs,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8a" }
attributes #1 = { "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+altnzcv,+ccdp,+ccidx,+ccpp,+complxnum,+crc,+dit,+dotprod,+flagm,+fp-armv8,+fp16fml,+fptoint,+fullfp16,+jsconv,+lse,+neon,+pauth,+perfmon,+predres,+ras,+rcpc,+rdm,+sb,+sha2,+sha3,+specrestrict,+ssbs,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8a" }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 2, !"SDK Version", [2 x i32] [i32 15, i32 5]}
!1 = !{i32 1, !"wchar_size", i32 4}
!2 = !{i32 8, !"PIC Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 1}
!4 = !{i32 7, !"frame-pointer", i32 1}
!5 = !{!"Homebrew clang version 21.1.2"}
