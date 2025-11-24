; ModuleID = 'tests/task5_tests/mustAnalysis/test1.c'
source_filename = "tests/task5_tests/mustAnalysis/test1.c"
target datalayout = "e-m:o-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-n32:64-S128-Fn32"
target triple = "arm64-apple-macosx26.0.0"

; Function Attrs: noinline nounwind ssp uwtable(sync)
define i32 @dse_must1(ptr noundef %0, i32 noundef %1) #0 {
  %3 = alloca ptr, align 8
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  %7 = alloca i32, align 4
  store ptr %0, ptr %3, align 8
  store i32 %1, ptr %4, align 4
  %8 = load ptr, ptr %3, align 8
  store i32 10, ptr %8, align 4
  %9 = load i32, ptr %4, align 4
  %10 = icmp ne i32 %9, 0
  %11 = zext i1 %10 to i64
  %12 = select i1 %10, i32 1, i32 2
  store i32 %12, ptr %5, align 4
  %13 = load i32, ptr %5, align 4
  %14 = mul nsw i32 %13, 3
  %15 = add nsw i32 %14, 5
  store i32 %15, ptr %5, align 4
  %16 = load ptr, ptr %3, align 8
  store i32 20, ptr %16, align 4
  %17 = load i32, ptr %5, align 4
  %18 = load i32, ptr %5, align 4
  %19 = mul nsw i32 %17, %18
  store i32 %19, ptr %6, align 4
  %20 = load i32, ptr %6, align 4
  %21 = and i32 %20, 1
  %22 = icmp ne i32 %21, 0
  br i1 %22, label %23, label %26

23:                                               ; preds = %2
  %24 = load i32, ptr %6, align 4
  %25 = add nsw i32 %24, 7
  store i32 %25, ptr %6, align 4
  br label %29

26:                                               ; preds = %2
  %27 = load i32, ptr %6, align 4
  %28 = sub nsw i32 %27, 3
  store i32 %28, ptr %6, align 4
  br label %29

29:                                               ; preds = %26, %23
  %30 = load ptr, ptr %3, align 8
  store i32 30, ptr %30, align 4
  %31 = load ptr, ptr %3, align 8
  %32 = load i32, ptr %31, align 4
  store i32 %32, ptr %7, align 4
  %33 = load i32, ptr %7, align 4
  %34 = load i32, ptr %6, align 4
  %35 = add nsw i32 %33, %34
  ret i32 %35
}

attributes #0 = { noinline nounwind ssp uwtable(sync) "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+altnzcv,+ccdp,+ccidx,+ccpp,+complxnum,+crc,+dit,+dotprod,+flagm,+fp-armv8,+fp16fml,+fptoint,+fullfp16,+jsconv,+lse,+neon,+pauth,+perfmon,+predres,+ras,+rcpc,+rdm,+sb,+sha2,+sha3,+specrestrict,+ssbs,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8a" }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 2, !"SDK Version", [2 x i32] [i32 26, i32 1]}
!1 = !{i32 1, !"wchar_size", i32 4}
!2 = !{i32 8, !"PIC Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 1}
!4 = !{i32 7, !"frame-pointer", i32 1}
!5 = !{!"Homebrew clang version 21.1.2"}
