#!/usr/bin/env python

"""Build the game's GDExtension against the sibling godot-cpp checkout."""

env = SConscript("godot-cpp/SConstruct")

# Keep project headers and sources in the conventional src/ directory.
env.Append(CPPPATH=["src"])
sources = Glob("src/*.cpp")

if not sources:
    Exit("No C++ source files were found in src/.")

# godot-cpp supplies the platform, target, architecture, and shared-library
# suffix.  This produces names such as:
#   bin/libracinggame.windows.debug.x86_64.dll
if env["platform"] == "macos":
    library = env.SharedLibrary(
        "bin/libracinggame.{}.{}.framework/libracinggame.{}.{}".format(
            env["platform"],
            env["target"],
            env["platform"],
            env["target"],
        ),
        source=sources,
    )
else:
    library = env.SharedLibrary(
        "bin/libracinggame{}{}".format(env["suffix"], env["SHLIBSUFFIX"]),
        source=sources,
    )

env.NoCache(library)
Default(library)
