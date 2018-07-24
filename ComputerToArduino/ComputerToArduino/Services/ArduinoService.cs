using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Text;

namespace ComputerToArduino.Services
{
    public class ArduinoService
    {
        private const int DebounceMs = 500;

        private Dictionary<string, DateTime> CommandExecutions { get; set; } = new Dictionary<string, DateTime>();

        public string[] GetAvailableComPorts()
        {
            return SerialPort.GetPortNames();
        }

        public void SendCommand(SerialPort serialPort, string command, bool value)
        {
            this.SendCommand(serialPort, command, value ? 1 : 0);
        }

        public void SendCommand(SerialPort serialPort, string command, int value)
        {
            if (serialPort == null || !serialPort.IsOpen) return;

            if (value < 0) value = 0;
            if (value > 1023) value = 1023;

            if (!this.CommandExecutions.ContainsKey(command) || DateTime.Now >= this.CommandExecutions[command])
            {
                var serialCommand = $"#{command}{value.ToString("0000")}\n";
                serialPort.Write(serialCommand);

                this.CommandExecutions[command] = DateTime.Now.AddMilliseconds(DebounceMs);
            }
        }
    }
}
