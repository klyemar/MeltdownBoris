using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO.Ports;
using ComputerToArduino.Services;
using ComputerToArduino.App;

namespace ComputerToArduino
{
    public partial class ArduinoForm : Form

    {
        String[] Ports { get; set; }

        SerialPort Port { get; set; }

        ArduinoService ArduinoService { get; set; } = new ArduinoService();

        bool IsConnected 
        {
            get { return this.Port != null && this.Port.IsOpen; }
        }

        public ArduinoForm()
        {
            InitializeComponent();
            DisableControls();
            this.Ports = this.ArduinoService.GetAvailableComPorts();

            foreach (string port in this.Ports)
            {
                this.comboBoxPorts.Items.Add(port);
                if (this.Ports[0] != null)
                {
                    this.comboBoxPorts.SelectedItem = this.Ports[0];
                }
            }
        }

        private void ButtonConnectClick(object sender, EventArgs e)
        {
            if (!this.IsConnected)
            {
                ConnectToArduino();
            } else
            {
                DisconnectFromArduino();
            }
        }

        private void ConnectToArduino()
        {
            string selectedPort = this.comboBoxPorts.GetItemText(this.comboBoxPorts.SelectedItem);
            this.Port = new SerialPort(selectedPort, 9600, Parity.None, 8, StopBits.One);
            this.Port.Open();
            this.Port.DataReceived += this.OnSerialData;
            this.buttonSerialConnect.Text = "Disconnect";
            EnableControls();
        }

        private void Hue1CheckboxClicked(object sender, EventArgs e)
        {
            if (sender is CheckBox) this.SendCommand(SerialCommands.Hue1, (sender as CheckBox).Checked);
        }

        private void Hue2CheckboxClicked(object sender, EventArgs e)
        {
            if (sender is CheckBox) this.SendCommand(SerialCommands.Hue2, (sender as CheckBox).Checked);
        }

        private void Hue3CheckboxClicked(object sender, EventArgs e)
        {
            if (sender is CheckBox) this.SendCommand(SerialCommands.Hue3, (sender as CheckBox).Checked);
        }

        private void Hue4CheckboxClicked(object sender, EventArgs e)
        {
            if (sender is CheckBox) this.SendCommand(SerialCommands.Hue4, (sender as CheckBox).Checked);
        }

        private void Hue5CheckboxClicked(object sender, EventArgs e)
        {
            if (sender is CheckBox) this.SendCommand(SerialCommands.Hue5, (sender as CheckBox).Checked);
        }

        private void InvertCheckboxClicked(object sender, EventArgs e)
        {
            if (sender is CheckBox) this.SendCommand(SerialCommands.Invert, (sender as CheckBox).Checked);
        }

        private void PauseCheckboxClicked(object sender, EventArgs e)
        {
            if (sender is CheckBox) this.SendCommand(SerialCommands.Pause, (sender as CheckBox).Checked);
        }

        private void TrackBarHueValueChanged(object sender, EventArgs e)
        {
            if (sender is TrackBar) this.SendCommand(SerialCommands.Speed, (sender as TrackBar).Value);
        }

        private void TrackBarFadeValueChanged(object sender, EventArgs e)
        {
            if (sender is TrackBar) this.SendCommand(SerialCommands.Fade, (sender as TrackBar).Value);
        }

        private void ButtonPatternClicked(object sender, EventArgs e)
        {
            this.SendCommand(SerialCommands.Pattern1);
        }

        private void DisconnectFromArduino()
        {
            if (this.IsConnected)
            {
                this.Port.Close();
            }
            this.Port.DataReceived -= this.OnSerialData;

            this.buttonSerialConnect.Text = "Connect";
            DisableControls();
            ResetDefaults();
        }

        private void EnableControls()
        {
            this.checkBoxHue1.Enabled = true;
            this.checkBoxHue2.Enabled = true;
            this.checkBoxHue3.Enabled = true;
            this.checkBoxHue4.Enabled = true;
            this.checkBoxHue5.Enabled = true;
            this.checkBoxPause.Enabled = true;
            this.checkBoxInvert.Enabled = true;
            this.groupBoxHue.Enabled = true;
            this.trackBarFps.Enabled = true;
            this.trackBarFade.Enabled = true;
            this.buttonPattern.Enabled = true;
        }

        private void DisableControls()
        {
            this.checkBoxHue1.Enabled = false;
            this.checkBoxHue2.Enabled = false;
            this.checkBoxHue3.Enabled = false;
            this.checkBoxHue4.Enabled = false;
            this.checkBoxHue5.Enabled = false;
            this.checkBoxPause.Enabled = false;
            this.checkBoxInvert.Enabled = false;
            this.groupBoxHue.Enabled = false;
            this.trackBarFps.Enabled = false;
            this.trackBarFade.Enabled = false;
            this.buttonPattern.Enabled = false;
        }

        private void ResetDefaults()
        {
            this.checkBoxHue1.Checked = false;
            this.checkBoxHue2.Checked = false;
            this.checkBoxHue3.Checked = false;
            this.checkBoxHue4.Checked = false;
            this.checkBoxHue5.Checked = false;
            this.checkBoxPause.Checked = false;
            this.checkBoxInvert.Checked = false;
            this.trackBarFade.Value = 0;
            this.trackBarFps.Value = 0;
        }

        private void SendCommand(string command)
        {
            if (!this.IsConnected) return;

            this.ArduinoService.SendCommand(this.Port, command, 0);
        }

        private void SendCommand(string command, int value)
        {
            if (!this.IsConnected) return;

            this.ArduinoService.SendCommand(this.Port, command, value);
        }

        private void SendCommand(string command, bool value)
        {
            if (!this.IsConnected) return;

            this.ArduinoService.SendCommand(this.Port, command, value);
        }

        private void OnSerialData(object sender, SerialDataReceivedEventArgs e)
        {
            var port = (SerialPort)sender;
            var data = port.ReadExisting();

            Console.Write(data);
        }
    }
}
