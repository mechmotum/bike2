# Copyright 2023 Toyota Motor Corporation. All rights reserved.
# Copyright 2022-2023 Woven Planet. All rights reserved.
"""
Sets up Python environment with Pip dependencies for users of @bazel_tooling.
"""

load("@python3_10//:defs.bzl", "interpreter")
load("@rules_python//python:pip.bzl", "pip_parse")
load("//python:requirements.bzl", "install_deps")

def tooling_python_setup():
    pip_parse(
        name = "pip",
        python_interpreter_target = interpreter,
        requirements = "//python:requirements.txt",
    )

    install_deps()
