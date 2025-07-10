const std = @import("std");

pub fn build(b: *std.Build) void {

    ////////////////////////////////////////////////////////////////////////////
    // Set project options
    //
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{
        .preferred_optimize_mode = .Debug,
    });

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

    // Array of .c and .cpp filenames to pass to addCSourceFiles()
    // #includes from any files listed here do not need to be added to this array
    const exe_files = [_][]const u8{
        "src/main.cpp",
    };
    // Array of clang compiler flags to pass to addCSourceFiles()
    const exe_flags = [_][]const u8{
        // Standards and optimization flags
        "-std=c++23", // C++ 23
        "-g", // Include debug information in binary
        "-ggdb3", // Maximizes debug information
        "-O0", // Optimization level: Og = debug, Os = size, O3 = maximum

        // Warning flags
        "-Wall", // Reasonable default warnings
        "-Wextra", // More reasonable default warnings
        "-Wpedantic", // Enforces ISO C and C++, ensures portability
        // "-Weverything",// EVERY SINGLE WARNING, introduces conflicts, do not use
        "-Wcast-qual", // Warns when type qualifiers are cast away
        "-Wcast-align", // Warns when casts increase required alignment
        "-Wconversion", // Warns on implicit conversion that may result in data loss
        "-Wfloat-equal", // Warns when floating point types are compared for in/equality
        "-Wmismatched-tags", // Warns against previous declarations
        "-Wnon-virtual-dtor", // == "-Weffc++": Warns when class has virtual functions but non-virtual destructor
        "-Wshadow-all", // Warns when a declaration shadows anything else
        "-Wstrict-prototypes", // Warns on f() rather than f(void) for C projects
        "-Wthread-safety", // Warns on potential race conditions across threads
        "-Wvla", // Warns when using C99 variable-length arrays
        "-Wsign-conversion", // Warns on sign conversion
        "-Wunsafe-buffer-usage", // Warns when a buffer operation is done on a raw pointer
        "-Wunused", // Warns on unused variables, functions etc.
        "-Wunused-parameter", // Warns on unused parameters
        "-Wunused-template", // Warns on unused templates

        // Sanitizers / Hardeners
        // "-fsanitize=undefined,bounds,implicit-conversion,nullability,unsigned-integer-overflow", // UB Sanitizer
        // "-fsanitize-trap=undefined,bounds,implicit-conversion,nullability,unsigned-integer-overflow", // UB Sanitizer handles UB by trapping
        "-fno-omit-frame-pointer", // Must be set (along with -g) to get proper debug information in the binary
        "-D_LIBCPP_HARDENING_MODE=_LIBCPP_HARDENING_MODE_DEBUG", // libc++ hardening mode. Note: possibly? inferred by zig cc from -Ox and -fsanitize=undefined flags
        "-ftrivial-auto-var-init=pattern", // Overwrites uninitialized memory with a pattern
        "-pedantic-errors", // "Reject all forbidden compiler extensions." Also appears to always turn -Wpendantic warnings into errors.

        // Dialect flags
        // "-fno-exceptions", // Disables exception handling
        // "-fno-rtti", // Disables runtime type info (commented-out here because it may be incompatible with a UBSan feature)

        // Useful flags
        // "-fwrapv", // Traps on signed integer overflow (to be consistent with UBSan). Set either this or "-fwrapv"
        // "-fwrapv", // Treats signed integer overflow as two’s complement (wraps around). Set either this or "-ftrapv"
        "-fstrict-aliasing", // Enables optimizations based on the assumption of strict aliasing
        "-fstrict-enums", // Enables optimizations based on the strict definition of an enum’s value range
        "-ftime-trace", // Outputs a Chrome Tracing .json object containing a compiler performance report

        // // Flags to DISABLE warnings -- for use with -Werror
        // "-Wno-cast-qual", // Disables warning when a pointer is cast so as to remove a type qualifier
        // "-Wno-conversion", // Disables warning on implicit conversion that may result in data loss
        // "-Wno-float-equal", // Disables warning when floating-point values used in equality comparisons
        // "-Wno-ignored-qualifiers", // Disables warning when return type of a function has a type qualifier
        // "-Wno-non-virtual-dtor", // Disables warning when class has virtual functions but non-virtual destructor
        // "-Wno-sign-compare", // Disables warning when a comparison between signed and unsigned values could produce an incorrect result
        // "-Wno-sign-conversion", // Disables warning on implicit conversion between un/signed
        // "-Wno-unused", // Disables warnings for unused variables, functions etc.
        // "-Wno-unused-parameter", // Disables unused parameter warnings
        // "-Wno-unused-template", // Disables unused template warnings

        // // Converts warnings into errors -- turn off when needed
        // "-Werror",
        // "-ferror-limit=0",

        // // Enable Tracy profiler
        // "-DTRACY_ENABLE",
    };

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
