const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    //
    // C++ executable
    //
    const exe = b.addExecutable(.{
        .name = "auconv",
        // .root_source_file = b.path("src/main.c"),
        .target = target,
        .optimize = optimize,
    });

    // Add files
    const exe_files = [_][]const u8{
        "src/main.cpp",
    };
    // Set flags
    const exe_flags = [_][]const u8{
        "-std=c++17",
        "-g",
        "-O3",
        "-Wall",
        "-Werror",
    };

    // Link libcpp to build a C++ app
    exe.linkLibCpp();
    exe.linkSystemLibrary("mp3lame");
    exe.linkSystemLibrary("sndfile");
    exe.addCSourceFiles(.{
        .files = &exe_files,
        .flags = &exe_flags,
    });
    exe.addIncludePath(b.path("include"));

    b.installArtifact(exe);

    //
    // 'run' command
    //

    const run_cmd = b.addRunArtifact(exe);
    run_cmd.step.dependOn(b.getInstallStep());
    if (b.args) |args| {
        run_cmd.addArgs(args);
    }
    const run_step = b.step("run", "Run the app");
    run_step.dependOn(&run_cmd.step);
}
