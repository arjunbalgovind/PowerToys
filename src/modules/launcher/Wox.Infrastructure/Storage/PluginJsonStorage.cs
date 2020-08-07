﻿// Copyright (c) Microsoft Corporation
// The Microsoft Corporation licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.

using System.IO;

namespace Wox.Infrastructure.Storage
{
    [System.Diagnostics.CodeAnalysis.SuppressMessage("StyleCop.CSharp.DocumentationRules", "SA1649:File name should match first type name", Justification = "Generic, file is named correctly")]
    public class PluginJsonStorage<T> : JsonStorage<T> where T : new()
    {
        public PluginJsonStorage()
        {
            // C# related, add python related below
            var dataType = typeof(T);
            var assemblyName = typeof(T).Assembly.GetName().Name;
            DirectoryPath = Path.Combine(Constant.DataDirectory, DirectoryName, Constant.Plugins, assemblyName);
            Helper.ValidateDirectory(DirectoryPath);

            FilePath = Path.Combine(DirectoryPath, $"{dataType.Name}{FileSuffix}");
        }
    }
}
