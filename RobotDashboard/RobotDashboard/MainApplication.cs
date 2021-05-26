using LiveCharts;
using LiveCharts.Configurations;
using System;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using System.Windows.Forms;
using LiveCharts.Wpf;

namespace RobotDashboard
{
   public partial class Dashboard : Form
   {
      private ConnectForm connectForm;
      private Thread udpServerThread;

      private ChartValues<MeasureModel> ChartValues { get; set; }

      public Dashboard()
      {
         InitializeComponent();

         var mapper = Mappers.Xy<MeasureModel>()
                .X(model => model.DateTime.Ticks)   //use DateTime.Ticks as X
                .Y(model => model.AverageSpeed);           //use the value property as Y

         //lets save the mapper globally.
         Charting.For<MeasureModel>(mapper);

         //the ChartValues property will store our values array
         ChartValues = new ChartValues<MeasureModel>();
         chartSpeed.Series = new SeriesCollection
            {
                new LineSeries
                {
                    Values = ChartValues,
                    PointGeometrySize = 0,
                    StrokeThickness = 1
                }
            };
         chartSpeed.AxisX.Add(new Axis
         {
            DisableAnimations = true,
            LabelFormatter = value => new System.DateTime((long)value).ToString("mm:ss"),
            Separator = new Separator
            {
               Step = TimeSpan.FromSeconds(1).Ticks
            }
         });

         chartSpeed.AxisY.Add(new Axis
         {
            MinValue = 0,
            MaxValue = 150
         });

         SetAxisLimits(System.DateTime.Now);

         //The next code simulates data changes every 500 ms
         System.Windows.Forms.Timer timer = new System.Windows.Forms.Timer
         {
            Interval = 5000
         };
         timer.Tick += TimerOnTick;
         timer.Start();
      }

      /// <summary>
      /// Starts thread of UDP server and allows user to send credentials to ESP32.
      /// </summary>
      /// <param name="sender"></param>
      /// <param name="e"></param>
      private void Dashboard_Load(object sender, EventArgs e)
      {
         // Show form to let user send credentials to ESP32
         connectForm = new ConnectForm();
         connectForm.ShowDialog();

         // Put form at center of screen
         CenterToScreen();

         // Create and run the thread that hosts the UDP server
         udpServerThread = new Thread(HostUDPserver)
         {
            IsBackground = true
         };
         udpServerThread.Start();
      }

      private const int PORTNUMBER = 8888;
      private int speedSum = 0;
      private int speedCount = 0;
      /// <summary>
      /// Opens up port for ESP32 to send its data to.
      /// </summary>
      private void HostUDPserver()
      {
         // Allow ESP32 to send its data to the specified port number 
         IPEndPoint ipep = new IPEndPoint(IPAddress.Any, PORTNUMBER);
         UdpClient newsock = new UdpClient(ipep);
         IPEndPoint sender = new IPEndPoint(IPAddress.Any, 0);

         while (true)
         {
            byte[] data = newsock.Receive(ref sender); // Function that blocks until new data is received.
            speedSum += data[0];
            speedCount += 1;
            AppendTextBox(data[0], data[1]); // Change values on the labels.
         }
      }

      /// <summary>
      /// Method to set the axis limits.
      /// </summary>
      /// <param name="now"></param>
      private void SetAxisLimits(System.DateTime now)
      {
         chartSpeed.AxisX[0].MaxValue = now.Ticks + TimeSpan.FromSeconds(1).Ticks; // Force the axis to be 100 milliseconds ahead
         chartSpeed.AxisX[0].MinValue = now.Ticks - TimeSpan.FromSeconds(10).Ticks; // We keep track of the last 10 seconds
      }

      /// <summary>
      /// Method that gets executed every time the timer reaches the interval.
      /// </summary>
      /// <param name="sender"></param>
      /// <param name="eventArgs"></param>
      private void TimerOnTick(object sender, EventArgs eventArgs)
      {
         var now = DateTime.Now; // Get current time

         // Calculate average speed
         int average = 0;
         if (speedSum != 0 && speedCount != 0)
         {
            average = speedSum / speedCount;
         }

         // Add the time and corresponding average to chart
         ChartValues.Add(new MeasureModel
         {
            DateTime = now,
            AverageSpeed = average
         });
         speedSum = 0;
         speedCount = 0;

         // Set the axis limits
         SetAxisLimits(now);

         // Only use the last 5 values
         if (ChartValues.Count > 5)
         {
            ChartValues.RemoveAt(0);
         }
      }

      /// <summary>
      /// Puts new data to labels.
      /// </summary>
      /// <param name="speed"></param>
      /// <param name="distance"></param>
      private void AppendTextBox(byte speed, byte distance)
      {
         if (InvokeRequired)
         {
            Invoke(new Action<byte, byte>(AppendTextBox), new object[] { speed, distance });
            return;
         }
         lbSpeed.Text = "Current speed: " + Convert.ToString(speed);
         lbDistance.Text = "Distance to nearest obstacle: " + Convert.ToString(distance);
      }

      /// <summary>
      /// Allow user to send network credentials to ESP32.
      /// </summary>
      /// <param name="sender"></param>
      /// <param name="e"></param>
      private void BtnReconnect_Click(object sender, EventArgs e)
      {
         connectForm.ShowDialog();
         speedSum = 0;
         speedCount = 0;
         lbSpeed.Text = "Current speed: 0";
         lbDistance.Text = "Distance to nearest obstacle: 0";
      }

      /// <summary>
      /// Closes UDP server when form closes.
      /// </summary>
      /// <param name="sender"></param>
      /// <param name="e"></param>
      private void Dashboard_FormClosing(object sender, FormClosingEventArgs e)
      {
         if (udpServerThread != null)
         {
            udpServerThread.Abort();
         }
      }
   }
}
