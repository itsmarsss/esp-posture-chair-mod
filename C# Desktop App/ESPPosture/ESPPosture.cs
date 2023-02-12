using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Net;
using System.Net.NetworkInformation;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace ESPPosture
{
    public partial class Form : System.Windows.Forms.Form
    {
        public Form()
        {
            InitializeComponent();
        }

        private Thread udpListener;
        private UdpClient udpClient;

        private void Form_Load(object sender, EventArgs e)
        {
            udpListener = new Thread(new ThreadStart(serverThread));
            updateBtn.PerformClick();
        }

        private void serverThread()
        {
            int port;
            try
            {
                port = Int32.Parse(portBox.Text);
            }
            catch (Exception e)
            {
                AddToLogs("~~~ Invalid Port (Must be 0 - 65535) ~~~");
                return;
            }

            if (port < 0 || port > 65535)
            {
                AddToLogs("~~~ Invalid Port (Must be 0 - 65535) ~~~");
                return;
            }

            AddToLogs("~~~ Listening At Port [" + portBox.Text + "] ~~~");


            udpClient = new UdpClient(port);

            while (true)
            {
                try
                {

                    IPEndPoint RemoteIpEndPoint = new IPEndPoint(IPAddress.Any, port);
                    Byte[] receiveBytes = udpClient.Receive(ref RemoteIpEndPoint);
                    string data = Encoding.ASCII.GetString(receiveBytes);
                    AddToLogs(RemoteIpEndPoint.Address.ToString() + " : " + DateTime.Now.ToString("yyyy-MM-dd hh:mm:ss") + " : " + data.ToString());

                }
                catch (Exception e) { }
            }
        }

        private void AddToLogs(string str)
        {
            this.Invoke(new MethodInvoker(delegate ()
            {
                logs.Items.Add(str);
                logs.SelectedIndex = logs.Items.Count - 1;
                logs.SelectedIndex = -1;
            }));
        }

        private void updateBtn_Click(object sender, EventArgs e)
        {
            if (udpClient != null)
            {
                udpClient.Close();
            }

            udpListener = new Thread(new ThreadStart(serverThread));
            udpListener.Start();
        }

        protected override CreateParams CreateParams
        {
            get
            {
                CreateParams cp = base.CreateParams;
                cp.ClassStyle = cp.ClassStyle | 0x200;
                return cp;
            }
        }
    }
}
