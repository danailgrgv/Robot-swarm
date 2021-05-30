using System;
using System.Diagnostics;
using System.IO.Ports;
using System.Linq;
using System.Management;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using System.Windows.Forms;

namespace RobotDashboard
{
   public partial class ConnectForm : Form
   {
      public ConnectForm()
      {
         InitializeComponent();
      }

      /// <summary>
      /// Put application to center of screen.
      /// </summary>
      /// <param name="sender"></param>
      /// <param name="e"></param>
      private void ConnectForm_Load(object sender, EventArgs e)
      {
         CenterToScreen();
      }

      private Thread sendCredentialsThread;
      /// <summary>
      /// Start thread to send credentials to ESP32.
      /// </summary>
      /// <param name="sender"></param>
      /// <param name="e"></param>
      private void BtnConnect_Click(object sender, EventArgs e)
      {
         // Create new thread to send credentials.
         sendCredentialsThread = new Thread(SendCredentials)
         {
            IsBackground = true
         };
         sendCredentialsThread.Start();

         // Don't allow user to open multiple threads
         lbSSID.Visible = false;
         tbSSID.Visible = false;
         lbPswrd.Visible = false;
         tbPswrd.Visible = false;
         btnConnect.Visible = false;
         lbTime.Visible = true;
         lbWaitInfo.Visible = true;
      }

      private const int PORTNUMBER = 8888;
      /// <summary> Send credentials and other important network information to ESP32.
      /// <para> Find the ESP32 device. Open the corresponding COM port and send a string containing information 
      /// to connect to WiFi. This contains the network name, corresponding password, the IP address of the laptop 
      /// which runs the application and at last the port number. </para>
      /// </summary>
      private void SendCredentials()
      {
         // Start new timer to check if process isn't taking too long
         bool exit_flag = false;
         int secondsPassed = 0;
         System.Timers.Timer timer = new System.Timers.Timer
         {
            Interval = 1000,
         };
         timer.Elapsed += (sender, e) => OnTimedEvent(sender, e, ref exit_flag, ref secondsPassed);
         timer.Enabled = true;
         timer.Start();

         try
         {
            // Find IP address of local host machine
            string localIP = string.Empty;
            using (Socket socket = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, 0))
            {
               socket.Connect("8.8.8.8", 65530);
               IPEndPoint endPoint = socket.LocalEndPoint as IPEndPoint;
               localIP = endPoint.Address.ToString();
               socket.Dispose();
            }

            // Find COM port of ESP32
            string deviceID = string.Empty;
            ManagementScope connectionScope = new ManagementScope();
            SelectQuery serialQuery = new SelectQuery("SELECT * FROM Win32_SerialPort");
            ManagementObjectSearcher searcher = new ManagementObjectSearcher(connectionScope, serialQuery);

            foreach (ManagementObject obj in searcher.Get())
            {
               if (exit_flag == true)
               {
                  throw new InvalidOperationException("Taking too long to find ESP32");
               }

               if (obj["Description"].ToString().Contains("Silicon Labs CP210x"))
               {
                  deviceID = obj["DeviceID"].ToString();
               }
            }
            searcher.Dispose();

            // Throw exception if ESP32 device not found
            if (deviceID == string.Empty)
            {
               throw new InvalidOperationException("ESP32 device not found");
            }

            // Send network information to ESP32
            SerialPort serialPort = new SerialPort
            {
               PortName = deviceID,
               BaudRate = 115200
            };
            serialPort.Open();
            string toSend = $"<{tbSSID.Text}:{tbPswrd.Text}:{localIP}:{PORTNUMBER}>";
            serialPort.WriteLine(toSend);
            serialPort.Close();
            serialPort.Dispose();
         }
         catch // Error handling in case anything went wrong
         {
            timer.Stop();
            timer.Dispose();

            // Allow user to try and send network information to ESP32 again
            Invoke((MethodInvoker)delegate
            {
               lbTime.Text = "0";
               lbTime.Visible = false;
               lbWaitInfo.Visible = false;
               tbSSID.Text = string.Empty;
               tbPswrd.Text = string.Empty;
               btnConnect.Enabled = false;
               lbSSID.Visible = true;
               tbSSID.Visible = true;
               lbPswrd.Visible = true;
               tbPswrd.Visible = true;
               btnConnect.Visible = true;
            });

            // Error message
            string errMessage = "Couldn't connect to ESP32. Try again!";
            string errTitle = "Error";
            MessageBoxButtons errButtons = MessageBoxButtons.OK;
            MessageBoxIcon errIcon = MessageBoxIcon.Error;
            MessageBox.Show(errMessage, errTitle, errButtons, errIcon);

            //Abort this thread
            sendCredentialsThread.Abort();
         }

         timer.Stop();
         timer.Dispose();

         // Set labels for re-use of form later on
         Invoke((MethodInvoker)delegate
         {
            lbTime.Text = "0";
            lbTime.Visible = false;
            lbWaitInfo.Visible = false;
            tbSSID.Text = string.Empty;
            tbPswrd.Text = string.Empty;
            btnConnect.Enabled = false;
            lbSSID.Visible = true;
            tbSSID.Visible = true;
            lbPswrd.Visible = true;
            tbPswrd.Visible = true;
            btnConnect.Visible = true;
         });

         // Success message
         string message = "Credentials sent to ESP32!" + Environment.NewLine +
            "Warning: ESP32 might not be connected to network due to incorrect credentials.";
         string title = "Success";
         MessageBoxButtons buttons = MessageBoxButtons.OK;
         MessageBoxIcon icon = MessageBoxIcon.Information;
         MessageBox.Show(message, title, buttons, icon);

         // Close the form
         Invoke((MethodInvoker)delegate
         {
            Close();
         });
      }

      /// <summary>
      /// Enable button if textboxes are not empty. Otherwise disable button.
      /// </summary>
      private void SetConnectButton()
      {
         if (tbSSID_flag == true && tbPswrd_flag == true)
         {
            btnConnect.Enabled = true;
         }
         else
         {
            btnConnect.Enabled = false;
         }
      }

      /// <summary>
      /// Event to set label to time passed since beginning of thread. Also signals
      /// the object searcher to stop searching if it takes too long.
      /// </summary>
      /// <param name="sender"></param>
      /// <param name="e"></param>
      /// <param name="exit_flag"></param>
      /// <param name="secondsPassed"></param>
      private void OnTimedEvent(object sender, EventArgs e, ref bool exit_flag, ref int secondsPassed)
      {
         secondsPassed += 1; // Keep track of seconds passed

         string seconds = secondsPassed.ToString();
         Invoke((MethodInvoker)delegate
         {
            lbTime.Text = seconds.ToString(); // Set label to seconds passed
         });

         if (secondsPassed == 30) // Tell thread to stop searching if timer reached 30 seconds
         {
            exit_flag = true;
         }
      }

      private bool tbSSID_flag = false;
      /// <summary>
      /// Check if textbox is empty.
      /// </summary>
      /// <param name="sender"></param>
      /// <param name="e"></param>
      private void TbSSID_TextChanged(object sender, EventArgs e)
      {
         if (tbSSID.TextLength > 0)
         {
            tbSSID_flag = true;
         }
         else
         {
            tbSSID_flag = false;
         }
         SetConnectButton();
      }

      private bool tbPswrd_flag = false;
      /// <summary>
      /// Check if textbox is empty.
      /// </summary>
      /// <param name="sender"></param>
      /// <param name="e"></param>
      private void TbPswrd_TextChanged(object sender, EventArgs e)
      {
         if (tbPswrd.TextLength > 0)
         {
            tbPswrd_flag = true;
         }
         else
         {
            tbPswrd_flag = false;
         }
         SetConnectButton();
      }

      /// <summary> Only allow form to close if application is not busy connecting.
      /// </summary>
      /// <param name="sender"></param>
      /// <param name="e"></param>
      private void ConnectForm_FormClosing(object sender, FormClosingEventArgs e)
      {
         if (new StackTrace().GetFrames().Any(x => x.GetMethod().Name == "Close")) { }
         else
         {
            if (sendCredentialsThread != null)
            {
               if (sendCredentialsThread.IsAlive)
               {
                  e.Cancel = true; // Keep form open if thread is still busy
                  return;
               }
               else
               {
                  sendCredentialsThread.Abort(); // Throw away thread if it was not in use
               }
            }
         }
      }
   }
}
