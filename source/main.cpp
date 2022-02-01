/*  Copyright (c) 2022 Mitya Selivanov
 */

#include <iostream>
#include <node.h>
#include <uv.h>

using std::vector, std::string, std::cout, std::cerr, std::unique_ptr;

int RunNodeInstance(node::MultiIsolatePlatform *platform,
                    vector<string> const       &args,
                    vector<string> const       &exec_args) {
  int exit_code = 0;

  auto errors = vector<string> {};
  auto setup = node::CommonEnvironmentSetup::Create(platform, &errors,
                                                    args, exec_args);

  if (!setup) {
    for (auto const &err : errors)
      cerr << args[0] << ": " << err << '\n';
    return 1;
  }

  auto *isolate = setup->isolate();
  auto *env     = setup->env();

  {
    v8::Locker         locker(isolate);
    v8::Isolate::Scope isolate_scope(isolate);
    v8::Context::Scope context_scope(setup->context());

    auto loadenv_ret = node::LoadEnvironment(
        env, "const publicRequire ="
             "  require('module').createRequire(process.cwd() + '/');"
             "globalThis.require = publicRequire;"
             "require('vm').runInThisContext(process.argv[1]);");

    if (loadenv_ret.IsEmpty())
      return 1;

    exit_code = node::SpinEventLoop(env).FromMaybe(1);

    node::Stop(env);
  }

  return exit_code;
}

auto main(int argc, char **argv) -> int {
  argv           = uv_setup_args(argc, argv);
  auto args      = vector<string>(argv, argv + argc);
  auto exec_args = vector<string> {};
  auto errors    = vector<string> {};

  int exit_code = node::InitializeNodeWithArgs(&args, &exec_args,
                                               &errors);

  for (auto &error : errors) cerr << args[0] << ": " << error << '\n';

  if (exit_code != 0)
    return exit_code;

  auto platform = node::MultiIsolatePlatform::Create(4);

  using v8::V8;
  
  V8::InitializePlatform(platform.get());
  V8::Initialize();

  int ret = RunNodeInstance(platform.get(), args, exec_args);

  V8::Dispose();
  V8::ShutdownPlatform();
  return ret;
}
