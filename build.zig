const std = @import("std");

// Generates a `compile_commands.json` in the root dir for use by clangd
const ccjson = @import("compile_commands");

pub fn build(b: *std.Build) void {

    ////////////////////////////////////////////////////////////////////////////
    // Set project options
    //
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{
        .preferred_optimize_mode = .Debug,
    });

    // Make a list of targets that have include files and C source files
    var targets = std.ArrayList(*std.Build.Step.Compile).init(b.allocator);

    ////////////////////////////////////////////////////////////////////////////
    // Build and install a C++ library
    //
    // TODO(MATT): use this template to build libsndfile, libmp3lame, ...
    const lib = b.addStaticLibrary(.{
        .name = "factorial",
        .target = target,
        .optimize = .Debug,
    });

    lib.addCSourceFiles(.{
        .files = &.{
            "lib/lib.cpp",
        },
        .flags = &.{
            "-std=c++23",
            "-O0",
            // NOTE(matt): Use more flags for building real libraries
        },
    });
    lib.addIncludePath(b.path("include/"));
    lib.linkLibCpp();

    b.installArtifact(lib);

    ////////////////////////////////////////////////////////////////////////////
    // Build executable
    //
    const exe = b.addExecutable(.{
        .name = "auconv",
        .target = target,
        .optimize = optimize,
    });

    targets.append(exe) catch @panic("OOM");

    // Array of .c and .cpp filenames to pass to addCSourceFiles()
    // #includes from any files listed here do not need to be added to this array
    const exe_files = [_][]const u8{
        "src/main.cpp",

        "src/cli.cpp",
        "src/files.cpp",
        "src/mp3.cpp",
        "src/snd.cpp",
    };
    // Array of clang compiler flags to pass to addCSourceFiles()
    const exe_flags = [_][]const u8{
        // Standards and optimization flags
        "-std=c++23", // C++ 23
        "-g", // Include debug information in binary
        "-ggdb3", // Maximizes debug information
        "-Og", // Optimization level: Og = debug, Os = size, O2 = good, O3 = maximum

        // // Warning flags
        "-Wall", // Reasonable default warnings
        "-Wextra", // More reasonable default warnings
        "-Wpedantic", // Enforces ISO C and C++, ensures portability
        // "-Weverything", // EVERY SINGLE WARNING. Introduces conflicts! Do not use!
        "-Walloca", // Warns on use of `alloca`
        "-Wcast-align", // Warns when casts increase required alignment
        "-Wcast-qual", // Warns when type qualifiers are cast away
        "-Wcomma", // Warns on possible misuse of the comma operator
        "-Wconversion", // Warns on implicit conversions that may result in data loss
        "-Wdouble-promotion", // Warns when implicit conversion increases floating-point precision
        "-Wextra-semi", // Warns about redundant semicolons
        "-Wfloat-equal", // Warns when floating point types are compared for in/equality
        "-Wnon-virtual-dtor", // == "-Weffc++": Warns when a class has virtual functions but a non-virtual destructor
        "-Wold-style-cast", // Warns on old C-style casts
        "-Wpointer-arith", // Warns on pointer arithmetic that is invalid or introduces UB
        "-Wrange-loop-analysis", // Warns when a range-based for loop creates an unnecessary copy
        "-Wshadow-all", // Warns when a declaration shadows anything else
        "-Wthread-safety", // Warns on potential race conditions across threads
        "-Wunsafe-buffer-usage", // Warns when a buffer operation is done on a raw pointer
        "-Wunreachable-code-aggressive", // Warns on code that will never be executed
        "-Wunused-template", // Warns on unused templates
        "-Wvla", // Warns on use of C99 variable-length arrays

        // // Rule of 0/3/5/6
        "-Wdeprecated-copy", // Warns when user-declared copy ctor or operator= prevent implicit definition of the other one
        "-Wdeprecated-copy-with-dtor", // Warns when user-declared dtor prevents implicit definition of copy ctor and operator=
        "-Wdeprecated-implementations", // Warns when implementing deprecated or unavailable methods, classes, and categories

        // // Deprecated language features
        "-Wdeprecated", // Warns on use of deprecated C++ language features

        // // Sanitizers / Hardeners
        // NOTE: Do not use the -fsanitize option "unsigned-integer-overflow". We should not be using unsigned integers
        //      except in situations where overflowing is desirable or mandated by the language!
        "-fsanitize=undefined,bounds,implicit-conversion,nullability", // UB Sanitizer
        "-fsanitize-trap=undefined,bounds,implicit-conversion,nullability", // UB Sanitizer handles UB by trapping
        "-fsafe-buffer-usage-suggestions", // "semi-automatically transform large bodies of code to follow the C++ Safe Buffers programming model"
        "-fno-omit-frame-pointer", // Must be set (along with -g) to get proper debug information in the binary. Can remove for release builds.
        "-D_LIBCPP_HARDENING_MODE=_LIBCPP_HARDENING_MODE_DEBUG", // libc++ hardening mode. Note: possibly? inferred by zig cc from -Ox and -fsanitize=undefined flags
        "-ftrivial-auto-var-init=pattern", // Overwrites uninitialized memory with a pattern
        "-ftrapv", // Traps on signed integer overflow (to be consistent with UBSan). Set either this or "-fwrapv"
        // "-fwrapv", // Treats signed integer overflow as two’s complement (wraps around). Set either this or "-ftrapv"
        "-pedantic-errors", // "Reject all forbidden compiler extensions." Also appears to always turn -Wpendantic warnings into errors.

        // // Optimizations (exploit assumption that UB cannot happen in certain cases)
        "-fstrict-aliasing", // Enables optimizations based on the assumption of strict aliasing
        "-fstrict-enums", // Enables optimizations based on the strict definition of an enum’s value range
        "-fstrict-overflow", // Enables optimizations based on strict signed overflow rules
        "-fstrict-return", // Enables optimizations based on treating control flow paths that fall off the end of a non-void function as unreachable
        "-fstrict-vtable-pointers", // Enables optimizations based on the strict rules for overwriting polymorphic C++ objects

        // // Dialect flags
        // "-fno-exceptions", // Disables exception handling
        // "-fno-rtti", // Disables runtime type info (commented-out here because it may be incompatible with a UBSan feature)

        // // Useful flags
        // "-ftime-trace", // Outputs a Chrome Tracing .json object containing a compiler performance report
        // "-fextend-variable-liveness" // clang 20+: preserves the liveness of variables so that they are visible in a debugger more often

        // // Converts warnings into errors -- turn off when needed
        // "-Werror",
        // "-ferror-limit=20",

        // // Flags to DISABLE warnings -- for use with -Werror
        // "-Wno-unused", // Disables warnings for unused variables, functions etc.
        // "-Wno-error=unused", // Disables warnings for unused variables, functions etc.
    };

    // Add a step called "ccjson" (Compile commands DataBase) for making compile_commands.json.
    // To run this step, do `zig build ccjson`
    ccjson.createStep(b, "ccjson", targets.toOwnedSlice() catch @panic("OOM"));

    // Add include paths (one path per folder containing a #include)
    exe.addIncludePath(b.path("include/"));
    exe.addIncludePath(b.path("lib/include/"));
    exe.addIncludePath(b.path("external/"));

    // Link libraries
    // exe.linkLibC(); // when appropriate
    exe.linkLibCpp();
    exe.linkSystemLibrary("mp3lame"); // TODO(MATT): replace with a built-from-source library
    exe.linkSystemLibrary("sndfile"); // TODO(MATT): replace with a built-from-source library
    // e.g. exe.linkSystemLibrary("SDL3"); // when appropriate

    // Add .c and .cpp files along with specified compiler flags
    exe.addCSourceFiles(.{
        .files = &exe_files,
        .flags = &exe_flags,
    });

    b.installArtifact(exe);

    ////////////////////////////////////////////////////////////////////////////
    // Define `run` command
    //
    const run_cmd = b.addRunArtifact(exe);
    run_cmd.step.dependOn(b.getInstallStep());

    // This allows the user to pass arguments to the application in the build
    // command itself, like this: `zig build run -- arg1 arg2 etc`
    if (b.args) |args| {
        run_cmd.addArgs(args);
    }

    const run_step = b.step("run", "Run the app");
    run_step.dependOn(&run_cmd.step);
}
