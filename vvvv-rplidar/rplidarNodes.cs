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

        [Input("Enable", DefaultValue = 0.0)]
        public ISpread<bool> FEnable;

        [Output("Output")]
        public ISpread<double> FOutput;

        [Import()]
        public ILogger FLogger;

        [DllImport("lib-rplidar.dll")]
        public static extern int init();

        [DllImport("lib-rplidar.dll")]
        public static extern void startScan();

        [DllImport("lib-rplidar.dll")] 
        public static extern void stopScan();

        private bool started;
        private int deb;

        #endregion fields & pins
        public rplidar() {
            init();
        }

        public void Evaluate(int SpreadMax)
        {
            FOutput.SliceCount = SpreadMax;
            for (int i = 0; i < SpreadMax; i++){
                if(started != FEnable[i]){
                    started = FEnable[i];
                    if(started) {
                        FLogger.Log(LogType.Debug, "started");
                        startScan();
                    }
                    else {
                       FLogger.Log(LogType.Debug, "stoped");
                         stopScan();
                    }
                }
                FOutput[i] = deb;
             }
        }
    }
}
