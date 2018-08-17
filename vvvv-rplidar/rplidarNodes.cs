#region usings
using System;
using System.Runtime.InteropServices;
using System.ComponentModel.Composition;
using System.Collections.Generic;
using VVVV.PluginInterfaces.V1;
using VVVV.PluginInterfaces.V2;
using System.IO.Ports;

using VVVV.Core.Logging;
#endregion usings

namespace VVVV.Nodes
{
    #region PluginInfo
    [PluginInfo(Name = "RPLIDAR", Category = "Value")]
    #endregion PluginInfo

    public class rplidar : IPluginEvaluate, IDisposable {

        private bool started;
        private int count = 0;
        private string[] _AvailablePorts;
        private const string CComEnum = "COM1";
        private string CComEnumCashe;
        private bool portChange;


        [ImportingConstructor()]
        public rplidar() { 
            List<string> tPorts = new List<string>(SerialPort.GetPortNames());
            tPorts.Sort();
            _AvailablePorts = tPorts.ToArray();
            EnumManager.UpdateEnum(CComEnum, "COM1", _AvailablePorts);
        }
        #region fields & pins

        [Input("Enable", DefaultValue = 0.0)]
        public ISpread<bool> FEnable;

        [Input("Port Name", EnumName = CComEnum)]
		public IDiffSpread<EnumEntry> FCom;

        [Output("Angle")]
        public ISpread<double> FAngle;

        [Output("Distance")]
        public ISpread<double> FDist;

        [Output("Quality")]
        public ISpread<double> FQ;

        [Output("Connected")]
        public ISpread<bool> FConnection;

        [Import()]
		public ILogger FLogger;

        #endregion fields & pins

        #region DLL Import
    
        [DllImport("lib-rplidar.dll")]
        public static extern int connecting( string portName );

        [DllImport("lib-rplidar.dll")]
        public static extern void startScan();

        [DllImport("lib-rplidar.dll")] 
        public static extern void stopScan();

        [DllImport("lib-rplidar.dll")] 
        public static extern int update();

        [DllImport("lib-rplidar.dll")] 
        public static extern float getAngle( int i );

        [DllImport("lib-rplidar.dll")] 
        public static extern float getDist( int i );

        [DllImport("lib-rplidar.dll")] 
        public static extern float getQuality( int i );

        [DllImport("lib-rplidar.dll")] 
        public static extern void finish();

        #endregion DLL Import

        public void Evaluate( int SpreadMax ) {
            if( FConnection[0] ) {
                count = update();

                FLogger.Log(LogType.Debug, "count: " + count);

                FAngle.SliceCount = count;
                FDist.SliceCount = count;
                FQ.SliceCount = count;

                for(int i = 0; i < count; i++){
                    FAngle[i] = getAngle(i);
                    FDist[i] = getDist(i);
                    FQ[i] = getQuality(i);
                }
            } 

            if( portChange ) {
                portChange = false;
                FConnection[0] = Convert.ToBoolean( connecting("\\\\.\\" + FCom[0].Name) );
            }

            if( CComEnumCashe != FCom[0].Name ) {
                portChange = true;
                CComEnumCashe = FCom[0].Name;
            }

            if( started != FEnable[0] && FConnection[0] ) {
                started = FEnable[0];
                if( started ) {
                    startScan();
                } else {
                    stopScan();
                }
            }      
        }

        public void Dispose() {
            stopScan();
            finish();
        }
    }
}