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

        private long previous = DateTimeOffset.Now.ToUnixTimeMilliseconds();
        private Boolean moved = false;
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
                    IPEndPoint iep = new IPEndPoint(IPAddress.Any, port);
                    Byte[] bytes = udpClient.Receive(ref iep);
                    string data = Encoding.ASCII.GetString(bytes);
                    AddToLogs(iep.Address.ToString() + " : " + DateTime.Now.ToString("yyyy-MM-dd hh:mm:ss") + " : " + data.ToString());
                    if (DateTimeOffset.Now.ToUnixTimeMilliseconds() - previous > 1500)
                    {
                        if (data.ToString().Equals("top"))
                        {
                            SendTop();
                            moved = false;
                        }
                        else if (data.ToString().Equals("low"))
                        {
                            SendBack();
                            moved = false;
                        }
                        else if (data.ToString().Equals("both"))
                        {
                            SendBoth();
                            moved = false;
                        }
                    }
                    if (data.ToString().Equals("sleep"))
                    {
                        SendSleep();
                    }
                    else if (data.ToString().Equals("test"))
                    {
                        SendTest();
                    }
                    else if (data.ToString().Equals("move"))
                    {
                        if (!moved)
                        {
                            SendMove();
                        }
                        moved = true;
                    }

                }
                catch (Exception e) { }
            }
        }

        private void SendTop()
        {
            this.Invoke(new MethodInvoker(delegate ()
            {
                notifyTop.Icon = new Icon(Path.GetFullPath("ESPPosture.ico"));
                notifyTop.Visible = true;
                notifyTop.ShowBalloonTip(0);
                previous = DateTimeOffset.Now.ToUnixTimeMilliseconds();
            }));
        }

        private void SendBack()
        {
            this.Invoke(new MethodInvoker(delegate ()
            {
                notifyBack.Icon = new Icon(Path.GetFullPath("ESPPosture.ico"));
                notifyBack.Visible = true;
                notifyBack.ShowBalloonTip(0);
                previous = DateTimeOffset.Now.ToUnixTimeMilliseconds();
            }));
        }

        private void SendBoth()
        {
            this.Invoke(new MethodInvoker(delegate ()
            {
                notifyBoth.Icon = new Icon(Path.GetFullPath("ESPPosture.ico"));
                notifyBoth.Visible = true;
                notifyBoth.ShowBalloonTip(0);
                previous = DateTimeOffset.Now.ToUnixTimeMilliseconds();
            }));
        }

        private void SendSleep()
        {
            this.Invoke(new MethodInvoker(delegate ()
            {
                notifySleep.Icon = new Icon(Path.GetFullPath("ESPPosture.ico"));
                notifySleep.Visible = true;
                notifySleep.ShowBalloonTip(0);
                previous = DateTimeOffset.Now.ToUnixTimeMilliseconds();
            }));
        }

        private void SendTest()
        {
            this.Invoke(new MethodInvoker(delegate ()
            {
                notifyTest.Icon = new Icon(Path.GetFullPath("ESPPosture.ico"));
                notifyTest.Visible = true;
                notifyTest.ShowBalloonTip(0);
                previous = DateTimeOffset.Now.ToUnixTimeMilliseconds();
            }));
        }

        private void SendMove()
        {
            this.Invoke(new MethodInvoker(delegate ()
            {
                notifyMove.Icon = new Icon(Path.GetFullPath("ESPPosture.ico"));
                notifyMove.Visible = true;
                notifyMove.ShowBalloonTip(0);
                previous = DateTimeOffset.Now.ToUnixTimeMilliseconds();
            }));
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
