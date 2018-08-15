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

        [Output("Angle")]
        public ISpread<double> FAngle;
        [Output("Distance")]
        public ISpread<double> FDist;
        [Output("Quality")]
        public ISpread<double> FQ;
        
        [Import()]
        public ILogger FLogger;

        [DllImport("lib-rplidar.dll")]
        public static extern int init();

        [DllImport("lib-rplidar.dll")]
        public static extern void startScan();
        [DllImport("lib-rplidar.dll")] 
        public static extern void stopScan();

        [DllImport("lib-rplidar.dll")] 
        public static extern int update();

        [DllImport("lib-rplidar.dll")] 
        public static extern float getAngle(int i);
        [DllImport("lib-rplidar.dll")] 
        public static extern float getDist(int i);
        [DllImport("lib-rplidar.dll")] 
        public static extern float getQuality(int i);

        [DllImport("lib-rplidar.dll")] 
        public static extern void finish();

        private bool started;
        private int count = 0;

        #endregion fields & pins
        public rplidar() { init(); }
        ~rplidar() { finish(); }

        public void Evaluate(int SpreadMax)
        {
            
            if(started != FEnable[0]){
                    started = FEnable[0];
                    if(started) {
                        FLogger.Log(LogType.Debug, "started");
                        startScan();
                    }
                    else {
                       FLogger.Log(LogType.Debug, "stoped");
                         stopScan();
                    }
            }

            count = update();

            FAngle.SliceCount = count;
            FDist.SliceCount = count;
            FQ.SliceCount = count;

            for(int i = 0; i < count; i++){
                FAngle[i] = getAngle(i);
                FDist[i] = getDist(i);
                FQ[i] = getQuality(i);
            }
        }
    }
}
