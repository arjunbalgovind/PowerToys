﻿// Copyright (c) Microsoft Corporation
// The Microsoft Corporation licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.

using System.Text.Json;
using System.Text.Json.Serialization;

namespace Microsoft.PowerToys.Settings.UI.Lib
{

    // Represents the configuration property of the settings that store Double type.
    public class DoubleProperty
    {
        public DoubleProperty()
        {
            this.Value = 0.0;
        }

        // Gets or sets the double value of the settings configuration.
        [JsonPropertyName("value")]
        public double Value { get; set; }

        // Returns a JSON version of the class settings configuration class.
        public override string ToString()
        {
            return JsonSerializer.Serialize(this);
        }
    }
}
