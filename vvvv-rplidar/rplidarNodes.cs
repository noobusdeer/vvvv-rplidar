#region usings
using System;
using System.Runtime.InteropServices;
using System.ComponentModel.Composition;

using VVVV.PluginInterfaces.V1;
using VVVV.PluginInterfaces.V2;
using VVVV.Utils.VColor;
using VVVV.Utils.VMath;

using VVVV.Core.Logging;
#endregion usings

namespace VVVV.Nodes
{
    #region PluginInfo
    [PluginInfo(Name = "Lidar", Category = "Value")]
    #endregion PluginInfo
    public class rplidar : IPluginEvaluate
    {
        #region fields & pins
        [Input("Input", DefaultValue = 1.0)]
        public ISpread<double> FInput;

        [Output("Output")]
        public ISpread<double> FOutput;

        [Import()]
        public ILogger FLogger;

        [DllImport("lib-rplidar.dll")]
        public static extern int testfunc(int n);
        [DllImport("lib-rplidar.dll")]
        public static extern void test();

        #endregion fields & pins

        public void Evaluate(int SpreadMax)
        {
            FOutput.SliceCount = SpreadMax;

            for (int i = 0; i < SpreadMax; i++)
                FOutput[i] = testfunc((int)FInput[i]);

            test();

           // FLogger.Log(LogType.Debug, testfunc(0).ToString());
        }
    }
}
