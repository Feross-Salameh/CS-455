using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Diagnostics;
using System.IO;
using System.Net;
using System.Net.Sockets;

namespace command_gui
{
    public partial class starter : Form
    {
        Dictionary<char, Process> routerDictionary = new Dictionary<char, Process>();
        string folder = "test1";
        public starter()
        {
            InitializeComponent();
        }

        private void btn_start_Click(object sender, EventArgs e)
        {
            string name = tb_name.Text;
            bool check = cb_poisedReverse.Checked;
        }

        private void btn_startFromFolder_Click(object sender, EventArgs e)
        {
            string name = tb_name.Text;
            string path = "";
            bool check = cb_poisedReverse.Checked;
            foreach (KeyValuePair<char, Process> entry in routerDictionary)
                entry.Value.Kill();
            routerDictionary.Clear();
            lb_processList.DataSource = null;
            using (FolderBrowserDialog fbd = new FolderBrowserDialog())
            {
                fbd.SelectedPath = Directory.GetCurrentDirectory();
                if (fbd.ShowDialog() == DialogResult.OK)
                {
                    path = fbd.SelectedPath;
                }
                else
                    return;
            }
            Directory.SetCurrentDirectory(path);
            string[] paths = Directory.GetFiles(path);

            foreach(string str in paths)
            {
                string[] tok = str.Split('\\');
                string file = tok.Last();
                string[] fileSplt = file.Split('.');
                if (fileSplt.Last() == "cfg")
                {
                    Process newProc = new Process();
                    Directory.SetCurrentDirectory("..\\..\\router\\Debug\\");
                    newProc.StartInfo.FileName = Directory.GetCurrentDirectory() + "\\router.exe";
                    newProc.StartInfo.WorkingDirectory = "..\\..\\router\\Debug\\";
                    if (check)
                        newProc.StartInfo.Arguments = " -p";
                    newProc.StartInfo.Arguments += " " + tok[tok.Length - 2] + " ";
                    folder = tok[tok.Length - 2];
                    newProc.StartInfo.Arguments += file[0].ToString();
                    newProc.Start();
                    routerDictionary[file[0]] = newProc;
                    lb_processList.DataSource = new BindingSource(routerDictionary, null);
                    lb_processList.DisplayMember = "Key";
                    lb_processList.ValueMember = "Value";
                    
                }
            }
        }

        private void btn_startSingle_Click(object sender, EventArgs e)
        {
            if(tb_test.Text.Trim() == "")
            {
                MessageBox.Show("Test folder name is emply");
                return;
            }
            if(tb_name.Text.Trim() == "")
            {
                MessageBox.Show("Name text is emply.");
                return;
            }
            Process newProc = new Process();
            try
            {
                Directory.SetCurrentDirectory("..\\..\\..\\..\\router\\Debug\\");
            }
            catch { }
            newProc.StartInfo.FileName = Directory.GetCurrentDirectory() + "\\router.exe";
            newProc.StartInfo.WorkingDirectory = "..\\..\\router\\Debug\\";
            if (cb_poisedReverse.Checked)
                newProc.StartInfo.Arguments = " -p";
            newProc.StartInfo.Arguments += " " + tb_test.Text.Trim() + " ";
            folder = tb_test.Text.Trim();
            newProc.StartInfo.Arguments += tb_name.Text.Trim()[0];
            newProc.Start();
            routerDictionary[tb_name.Text.Trim()[0]] = newProc;
            lb_processList.DataSource = new BindingSource(routerDictionary, null);
            lb_processList.DisplayMember = "Key";
            lb_processList.ValueMember = "Value";
        }

        private void btn_terminateAll_Click(object sender, EventArgs e)
        {
            if(routerDictionary.Count == 0)
            {
                MessageBox.Show("No routers to terminate.");
                return;
            }
            foreach (KeyValuePair<char, Process> entry in routerDictionary)
                entry.Value.Kill();
            routerDictionary.Clear();
            lb_processList.DataSource = null;
        }

        private void btn_terminateSingle_Click(object sender, EventArgs e)
        {
            if (routerDictionary.Count == 0)
            {
                MessageBox.Show("No routers to terminate.");
                return;
            }
            KeyValuePair<char, Process> entry = (KeyValuePair<char, Process>)lb_processList.SelectedItem;
            routerDictionary[entry.Key].Kill();
            routerDictionary.Remove(entry.Key);
            lb_processList.DataSource = new BindingSource(routerDictionary, null);
            lb_processList.DisplayMember = "Key";
            lb_processList.ValueMember = "Value";
        }

        private void btn_print_Click(object sender, EventArgs e)
        {
            if (routerDictionary.Count == 0)
            {
                MessageBox.Show("No routers to print routing table.");
                return;
            }
            if(tb_message.Text.Trim() == "")
            {
                MessageBox.Show("No message to send");
                return;
            }
            int port = -1;
            char router = ((KeyValuePair<char, Process>)lb_processList.SelectedItem).Key;
            string[] lines = File.ReadAllLines("..\\..\\proj2-skeleton\\" + folder + "\\routers");
            foreach(string str in lines)
            {
                string[] tok = str.Split(' ');
                if(tok[0][0] == router)
                {
                    port = Int32.Parse(tok[2]);
                    break;
                }
            }
            if(port == -1)
            {
                MessageBox.Show("Could not find router in routers file.");
                return;
            }
            IPAddress address = IPAddress.Parse("127.0.0.1");
            IPEndPoint endPoint = new IPEndPoint(address, port);
            Socket printSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.IP);
            try
            {
                printSocket.Connect(endPoint);
                Byte[] printRequest = Encoding.ASCII.GetBytes(tb_message.Text.Trim());
                printSocket.Send(printRequest);
                printSocket.Shutdown(SocketShutdown.Both);
                printSocket.Close();
            }
            catch (Exception en)
            {
                MessageBox.Show("Unable to connect to router. Error: " + en.ToString());
            }
        }
    }
}
