// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.ReactNative;
using Microsoft.ReactNative.Bridge;
using System.Collections.Generic;

namespace SampleApp
{
  sealed class MainReactNativeHost : ReactNativeHost
  {
    public MainReactNativeHost()
    {
      this.InstanceSettings.UseWebDebugger = false; // Disabled temporarily because of issue #2877
      this.InstanceSettings.UseLiveReload = true; // true by default in debug builds already
      this.InstanceSettings.UseJsi = true;
    }

    protected override string MainComponentName => "SampleApp";
    protected override string JavaScriptMainModuleName => "index.windows";
    protected override IReadOnlyList<IReactPackage> Packages
    {
      get
      {
        return new[] { new SamplePackage() };
      }
    }
  }
}
